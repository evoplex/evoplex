/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#include <QtDebug>
#include <QFile>

#include "plugin.h"

namespace evoplex {

CustomGraph::CustomGraph(const QString& name)
    : AbstractGraph(name)
{
}

bool CustomGraph::init()
{
    m_filePath = attrs()->value(FilePath).toString();
    if (m_filePath.isEmpty()) {
        qWarning() << "file path cannot be empty.";
        return false;
    }
    return true;
}

void CustomGraph::reset()
{
    Utils::deleteAndShrink(m_edges);

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "unable to read csv file with the set of nodes." << m_filePath;
        return;
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = in.readLine().split(",");
        if (header.size() < 2 || header.at(0) != "origin" || header.at(1) != "target") {
            header.clear();
        }
    }

    if (header.isEmpty()) {
        qWarning() << "the header is invalid."
                   << "It should have at least two columns: 'origin' and 'target'."
                   << m_filePath;
        return;
    }

    // create edges
    int row = 0;
    bool isValid = true;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (values.size() != header.size()) {
            qWarning() << "rows must have the same number of columns!"
                       << m_filePath << "Row: " << row;
            isValid = false;
            break;
        }

        bool ok1, ok2;
        int originId = values.at(0).toInt(&ok1);
        int targetId = values.at(1).toInt(&ok2);
        if (!ok1 || !ok2) {
            qWarning() << "invalid values."
                       << "The values for 'origin' and 'target' must be integers."
                       << m_filePath << "Row: " << row;
            isValid = false;
            break;
        } else if (originId < 0 || originId >= m_nodes.size() ||
                   targetId < 0 || targetId >= m_nodes.size()) {
            qWarning() << "invalid values. 'origin' or 'target' are not in the set of nodes."
                          << m_filePath << "Row: " << row;
            isValid = false;
            break;
        }

        m_edges.emplace_back(new Edge(m_nodes.at(originId), m_nodes.at(targetId), type() ==  Directed));
        ++row;
    }
    file.close();

    if (!isValid) {
        Utils::deleteAndShrink(m_edges);
    }
}


} // evoplex
REGISTER_GRAPH(CustomGraph)
#include "plugin.moc"
