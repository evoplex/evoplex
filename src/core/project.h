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

#include <QObject>
#include <QEnableSharedFromThis>

#include "abstractmodel.h"
#include "experiment.h"
#include "mainapp.h"

namespace evoplex
{

class Project : public QObject, public QEnableSharedFromThis<Project>
{
    Q_OBJECT

public:
    Project(MainApp* mainApp, int id);
    ~Project() {}

    bool init(QString& error, const QString& filepath="");

    void destroyExperiments();

    // Add a new experiment to this project.
    // @return nullptr if unsuccessful
    Experiment* newExperiment(Experiment::ExperimentInputs* inputs, QString& error);

    // Edit an experiment of this project.
    bool editExperiment(int expId, Experiment::ExperimentInputs* newInputs, QString& error);

    // Import a set of experiments from a csv file. It stops if an experiment fails.
    // return the number of experiments imported.
    int importExperiments(const QString& filePath, QString& errorMsg);

    // Save project into the dest directory.
    // A project is composed of plain csv files
    bool saveProject(QString& errMsg, std::function<void(int)>& progress);

    // execute all experiments of this project.
    void playAll();

    inline const QString& name() const { return m_name; }
    inline const QString& filepath() const { return m_filepath; }
    void setFilePath(const QString& path);

    inline Experiment* experiment(int expId) const { return m_experiments.at(expId); }
    inline const std::map<int, Experiment*>& experiments() const { return m_experiments; }

    inline int id() const { return m_id; }
    inline bool hasUnsavedChanges() const { return m_hasUnsavedChanges; }

    // generate a valid experiment id
    int generateExpId() const;

signals:
    void expAdded(Experiment* exp);
    void expEdited(const Experiment* exp);
    void hasUnsavedChanges(bool);

private:
    MainApp* m_mainApp;
    const int m_id;
    QString m_filepath;
    QString m_name;
    bool m_hasUnsavedChanges;
    std::map<int, Experiment*> m_experiments;
};
}

#endif // PROJECT_H
