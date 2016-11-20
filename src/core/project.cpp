/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QtDebug>

#include "core/project.h"
#include "utils/utils.h"

Project::Project(MainApp* mainApp, int projId, int modelId, const QString& name,
                 const QString& descr, const QString& dir)
    : m_mainApp(mainApp)
    , m_projId(projId)
    , m_modelId(modelId)
    , m_model(mainApp->getModel(modelId))
    , m_name(name)
    , m_description(descr)
    , m_dir(dir)
{
}

Project::~Project()
{
    delete m_model;
    m_model = NULL;
}

int Project::runExperiment(const int expId)
{
    if (!m_experiments.contains(expId)) {
        return -1;
    }
    return m_mainApp->getProcessesMgr()->addAndPlay(m_experiments.value(expId));
}

Simulation* Project::newExperiment(const QVariantHash& generalParams, const QVariantHash& modelParams)
{
    // load agents
    QString agentsPath = generalParams.value(GENERAL_PROPERTY_NAME_AGENTS).toString();
    QVector<IAgent*> agents = getAgentsFromFile(agentsPath);
    if (agents.isEmpty()) {
        qWarning() << "[Project] The initial population chosen is invalid for this model!";
        qDeleteAll(agents);
        return NULL;
    }

    // create graph
    QString graphType = generalParams.value(GENERAL_PROPERTY_NAME_GRAPHTYPE).toString();
    Graph* graph = new Graph(agents, graphType);
    if (graph && !graph->isValid()) {
        qWarning() << "[Project] Initial Population", "Unable to create the graph!";
        delete graph;
        return NULL;
    }

    int expId = m_experiments.size();
    IModel* modelObj = m_model->factory->create();
    Simulation* sim = new Simulation(expId, m_projId, m_modelId, modelObj, generalParams);
    if (!modelObj->init(graph, modelParams) || !sim->isValid()) {
        delete modelObj;
        delete sim;
        return NULL;
    }

    m_experiments.insert(expId, sim);
    return sim;
}

QList<Simulation*> Project::importExperiments(const QString& filePath, QVariantHash& generalParams, QVariantHash& modelParams)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Project]: unable to read csv file with the experiments." << filePath;
        return QList<Simulation*>();
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = in.readLine().split(",");
        foreach (QString p, header) {
            if (!m_mainApp->getGeneralProperties().contains(p)
                    && !m_model->defaultModelParams.contains(p)) {
                    header.clear();
                    break;
            }
        }
    }

    if (header.isEmpty()) {
        header << m_mainApp->getGeneralProperties()
               << m_model->defaultModelParams.keys();
        qWarning() << "[Project]: unable to read the experiments from" << filePath
                   << "The header must have:" << header;
        return QList<Simulation*>();
    }

    // import experiments
    QList<Simulation*> experiments;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "[Project]: rows must have the same number of columns!";
            return QList<Simulation*>();
        }

        // get the value of each property
        for (int i = 0; i < values.size(); ++i) {
            if (m_mainApp->getGeneralProperties().contains(header.at(i))) {
                generalParams.insert(header.at(i), values.at(i));
            } else {
                modelParams.insert(header.at(i), values.at(i));
            }
        }

        // create the experiment
        Simulation* sim = newExperiment(generalParams, modelParams);
        if (sim) {
            experiments.append(sim);
        }
    }
    file.close();

    return experiments;
}

QVector<IAgent*> Project::getAgentsFromFile(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[Project]: unable to read csv file with the set of agents." << filePath;
        return QVector<IAgent*>();
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = in.readLine().split(",");
        if (header.size() == m_model->agentParamsDomain.size()) {
            foreach (QString prop, header) {
                if (!m_model->agentParamsDomain.contains(prop)) {
                    header.clear();
                    break;
                }
            }
        } else {
            header.clear();
        }
    }

    if (header.isEmpty()) {
        qWarning() << "[Project]: unable to read the set of agents from" << filePath
                   << "Expected properties:" << m_model->agentParamsDomain.keys();
        return QVector<IAgent*>();
    }

    // create agents
    IModel* model = m_model->factory->create();
    QList<IAgent*> agents;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "[Project]: rows must have the same number of columns!";
            return QVector<IAgent*>();
        }

        IAgent* agent = model->newDefaultAgent();
        for (int i = 0; i < values.size(); ++i) {
            QString domain = m_model->agentParamsDomain.value(header.at(i)).toString();
            if (!Utils::validateProperty(domain, values.at(i))) {
                return QVector<IAgent*>();
            }
            agent->setProperty(header.at(i), QVariant(values.at(i)));
        }
        agents.append(agent);
    }

    delete model;
    file.close();

    return agents.toVector();
}
