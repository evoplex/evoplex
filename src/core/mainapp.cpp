/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QPluginLoader>
#include <QtDebug>

#include "core/graph.h"
#include "core/mainapp.h"
#include "core/project.h"

MainApp::MainApp()
    : m_processes(new Processes())
{
}

MainApp::~MainApp()
{
    foreach (Project* p, m_openedProjects) {
        delete p;
        p = NULL;
    }
    m_openedProjects.clear();
}

QString MainApp::loadModel(QString path)
{
    QPluginLoader loader(path);
    QObject* obj = loader.instance();
    if (!obj) {
        qWarning() << "[MainApp] unable to load the model" << path;
        return QString();
    }

    Model* model = new Model();
    model->factory = qobject_cast<IModelFactory*>(obj);
    model->name = model->factory->name();
    model->description = model->factory->description();
    model->author = model->factory->author();
    if (model->name.isEmpty()) {
        qWarning() << "[MainApp] unable to load the model. Name cannot be empty" << path;
        return QString();
    }

    IModel* mi = model->factory->create();
    const QMetaObject* meta = mi->metaObject();
    for(int i = 1; i < meta->propertyCount(); ++i) { // skip objectName
        QMetaProperty p = meta->property(i);
        model->params.insert(p.name(), p.read(mi));
    }

    m_models.insert(model->name, model);
    return model->name;
}

Project* MainApp::newProject(const QString& modelName, const QString& name,
        const QString& descr, const QString& dir)
{
    Project* p = new Project(*this, m_models.value(modelName), name, descr, dir);
    m_openedProjects.append(p);
    return p;
}
