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

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtDebug>
#include <QStringList>

#include "nodes.h"
#include "attrsgenerator.h"

namespace evoplex {

Nodes Nodes::fromCmd(const QString& cmd, const AttributesScope& attrsScope,
        const GraphType& graphType, QString& error, std::function<void(int)> progress)
{
    if (QFileInfo::exists(cmd)) {
        return Nodes::fromFile(cmd, attrsScope, graphType, error, progress);
    }

    AttrsGenerator* ag = AttrsGenerator::parse(attrsScope, cmd, error);
    if (!ag) {
        return Nodes();
    }

    SetOfAttributes setOfAttrs = ag->create();
    delete ag;

    Node::constructor_key k;
    Nodes nodes;
    int id = 0;
    if (graphType == GraphType::Directed) {
        for (Attributes attrs : setOfAttrs) {
            nodes.insert({id, std::make_shared<DNode>(k, id, attrs)});
            ++id;
        }
    } else if (graphType == GraphType::Undirected) {
        for (Attributes attrs : setOfAttrs) {
            nodes.insert({id, std::make_shared<UNode>(k, id, attrs)});
            ++id;
        }
    }

    return nodes;
}

Nodes Nodes::fromFile(const QString& filePath, const AttributesScope& attrsScope,
        const GraphType& graphType, QString& error, std::function<void(int)> progress)
{
    bool isDirected = graphType == GraphType::Directed;
    Q_ASSERT_X(isDirected || graphType == GraphType::Undirected,
               "Nodes", "graph type must be 'directed' or 'undirected'");

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = "unable to read csv file with the set of nodes.\n" + filePath;
        qWarning() << error;
        return Nodes();
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = validateHeader(in.readLine(), attrsScope, error);
        if (header.isEmpty()) {
            error = "failed to read attributes from file.\n" + filePath;
            qWarning() << error;
            return Nodes();
        }
    }

    // create set of attributes
    int row = 0;
    Nodes nodes;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        NodePtr node = readRow(row, header, values, attrsScope, isDirected, error);
        if (!node) {
            error = QString("%1\n row: %2").arg(error).arg(row);
            qWarning() << error;
            return Nodes();
        }
        nodes.insert({row, node});
        progress(row);
        ++row;
    }
    file.close();

    return nodes;
}

bool Nodes::saveToFile(QString filePath, std::function<void(int)> progress) const
{
    if (empty()) {
        qWarning() << "tried to save an empty set of nodes.";
        return false;
    }

    if (!filePath.endsWith(".csv")) {
        filePath += ".csv";
    }

    QFile file(filePath);
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        qWarning() << "failed to save the set of nodes to file." << filePath;
        return false;
    }

    QTextStream out(&file);
    const std::vector<QString>& header = (*begin()).second->attrs().names();
    for (const QString& col : header) {
        out << col << ",";
    }
    out << "x,y\n";

    std::vector<int> orderedIds;
    for (auto const& pair : (*this)) {
        orderedIds.emplace_back(pair.first);
    }
    std::sort(orderedIds.begin(), orderedIds.end());

    for (const int id : orderedIds) {
        const NodePtr& node = this->at(id);
        for (const Value& value : node->attrs().values()) {
            out << value.toQString() << ",";
        }
        out << node->x() << ",";
        out << node->y() << "\n";

        out.flush();
        progress(node->id());
    }

    file.close();
    return true;
}

QStringList Nodes::validateHeader(const QString& header,
        const AttributesScope& attrsScope, QString& error)
{
    QStringList headerList = header.split(",");
    int duplicates = headerList.removeDuplicates();
    if (duplicates > 0) {
        error = QString("there are '%1' duplicated keys.").arg(duplicates);
        return QStringList();
    } else if (headerList.empty()) {
        error = "the header cannot be empty. Is it comma-separated format?";
        return QStringList();
    }

    const int xIdx = headerList.indexOf("x");
    const int yIdx = headerList.indexOf("y");
    if ((xIdx != -1 && yIdx == -1) || (xIdx == -1 && yIdx != -1)) {
        error = "missing 'x' or 'y' columns. It should have both or none.";
        return QStringList();
    }

    for (const auto* attrRange : attrsScope) {
        if (!header.contains(attrRange->attrName())) {
            const QStringList expectedAttrs = attrsScope.keys();
            error = QString("the header is imcompatible for the model.\n"
                            "Expected attributes: %1").arg(expectedAttrs.join(", "));
            return QStringList();
        }
    }
    return headerList;
}

NodePtr Nodes::readRow(const int row, const QStringList& header, const QStringList& values,
                       const AttributesScope& attrsScope, const bool isDirected, QString& error)
{
    if (values.size() != header.size()) {
        error = "rows must have the same number of columns!";
        return nullptr;
    }

    int coordX = 0;
    int coordY = row;
    Attributes attrs(attrsScope.size());
    for (int col = 0; col < values.size(); ++col) {
        bool isValid = false;
        if (header.at(col) == "x") {
            coordX = values.at(col).toInt(&isValid);
        } else if (header.at(col) == "y") {
            coordY = values.at(col).toInt(&isValid);
        } else {
            AttributeRange* attrRange = attrsScope.value(header.at(col));
            if (attrRange) { // is null if the column is not required
                Value value = attrRange->validate(values.at(col));
                if (value.isValid()) {
                    isValid = true;
                    attrs.replace(attrRange->id(), header.at(col), value);
                }
            }
        }

        if (!isValid) {
            error = QString("invalid value at column: %1").arg(col);
            return nullptr;
        }
    }

    Node::constructor_key k;
    if (isDirected) {
        return std::make_shared<DNode>(k, row, attrs, coordX, coordY);
    }
    return std::make_shared<UNode>(k, row, attrs, coordX, coordY);
}

} // evoplex
