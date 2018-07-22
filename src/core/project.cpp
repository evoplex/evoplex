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

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QMutexLocker>
#include <QVector>
#include <QStringList>
#include <QTextStream>
#include <set>

#include "project.h"
#include "experiment.h"
#include "utils.h"

namespace evoplex {

Project::Project(MainApp* mainApp, int id)
    : m_mainApp(mainApp),
      m_id(id)
{
}

Project::~Project()
{
    for (auto& e : m_experiments) {
        m_mainApp->expMgr()->remove(e.second);
        e.second->setAutoDeleteTrials(true);
        e.second->setExpStatus(Status::Invalid);
        e.second->pause();
    }
    m_experiments.clear();
}

bool Project::init(QString& error, const QString& filepath)
{
    Q_ASSERT_X(m_experiments.empty(), "Project", "a project cannot be initialized twice");
    setFilePath(filepath);
    if (!filepath.isEmpty()) {
        blockSignals(true);
        importExperiments(filepath, error);
        blockSignals(false);
    }
    m_hasUnsavedChanges = false;
    return !error.isEmpty();
}

void Project::setFilePath(const QString& path)
{
    m_filepath = path;
    m_name = path.isEmpty() ? QString("Project%1").arg(m_id)
                            : QFileInfo(path).baseName();
}

void Project::playAll()
{
    for (auto& i : m_experiments)
        i.second->play();
}

int Project::generateExpId() const
{
    return m_experiments.empty() ? 0 : (--m_experiments.end())->first + 1;
}

ExperimentPtr Project::newExperiment(ExpInputs* inputs, QString& error)
{
    QMutexLocker locker(&m_mutex);

    if (!inputs) {
        error += "Null inputs!";
        return nullptr;
    }

    int expId = inputs->general(GENERAL_ATTRIBUTE_EXPID).toInt();
    if (m_experiments.count(expId)) {
        error += "The Experiment Id must be unique!";
        return nullptr;
    }

    ExperimentPtr exp = std::make_shared<Experiment>(m_mainApp, expId, shared_from_this());
    m_experiments.insert({expId, exp});
    exp->setInputs(inputs, error);

    m_hasUnsavedChanges = true;
    emit (hasUnsavedChanges(m_hasUnsavedChanges));
    emit (expAdded(expId));
    return exp;
}

bool Project::removeExperiment(int expId, QString& error)
{
    QMutexLocker locker(&m_mutex);

    auto it = m_experiments.find(expId);
    if (it == m_experiments.cend()) {
        error += "tried to remove a nonexistent experiment";
        return false;
    }

    ExperimentPtr exp = (*it).second;
    if (m_experiments.erase(exp->id()) < 1) {
        error += "failed to remove the experiment";
        return false;
    }

    emit (expRemoved(expId));
    m_mainApp->expMgr()->remove(exp);
    exp->setAutoDeleteTrials(true);
    exp->setExpStatus(Status::Invalid);
    exp->pause();

    m_hasUnsavedChanges = true;
    emit (hasUnsavedChanges(m_hasUnsavedChanges));
    return true;
}

bool Project::editExperiment(int expId, ExpInputs* newInputs, QString& error)
{
    QMutexLocker locker(&m_mutex);

    auto it = m_experiments.find(expId);
    if (it == m_experiments.cend()) {
        error += "tried to edit a nonexistent experiment";
        return false;
    }
    if (!(*it).second->setInputs(newInputs, error)) {
        return false;
    }
    m_hasUnsavedChanges = true;
    emit (hasUnsavedChanges(m_hasUnsavedChanges));
    emit (expEdited(expId));
    return true;
}

int Project::importExperiments(const QString& filePath, QString& error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error += QString("Couldn't read the experiments from:\n'%1'\n"
                 "Please, make sure it is a readable csv file.\n").arg(filePath);
        qWarning() << error;
        return 0;
    }

    QTextStream in(&file);

    // read header
    const QStringList header = in.readLine().split(",");
    if (header.isEmpty()) {
        error += QString("Couldn't read the experiments from:\n'%1'\n"
                 "The header must have the following columns: %2\n")
                 .arg(filePath).arg(m_mainApp->generalAttrsScope().keys().join(", "));
        qWarning() << error;
        return 0;
    }

    // import experiments
    int row = 1;
    while (!in.atEnd()) {
        const QStringList values = in.readLine().split(",");
        QString expErrorMsg;
        ExpInputs* inputs = ExpInputs::parse(m_mainApp, header, values, expErrorMsg);
        if (!expErrorMsg.isEmpty()) {
            error += QString("Row %1 : Warning: %2\n").arg(row).arg(expErrorMsg);
        }
        expErrorMsg.clear();
        if (!inputs || !newExperiment(inputs, expErrorMsg)) {
            error += QString("Row %1 (skipped): Critical error: %2\n").arg(row).arg(expErrorMsg);
        }
        if (!expErrorMsg.isEmpty()) {
            error += QString("Row %1 : Warning: %2\n").arg(row).arg(expErrorMsg);
        }
        ++row;
    }
    file.close();

    if (row == 1) {
        error += QString("This file is empty.\n"
                 "There were no experiments to be read.\n'%1'\n").arg(filePath);
    }

    if (!error.isEmpty()) {
        error += QString("`%1`\n").arg(filePath);
        qWarning() << error;
    }

    return row - 1;
}

bool Project::saveProject(QString& errMsg, std::function<void(int)>& progress)
{
    if (m_experiments.empty()) {
        errMsg = QString("Unable to save the project '%1'.\n"
                "This project is empty. There is nothing to save.").arg(name());
        qWarning() << errMsg;
        return false;
    }

    QFile file(m_filepath);
    QFileInfo fi(file);
    if (fi.suffix() != "csv" || !file.open(QFile::WriteOnly | QFile::Truncate)) {
        errMsg = QString("Unable to save the project '%1'.\n"
                "Please, make sure the path below corresponds to a writable csv file!\n%2")
                .arg(name()).arg(m_filepath);
        qWarning() << errMsg;
        return false;
    }

    const float kProgress = (2.f * m_experiments.size()) / 100.f;
    int _progress = 0;

    // join the header of all experiments
    std::vector<QString> header;
    QString lModelId, lGraphId;
    for (auto const& i : m_experiments) {
        if (i.second->modelId() == lModelId && i.second->graphId() == lGraphId) {
            continue;
        }
        lModelId = i.second->modelId();
        lGraphId = i.second->graphId();
        std::vector<QString> h = i.second->inputs()->exportAttrNames();
        header.insert(header.end(), h.begin(), h.end());
        _progress += kProgress;
        progress(_progress);
    }
    // remove duplicates
    std::set<QString> s(header.begin(), header.end());
    header.assign(s.begin(), s.end());

    // write header to file
    QTextStream out(&file);
    for (size_t h = 0; h < header.size()-1; ++h) {
        out << header.at(h) << ",";
    }
    out << header.at(header.size() - 1) << "\n";

    // write values to file
    for (auto const& i : m_experiments) {
        const ExperimentPtr exp = i.second;
        const ExpInputs* inputs = exp->inputs();
        const QString modelId_ = exp->modelId() + "_";
        const QString graphId_ = exp->graphId() + "_";

        QString values;
        for (QString attrName : header) {
            Value v;
            if (attrName.startsWith(modelId_)) {
                v = inputs->model(attrName.remove(modelId_));
            } else if (attrName.startsWith(graphId_)) {
                v = inputs->graph(attrName.remove(graphId_));
            } else {
                v = inputs->general(attrName);
            }
            values.append(v.toQString() + ","); // will leave empty if not found
        }
        values.remove(values.size()-1, 1); // remove last comma
        out << values << "\n";

        _progress += kProgress;
        progress(_progress);
    }
    file.close();

    m_mainApp->addPathToRecentProjects(m_filepath);

    m_hasUnsavedChanges = false;
    emit (hasUnsavedChanges(false));
    progress((100));
    qDebug() << "a project has been saved!" << name();
    return true;
}

}
