/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtDebug>
#include <QFile>

#include "graph.h"
#include "core/utils.h"

namespace evoplex {

CustomGraph::CustomGraph(const QString& name)
    : AbstractGraph(name)
{
}

bool CustomGraph::init()
{
    m_filePath = attrs()->value(FilePath).toString;
    if (m_filePath.isEmpty()) {
        qWarning() << "[CustomGraph]: file path cannot be empty.";
        return false;
    }
    return true;
}

void CustomGraph::reset()
{
    qDeleteAll(m_edges);
    m_edges.clear();
    Edges().swap(m_edges);

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "[CustomGraph]: unable to read csv file with the set of agents." << m_filePath;
        return;
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = in.readLine().split(",");
        if (header.size() < 2 || header.at(0) != "origin" || header.at(1) != "target") {
            qWarning() << "[CustomGraph]: header is invalid."
                       << "It should have at least three columns: 'origin', 'target' and 'directed'."
                       << m_filePath;
            return;
        }
    }

    // create edges
    int row = 0;
    bool isValid = true;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "[CustomGraph]: rows must have the same number of columns!"
                       << m_filePath << "Row: " << row;
            isValid = false;
            break;
        }

        bool ok1, ok2;
        int originId = values.at(0).toInt(&ok1);
        int targetId = values.at(1).toInt(&ok2);
        if (!ok1 || !ok2) {
            qWarning() << "[CustomGraph]: invalid values."
                       << "The values for 'origin' and 'target' must be integers."
                       << m_filePath << "Row: " << row;
            isValid = false;
            break;
        } else if (originId < 0 || originId >= m_agents.size() ||
                   targetId < 0 || targetId >= m_agents.size()) {
            qWarning() << "[CustomGraph]: invalid values. 'origin' or 'target' are not in the set of agents."
                          << m_filePath << "Row: " << row;
            isValid = false;
            break;
        }

        m_edges.emplace_back(new Edge(m_agents.at(originId), m_agents.at(targetId), type() ==  Directed));
        ++row;
    }
    file.close();

    if (!isValid) {
        qDeleteAll(m_edges);
        m_edges.clear();
        Edges().swap(m_edges);
    }
}


} // evoplex
REGISTER_GRAPH(CustomGraph)
#include "graph.moc"
