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
#include <QVector>
#include <QStringList>
#include <QTextStream>
#include <set>

#include "project.h"
#include "experiment.h"
#include "utils.h"

namespace evoplex
{

Project::Project(MainApp* mainApp, int id)
    : m_mainApp(mainApp)
    , m_id(id)
{
}

bool Project::init(QString& error, const QString& filepath)
{
    setFilePath(filepath);
    if (!filepath.isEmpty()) {
        this->blockSignals(true);
        importExperiments(filepath, error);
        this->blockSignals(false);
    }
    m_hasUnsavedChanges = false;
    return error.isEmpty();
}

void Project::destroyExperiments()
{
    for (auto& it : m_experiments) {
        m_mainApp->expMgr()->destroy(it.second);
    }
    m_experiments.clear();
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

Experiment* Project::newExperiment(ExpInputs* inputs, QString& error)
{
    if (!inputs) {
        error += "Null inputs!";
        return nullptr;
    }

    int expId = inputs->general(GENERAL_ATTRIBUTE_EXPID).toInt();
    if (m_experiments.count(expId)) {
        error += "The Experiment Id must be unique!";
        return nullptr;
    }

    Experiment* exp = new Experiment(m_mainApp, inputs, sharedFromThis());
    m_experiments.insert({expId, exp});

    m_hasUnsavedChanges = true;
    emit (hasUnsavedChanges(m_hasUnsavedChanges));
    emit (expAdded(exp));
    return exp;
}

bool Project::editExperiment(int expId, ExpInputs* newInputs, QString& error)
{
    Experiment* exp = m_experiments.at(expId);
    Q_ASSERT_X(exp, "Experiment", "tried to edit a nonexistent experiment");
    if (!exp->init(newInputs, error)) {
        return false;
    }
    m_hasUnsavedChanges = true;
    emit (hasUnsavedChanges(m_hasUnsavedChanges));
    emit (expEdited(exp));
    return true;
}

int Project::importExperiments(const QString& filePath, QString& errorMsg)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        errorMsg = "Couldn't read the experiments from:\n`" + filePath + "`"
                 + "Please, make sure it is a readable csv file.";
        qWarning() << errorMsg;
        return 0;
    }

    QTextStream in(&file);

    // read header
    const QStringList header = in.readLine().split(",");
    if (header.isEmpty()) {
        errorMsg = "Couldn't read the experiments from:\n`" + filePath + "`"
                 + "\nThe header must have: " + m_mainApp->generalAttrsScope().keys().join(", ");
        qWarning() << errorMsg;
        return 0;
    }

    // import experiments
    int row = 1;
    while (!in.atEnd()) {
        const QStringList values = in.readLine().split(",");
        QString expErrorMsg;
        ExpInputs* inputs = ExpInputs::parse(m_mainApp, header, values, expErrorMsg);
        if (!inputs || !newExperiment(inputs, expErrorMsg)) {
            errorMsg = QString("Couldn't read the experiment at line %1 from:\n`%2`\n"
                               "Error: %3").arg(row).arg(filePath).arg(expErrorMsg);
            qWarning() << errorMsg;
            delete inputs;
            file.close();
            return row - 1;
        }
        ++row;
    }
    file.close();

    if (row == 1) {
        errorMsg = QString("This file is empty.\nThere were no experiments to be read.\n%1").arg(filePath);
        qWarning() << errorMsg;
        return 0;
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
        const Experiment* exp = i.second;
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
        values.replace(values.size(), "\n");
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
