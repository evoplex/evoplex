/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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

Project::Project(MainApp* mainApp, int id, const QString& name, const QString& dest)
    : m_mainApp(mainApp)
    , m_id(id)
    , m_name(name)
    , m_dest(dest)
    , m_hasUnsavedChanges(false)
    , m_lastExpId(-1)
{
    if (m_name.isEmpty()) {
        m_name = QString("Project%1").arg(id);
    }
}

Project::~Project()
{
    qDeleteAll(m_experiments);
}

void Project::playAll()
{
    QHash<int, Experiment*>::iterator it;
    for (it = m_experiments.begin(); it != m_experiments.end(); ++it)
        it.value()->play();
}

Experiment* Project::newExperiment(Experiment::ExperimentInputs* inputs)
{
    if (!inputs) {
        return nullptr;
    }

    ++m_lastExpId;
    Experiment* exp = new Experiment(m_mainApp, m_lastExpId, m_id, inputs);
    m_experiments.insert(m_lastExpId, exp);

    m_hasUnsavedChanges = true;
    emit (hasUnsavedChanges(m_hasUnsavedChanges));
    emit (expAdded(m_lastExpId));
    return exp;
}

bool Project::editExperiment(int expId, Experiment::ExperimentInputs* newInputs)
{
    Experiment* exp = m_experiments.value(expId);
    Q_ASSERT(exp);
    if (!exp->init(newInputs)) {
        return false;
    }
    exp->reset();
    emit (expEdited(expId));
    return true;
}

const int Project::importExperiments(const QString& filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[FileMgr]: unable to read csv file with the experiments." << filePath;
        return -1;
    }

    QTextStream in(&file);

    // read header
    const QStringList header = in.readLine().split(",");
    if (header.isEmpty()) {
        qWarning() << "[FileMgr]: unable to read the experiments from" << filePath
                   << "The header must have:" << m_mainApp->generalAttrSpace();
        return -1;
    }

    // import experiments
    int failures = 0;
    int row = 1;
    while (!in.atEnd()) {
        const QStringList values = in.readLine().split(",");
        QString errorMsg;
        Experiment::ExperimentInputs* inputs = Experiment::readInputs(m_mainApp, header, values, errorMsg);
        if (!inputs || !newExperiment(inputs)) {
            qWarning() << "[FileMgr]: unable to read the experiment at" << row << filePath
                       << "\nError: \"" << errorMsg;
            ++failures;
        }
        ++row;
    }
    file.close();
    return failures == row - 1 ? -1 : failures;
}

bool Project::saveProject(QString& errMsg, std::function<void(int)>& progress)
{
    progress(0);
    if (m_experiments.empty()) {
        errMsg = QString("Unable to save %1.\n"
                "This project is empty. There is nothing to save.").arg(m_name);
        qWarning() << "[Project]" << errMsg;
        return false;
    }

    QDir dir(m_dest);
    if (m_dest.isEmpty() || m_name.isEmpty() || !dir.mkpath(m_dest)) {
        errMsg = QString("Unable to save %1.\n"
                "The destination is invalid!\n%2").arg(m_name).arg(m_dest);
        qWarning() << "[Project]" << errMsg;
        return false;
    }

    progress(5);
    QFile experimentsFile(dir.absoluteFilePath(m_name + ".csv"));
    if (!experimentsFile.open(QFile::WriteOnly | QFile::Truncate)) {
        errMsg = QString("Unable to save %1.\n"
                "Make sure the destination directory is writtable.\n%2")
                .arg(m_name).arg(dir.absolutePath());
        qWarning() << "[Project]" << errMsg;
        return false;
    }

    progress(10);
    std::vector<QString> header;
    for (Experiment* exp : m_experiments) {
        std::vector<QString> general = exp->generalAttrs()->names();
        header.insert(header.end(), general.begin(), general.end());
        // prefix all model attributes with the modelId
        foreach (const QString& attrName, exp->modelAttrs()->names()) {
            header.emplace_back(exp->modelId() + "_" + attrName);
        }
        // prefix all graph attributes with the graphId
        foreach (const QString& attrName, exp->graphAttrs()->names()) {
            header.emplace_back(exp->graphId() + "_" + attrName);
        }
    }
    // remove duplicates
    progress(25);
    std::set<QString> s(header.begin(), header.end());
    header.assign(s.begin(), s.end());

    progress(30);
    QTextStream out(&experimentsFile);
    for (int i = 0; i < header.size()-1; ++i) {
        out << header.at(i) << ",";
    }
    out << header.at(header.size()-1) << "\n";

    progress(35);
    for (Experiment* exp : m_experiments) {
        const Attributes* generalAttrs = exp->generalAttrs();
        const Attributes* modelAttrs = exp->modelAttrs();
        const Attributes* graphAttrs = exp->graphAttrs();
        const QString modelId_ = exp->modelId() + "_";
        const QString graphId_ = exp->graphId() + "_";

        QStringList values;
        foreach (QString attrName, header) {
            int idx = generalAttrs->indexOf(attrName);
            if (idx != -1) {
                values.append(generalAttrs->value(idx).toQString());
            } else if (attrName.startsWith(modelId_)) {
                idx = modelAttrs->indexOf(attrName.remove(modelId_));
                if (idx != -1) values.append(modelAttrs->value(idx).toQString());
            } else if (attrName.startsWith(graphId_)) {
                idx = graphAttrs->indexOf(attrName.remove(graphId_));
                if (idx != -1) values.append(graphAttrs->value(idx).toQString());
            } else {
                values.append(""); // not found; leave empty
            }
        }
        out << values.join(",") << "\n";
    }
    experimentsFile.close();
    m_hasUnsavedChanges = false;
    emit (hasUnsavedChanges(false));
    progress((100));
    qWarning() << "[Project]: project has been saved!" << m_name;
    return true;
}

}
