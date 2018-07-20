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
    Q_ASSERT_X(m_exp, "Trial", "a trial must belong to a valid experiment");
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
    return  m_exp.data()->graphType();
}

bool Trial::init(const ExperimentPtr& exp)
{
    if (!exp || exp->expStatus() == Status::Invalid || exp->pauseAt() < 0) {
        return false;
    }

    Nodes nodes = exp->cloneCachedNodes(m_id);
    if (nodes.empty() && !exp->createNodes(nodes)) {
        return false;
    }

    const quint16 seed = static_cast<quint16>(exp->inputs()->general(GENERAL_ATTRIBUTE_SEED).toInt());
    m_prg = new PRG(seed + m_id);

    m_graph = dynamic_cast<AbstractGraph*>(exp->graphPlugin()->create());
    if (!m_graph || !m_graph->setup(*this, *exp->inputs()->graph(), nodes)) {
        qWarning() << "unable to create the trials."
                   << "The graph could not be initialized."
                   << "Experiment:" << exp->id();
        return false;
    }

    m_model = dynamic_cast<AbstractModel*>(exp->modelPlugin()->create());
    if (!m_model || !m_model->setup(*this, *exp->inputs()->model())) {
        qWarning() << "unable to create the trials."
                   << "The model could not be initialized."
                   << "Experiment:" << exp->id();
        return false;
    }

    if (!exp->inputs()->fileCaches().empty()) {
        const QString fpath = exp->m_filePathPrefix + QString("%4.csv").arg(m_id);
        QFile file(fpath);
        if (file.open(QFile::WriteOnly | QFile::Truncate)) {
            QTextStream stream(&file);
            stream << exp->m_fileHeader;
            file.close();
        } else {
            qWarning() << "unable to create the trials. Could not write in " << fpath;
            return false;
        }

        // write this initial step to file
        for (OutputPtr output : exp->m_outputs) {
            output->doOperation(this);
        }
        writeCachedSteps(exp);
    }

    // make the set of nodes available for other trials
    if (exp->numTrials() > 1 && exp->m_clonableNodes.empty()) {
        exp->m_clonableNodes = Utils::clone(nodes);
    }

    m_step = 0; // important!

    return true;
}

void Trial::run()
{
    ExperimentPtr exp = m_exp.toStrongRef();
    if (!exp || exp->expStatus() == Status::Invalid) {
        m_status = Status::Invalid;
    }

    if (m_status == Status::Invalid || m_status == Status::Running
            || m_status == Status::Finished) {
        exp->trialFinished(this);
        return;
    }

    if (m_status == Status::Disabled) {
        // Ensure we init one trial at a time.
        // Thus, if one trial fail, the others will be aborted earlier.
        exp->m_mutex.lock();
        if (!init(exp)) {
            exp->m_mutex.unlock();
            m_status = Status::Invalid;
            exp->trialFinished(this);
            return;
        }
        exp->m_mutex.unlock();
        m_graph->reset();
    }

    m_status = Status::Running;
    emit (exp->trialCreated(m_id));

    if (!runSteps(exp) || m_step >= exp->stopAt()) {
        if (writeCachedSteps(exp)) {
            m_status = Status::Finished;
        } else {
            m_status = Status::Invalid;
        }
    } else {
        m_status = Status::Paused;
    }

    exp->trialFinished(this);
}

bool Trial::runSteps(const ExperimentPtr& exp)
{
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

bool Trial::writeCachedSteps(const ExperimentPtr& exp) const
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
