/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MAINAPP_H
#define MAINAPP_H

#include <QHash>
#include <QObject>
#include <QVector>

#include "plugininterfaces.h"
#include "attributes.h"
#include "graphplugin.h"
#include "modelplugin.h"

namespace evoplex {

class ExperimentsMgr;
class Project;

class MainApp : public QObject
{
    Q_OBJECT

public:
    explicit MainApp();
    ~MainApp();

    // loads plugin from a .so file; return true nullptr if unsuccessful
    const AbstractPlugin* importPlugin(const QString& path, QString& error);

    // Create a new project
    Project* newProject(const QString& name="", const QString& dest="");

    Project* openProject(const QString& filepath, QString& error);

    void closeProject(int projId);

    inline quint16 defaultDelay() const { return m_defaultDelay; }
    inline void setDefaultDelay(quint16 msec) { m_defaultDelay = msec; }

    inline ExperimentsMgr* expMgr() const { return m_experimentsMgr; }
    inline const QHash<QString, GraphPlugin*>& graphs() const { return m_graphs; }
    inline const QHash<QString, ModelPlugin*>& models() const { return m_models; }
    inline const QHash<int, Project*>& projects() const { return m_projects; }

    inline const GraphPlugin* graph(const QString& graphId) const { return m_graphs.value(graphId, nullptr); }
    inline const ModelPlugin* model(const QString& modelId) const { return m_models.value(modelId, nullptr); }
    inline Project* project(int projId) const { return m_projects.value(projId); }

    inline const AttributesSpace& generalAttrSpace() const { return m_generalAttrSpace; }

signals:
    void projectCreated(const Project* p);

private:
    ExperimentsMgr* m_experimentsMgr;
    quint16 m_defaultDelay; // msec

    int m_lastProjectId;
    QHash<int, Project*> m_projects; // opened projects.

    QHash<QString, GraphPlugin*> m_graphs;  // loaded graphs
    QHash<QString, ModelPlugin*> m_models;  // loaded models

    // lets build a hash with the name and space of the essential parameters
    // it is important to validate the contents of csv files
    AttributesSpace m_generalAttrSpace;

    // load plugin from a .so file; return nullptr if unsuccessful
    const AbstractPlugin* loadPlugin(const QString& path, QString& error);
};
}

#endif // MAINAPP_H
