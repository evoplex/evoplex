/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QThread>

#include "abstractgraph.h"
#include "abstractmodel.h"
#include "nodes_p.h"
#include "trial.h"
#include "project.h"
#include "utils.h"

namespace evoplex {

Trial::Trial(const quint16 id, ExperimentPtr exp)
    : m_id(id),
      m_exp(exp),
      m_step(-1), // important! a trial starts from -1
      m_status(Status::Disabled),
      m_prg(nullptr),
      m_graph(nullptr),
      m_model(nullptr)
{
    Q_ASSERT_X(exp, "Trial", "a trial must belong to a valid experiment");
    // important! Trials are deleted by the Experiment class,
    // let's turn off the autoDelete from QRunnable by default
    setAutoDelete(false);
}

Trial::~Trial()
{
    delete m_graph;
    delete m_model;
    delete m_prg;
}

const QString& Trial::graphId() const
{
    return m_exp->graphPlugin()->id();
}

GraphType Trial::graphType() const
{
    return  m_exp->graphType();
}

bool Trial::init()
{
    if (m_exp->expStatus() == Status::Invalid || m_exp->pauseAt() < 0) {
        return false;
    }

    Nodes nodes = m_exp->cloneCachedNodes(m_id);
    if (nodes.empty()) {
        nodes = m_exp->createNodes();
        if (nodes.empty()) {
            return false;
        }
    }

    bool ok = false;
    auto edgeAttrsGen = m_exp->edgeAttrsGen(ok);
    if (!ok) {
        qWarning() << "unable to create the trials."
                   << "Failed to create the edges' attributes generator.\n"
                   << "Experiment:" << m_exp->id();
        return false;
    }

    const quint32 seed = m_exp->inputs()->general(GENERAL_ATTR_SEED).toUInt();
    m_prg = new PRG(seed + m_id);

    m_graph = dynamic_cast<AbstractGraph*>(m_exp->graphPlugin()->create());
    if (!m_graph || !m_graph->setup(m_exp->graphId(), m_exp->graphType(), *m_prg,
                                    std::move(edgeAttrsGen), nodes, *m_exp->inputs()->graph())) {
        qWarning() << "unable to create the trials."
                   << "The graph could not be initialized."
                   << "Experiment:" << m_exp->id();
        return false;
    }

    m_model = dynamic_cast<AbstractModel*>(m_exp->modelPlugin()->create());
    if (!m_model || !m_model->setup(*this, *m_exp->inputs()->model())) {
        qWarning() << "unable to create the trials."
                   << "The model could not be initialized."
                   << "Experiment:" << m_exp->id();
        return false;
    }

    if (!m_exp->inputs()->fileCaches().empty()) {
        const QString fpath = m_exp->m_filePathPrefix + QString("%4.csv").arg(m_id);
        QFile file(fpath);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream stream(&file);
            stream << m_exp->m_fileHeader;
            file.close();
        } else {
            qWarning() << "unable to create the trials. Could not write in " << fpath;
            return false;
        }

        // write this initial step to file
        for (auto const& output : m_exp->m_outputs) {
            output->doOperation(this);
        }
        writeCachedSteps(m_exp.get());
    }

    // make the set of nodes available for other trials
    if (m_exp->numTrials() > 1 && m_exp->m_clonableNodes.empty()) {
        m_exp->m_clonableNodes = NodesPrivate::clone(nodes);
    }

    m_step = 0; // important!

    // set-up the edges for the first time
    if (!m_graph->reset()) {
        qWarning() << "unable to create the trials."
                   << "The graph could not be initialized."
                   << "Experiment:" << m_exp->id();
        return false;
    }

    return true;
}

void Trial::run()
{
    if (m_exp->expStatus() == Status::Invalid) {
        m_status = Status::Invalid;
    }

    if (m_status == Status::Invalid || m_status == Status::Running
            || m_status == Status::Finished) {
        m_exp->trialFinished(this);
        return;
    }

    if (m_status == Status::Disabled) {
        // Ensure we init one trial at a time.
        // Thus, if one trial fail, the others will be aborted earlier.
        m_exp->m_mutex.lock();
        if (!init()) {
            m_exp->m_mutex.unlock();
            m_status = Status::Invalid;
            m_exp->trialFinished(this);
            return;
        }
        m_exp->m_mutex.unlock();
    }

    m_status = Status::Running;
    emit (m_exp->trialCreated(m_id));

    if (!runSteps() || m_step >= m_exp->stopAt()) {
        if (writeCachedSteps(m_exp.get())) {
            m_status = Status::Finished;
        } else {
            m_status = Status::Invalid;
        }
    } else {
        m_status = Status::Paused;
    }

    m_exp->trialFinished(this);
}

bool Trial::runSteps()
{
    const Experiment* exp = m_exp.get();

    QElapsedTimer t;
    t.start();

    m_model->beforeLoop();

    bool hasNext = true;
    while (m_step < exp->pauseAt() && hasNext) {
        hasNext = m_model->algorithmStep();
        ++m_step;

        for (const OutputPtr& output : exp->m_outputs) {
            output->doOperation(this);
        }

        if (m_step % exp->m_mainApp->stepsToFlush() == 0 && !writeCachedSteps(exp)) {
            m_status = Status::Invalid;
            return false;
        }

        if (exp->delay() > 0) {
            QThread::msleep(exp->delay());
        }
    }

    m_model->afterLoop();

    qDebug() << QString("[E%1:T%2] %3s").arg(exp->id())
                .arg(m_id).arg(t.elapsed() / 1000);

    return hasNext;
}

bool Trial::writeCachedSteps(const Experiment* exp) const
{
    if (exp->inputs()->fileCaches().empty() ||
            exp->inputs()->fileCaches().front()->isEmpty(m_id)) {
        return true;
    }

    const QString fpath = exp->m_filePathPrefix + QString("%1.csv").arg(m_id);
    QFile file(fpath);
    if (!file.open(QFile::WriteOnly | QFile::Append)) {
        qWarning() << "unable to create the trials. Could not write in " << fpath;
        return false;
    }

    QTextStream stream(&file);
    do {
        QString row;
        for (Cache* cache : exp->inputs()->fileCaches()) {
            Values vals = cache->readFrontRow(m_id).second;
            cache->flushFrontRow(m_id);
            for (auto const& val : vals) {
                row += val.toQString() + ",";
            }
        }
        row.chop(1);
        stream << row << "\n";

    // we synchronously flush all the io stuff. So, it's safe to say
    // that if the front Output is empty, then all others are also empty.
    } while (!exp->inputs()->fileCaches().front()->isEmpty(m_id));

    file.close();
    return true;
}

} // evoplex
