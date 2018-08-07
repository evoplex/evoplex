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

#ifndef PROJECT_H
#define PROJECT_H

#include <memory>

#include <QMutex>
#include <QObject>

#include "abstractmodel.h"
#include "experiment.h"
#include "mainapp.h"

namespace evoplex {

class Project;
using ProjectPtr = std::shared_ptr<Project>;
using ProjectWPtr = std::weak_ptr<Project>;
using Experiments = std::map<int, ExperimentPtr>;

class Project : public QObject, public std::enable_shared_from_this<Project>
{
    Q_OBJECT

public:
    explicit Project(MainApp* mainApp, int id);
    ~Project();

    bool init(QString& error, const QString& filepath="");

    // Add a new experiment to this project.
    // @return nullptr if unsuccessful
    ExperimentPtr newExperiment(ExpInputsPtr inputs, QString& error);

    // Remove an experiment from this project
    // @return false if unsuccessful
    bool removeExperiment(int expId, QString& error);

    // Edit an experiment of this project.
    bool editExperiment(int expId, ExpInputsPtr newInputs, QString& error);

    // Import a set of experiments from a csv file. It stops if an experiment fails.
    // return the number of experiments imported.
    int importExperiments(const QString& filePath, QString& error);

    // Save project into the dest directory.
    // A project is composed of plain csv files
    bool saveProject(QString& errMsg, std::function<void(int)>& progress);

    // execute all experiments of this project.
    void playAll();

    void setFilePath(const QString& path);

    // generate a valid experiment id
    int generateExpId() const;

    inline int id() const;
    inline const QString& name() const;
    inline const QString& filepath() const;
    inline ExperimentPtr experiment(int expId) const;
    inline const Experiments& experiments() const;
    inline bool hasUnsavedChanges() const;
    inline bool isRunning() const;

signals:
    void expAdded(int expId);
    void expEdited(int expId);
    void expRemoved(int expId);
    void hasUnsavedChanges(bool);

private:
    QMutex m_mutex;
    MainApp* m_mainApp;
    const int m_id;
    QString m_filepath;
    QString m_name;
    bool m_hasUnsavedChanges;
    Experiments m_experiments;
};

inline const QString& Project::name() const
{ return m_name; }

inline const QString& Project::filepath() const
{ return m_filepath; }

inline ExperimentPtr Project::experiment(int expId) const {
    Experiments::const_iterator it = m_experiments.find(expId);
    return (it != m_experiments.end()) ? m_experiments.at(expId) : nullptr;
}

inline const Experiments& Project::experiments() const
{ return m_experiments; }

inline int Project::id() const
{ return m_id; }

inline bool Project::hasUnsavedChanges() const
{ return m_hasUnsavedChanges; }

inline bool Project::isRunning() const
{
    for (auto const& e : m_experiments) {
        if (e.second->expStatus() == Status::Running ||
                e.second->expStatus() == Status::Queued) {
            return true;
        }
    }
    return false;
}

} //evoplex
#endif // PROJECT_H
