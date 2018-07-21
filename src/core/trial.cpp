/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QElapsedTimer>
#include <QFile>
#include <QFileInfo>
#include <QThread>

#include "abstractgraph.h"
#include "abstractmodel.h"
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
    if (nodes.empty() && !m_exp->createNodes(nodes)) {
        return false;
    }

    const quint16 seed = static_cast<quint16>(m_exp->inputs()->general(GENERAL_ATTRIBUTE_SEED).toInt());
    m_prg = new PRG(seed + m_id);

    m_graph = dynamic_cast<AbstractGraph*>(m_exp->graphPlugin()->create());
    if (!m_graph || !m_graph->setup(*this, *m_exp->inputs()->graph(), nodes)) {
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
        for (OutputPtr output : m_exp->m_outputs) {
            output->doOperation(this);
        }
        writeCachedSteps(m_exp.get());
    }

    // make the set of nodes available for other trials
    if (m_exp->numTrials() > 1 && m_exp->m_clonableNodes.empty()) {
        m_exp->m_clonableNodes = Utils::clone(nodes);
    }

    m_step = 0; // important!

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
        m_graph->reset();
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
            for (Value val : vals) {
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
