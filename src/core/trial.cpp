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

#include "trial.h"
#include "abstractgraph.h"
#include "abstractmodel.h"
#include "project.h"
#include "utils.h"

namespace evoplex {

Trial::Trial(const quint16 id, Experiment* exp)
    : m_id(id),
      m_exp(exp),
      m_step(-1),
      m_status(Experiment::UNSET),
      m_prg(nullptr),
      m_graph(nullptr),
      m_model(nullptr)
{
    setAutoDelete(false);
}

Trial::~Trial()
{
    delete m_graph;
    delete m_model;
    delete m_prg;
}

bool Trial::init()
{
    if (m_exp->expStatus() == Experiment::INVALID || m_exp->pauseAt() == 0) {
        return false;
    }

    Nodes nodes = m_exp->cloneCachedNodes(m_id);
    if (nodes.empty()) {
        m_exp->createNodes(nodes);
    }

    const quint16 seed = static_cast<quint16>(m_exp->inputs()->general(GENERAL_ATTRIBUTE_SEED).toInt());
    m_prg = new PRG(seed + m_id);

    m_graph = dynamic_cast<AbstractGraph*>(m_exp->graphPlugin()->create());
    if (!m_graph || !m_graph->setup(*this, *m_exp->inputs()->graph(), nodes)) {
        qWarning() << "unable to create the trials."
                   << "The graph could not be initialized."
                   << "Project:" << m_exp->project()->name()
                   << "Experiment:" << m_exp->id();
        return false;
    }

    m_model = dynamic_cast<AbstractModel*>(m_exp->modelPlugin()->create());
    if (!m_model || !m_model->setup(*this, *m_exp->inputs()->model())) {
        qWarning() << "unable to create the trials."
                   << "The model could not be initialized."
                   << "Project:" << m_exp->project()->name()
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
            output->doOperation(m_id, m_model);
        }
        writeCachedSteps();
    }

    // make the set of nodes available for other trials
    if (m_exp->numTrials() > 1 && m_exp->m_clonableNodes.empty()) {
        m_exp->m_clonableNodes = Utils::clone(nodes);
    }

    return true;
}

void Trial::run()
{
    if (m_exp->expStatus() == Experiment::INVALID) {
        m_status = Experiment::INVALID;
        return;
    }

    if (m_status == Experiment::UNSET) {
        // Ensure we init one trial at a time.
        // Thus, if one trial fail, the others will be aborted earlier.
        m_exp->m_mutex.lock();
        if (!init()) {
            m_status = Experiment::INVALID;
            m_exp->m_expStatus = Experiment::INVALID;
            m_exp->pause();
            m_exp->m_mutex.unlock();
            return;
        }
        m_exp->m_mutex.unlock();
        m_graph->reset();
    }

    m_status = Experiment::RUNNING;
    emit (m_exp->trialCreated(m_id));

    if (!runSteps() || m_step >= m_exp->stopAt()) {
        if (writeCachedSteps()) {
            m_status = Experiment::FINISHED;
        } else {
            m_status = Experiment::INVALID;
            m_exp->setExpStatus(Experiment::INVALID);
            m_exp->pause();
        }
    } else {
        m_status = Experiment::READY;
    }

    m_exp->m_mainApp->expMgr()->finished(m_exp, m_id);
}

bool Trial::runSteps()
{
    QElapsedTimer t;
    t.start();

    bool hasNext = true;
    while (m_step < m_exp->pauseAt() && hasNext) {
        hasNext = m_model->algorithmStep();
        ++m_step;

        for (const OutputPtr& output : m_exp->m_outputs) {
            output->doOperation(m_id, m_model);
        }

        if (m_step % m_exp->m_mainApp->stepsToFlush() == 0 && !writeCachedSteps()) {
            m_status = Experiment::INVALID;
            m_exp->setExpStatus(Experiment::INVALID);
            m_exp->pause();
            return false;
        }

        if (m_exp->delay() > 0) {
            QThread::msleep(m_exp->delay());
        }
    }

    qDebug() << QString("%1 (E%2:T%3) - %4s")
                .arg(m_exp->project()->name()).arg(m_exp->id()).arg(m_id)
               .arg(t.elapsed() / 1000);

    return hasNext;
}

bool Trial::writeCachedSteps()
{
    if (m_exp->inputs()->fileCaches().empty() ||
            m_exp->inputs()->fileCaches().front()->isEmpty(m_id)) {
        return true;
    }

    const QString fpath = m_exp->m_filePathPrefix + QString("%1.csv").arg(m_id);
    QFile file(fpath);
    if (!file.open(QFile::WriteOnly | QFile::Append)) {
        qWarning() << "unable to create the trials. Could not write in " << fpath;
        return false;
    }

    QTextStream stream(&file);
    do {
        QString row;
        for (Cache* cache : m_exp->inputs()->fileCaches()) {
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
    } while (!m_exp->inputs()->fileCaches().front()->isEmpty(m_id));

    file.close();
    return true;
}

} // evoplex
