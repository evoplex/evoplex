/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QtDebug>
#include <QStringList>

#include "nodes_p.h"
#include "attrsgenerator.h"
#include "node_p.h"

namespace evoplex {

Nodes NodesPrivate::clone(const Nodes& nodes)
{
    Nodes ret;
    ret.reserve(nodes.size());
    for (auto const& pair : nodes) {
        ret.insert({pair.first, pair.second.clone()});
    }
    return ret;
}

Nodes NodesPrivate::fromCmd(const QString& cmd, const AttributesScope& attrsScope,
        const GraphType& graphType, QString& error, std::function<void(int)> progress)
{
    if (QFileInfo::exists(cmd)) {
        return fromFile(cmd, attrsScope, graphType, error, progress);
    }

    auto ag = AttrsGenerator::parse(attrsScope, cmd, error);
    if (!ag) {
        return Nodes();
    }

    SetOfAttributes setOfAttrs = ag->create();

    BaseNode::constructor_key k;
    Nodes nodes;
    int id = 0;
    if (graphType == GraphType::Directed) {
        for (Attributes attrs : setOfAttrs) {
            Node node;
            node.m_ptr = std::make_shared<DNode>(k, id, attrs);
            nodes.insert({id, node});
            ++id;
        }
    } else if (graphType == GraphType::Undirected) {
        for (Attributes attrs : setOfAttrs) {
            Node node;
            node.m_ptr = std::make_shared<UNode>(k, id, attrs);
            nodes.insert({id, node});
            ++id;
        }
    }

    return nodes;
}

Nodes NodesPrivate::fromFile(const QString& filePath, const AttributesScope& attrsScope,
        const GraphType& graphType, QString& error, std::function<void(int)> progress)
{
    bool isDirected = graphType == GraphType::Directed;
    Q_ASSERT_X(isDirected || graphType == GraphType::Undirected,
               "Nodes", "graph type must be 'directed' or 'undirected'");

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error += "unable to read csv file with the set of nodes.\n" + filePath;
        qWarning() << error;
        return Nodes();
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header;
    if (!in.atEnd()) {
        header = validateHeader(in.readLine(), attrsScope, error);
        if (header.isEmpty()) {
            error += " failed to read attributes from file.\n" + filePath;
            qWarning() << error;
            return Nodes();
        }
    }

    // create set of attributes
    int row = 0;
    Nodes nodes;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        Node node = readRow(row, header, values, attrsScope, isDirected, error);
        if (node.isNull()) {
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

bool NodesPrivate::saveToFile(const Nodes& nodes, QString filePath, std::function<void(int)> progress)
{
    if (nodes.empty()) {
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
    const std::vector<QString>& header = nodes.begin()->second.attrs().names();
    for (const QString& col : header) {
        out << col << ",";
    }
    out << "x,y\n";

    std::vector<int> orderedIds;
    for (auto const& pair : nodes) {
        orderedIds.emplace_back(pair.first);
    }
    std::sort(orderedIds.begin(), orderedIds.end());

    for (const int id : orderedIds) {
        const Node& node = nodes.at(id);
        for (const Value& value : node.attrs().values()) {
            out << value.toQString() << ",";
        }
        out << node.x() << ",";
        out << node.y() << "\n";

        out.flush();
        progress(node.id());
    }

    file.close();
    return true;
}

QStringList NodesPrivate::validateHeader(const QString& header,
        const AttributesScope& attrsScope, QString& error)
{
    QStringList headerList = header.split(",");
    int duplicates = headerList.removeDuplicates();
    if (duplicates > 0) {
        error = QString("there are '%1' duplicated keys.").arg(duplicates);
        return QStringList();
    } else if (headerList.empty()) {
        error = "the header cannot be empty. Is it in comma-separated format?";
        return QStringList();
    }

    const int xIdx = headerList.indexOf("x");
    const int yIdx = headerList.indexOf("y");
    if ((xIdx != -1 && yIdx == -1) || (xIdx == -1 && yIdx != -1)) {
        error = "missing 'x' or 'y' columns. It should have both or none.";
        return QStringList();
    }

    for (const auto& attrRange : attrsScope) {
        if (!header.contains(attrRange->attrName())) {
            const QStringList expectedAttrs = attrsScope.keys();
            error = QString("the header is incompatible with the model.\n"
                            "Expected attributes: %1").arg(expectedAttrs.join(", "));
            return QStringList();
        }
    }
    return headerList;
}

Node NodesPrivate::readRow(const int row, const QStringList& header, const QStringList& values,
        const AttributesScope& attrsScope, const bool isDirected, QString& error)
{
    if (values.size() != header.size()) {
        error += QString("the row %1 should have % columns!").arg(row).arg(header.size());
        return Node();
    }

    AttributeRangePtr attrRange;
    float coordX = 0.f;
    float coordY = row;
    Attributes attrs(attrsScope.size());
    for (int col = 0; col < values.size(); ++col) {
        bool isValid = true;
        if (header.at(col) == "x") {
            coordX = values.at(col).toFloat(&isValid);
        } else if (header.at(col) == "y") {
            coordY = values.at(col).toFloat(&isValid);
        } else {
            attrRange = attrsScope.value(header.at(col), nullptr);
            if (attrRange) { // is null if the column is not required
                Value value = attrRange->validate(values.at(col));
                if (value.isValid()) {
                    attrs.replace(attrRange->id(), header.at(col), value);
                } else {
                    isValid = false;
                }
            }
        }

        if (!isValid) {
            error += QString("invalid value at column %1 ('%2') row %3!\n"
                             "Expected: %4; Actual: %5")
                    .arg(col).arg(header.at(col)).arg(row)
                    .arg(attrRange ? attrRange->attrRangeStr() : "a number")
                    .arg(values.at(col));
            return Node();
        }
    }

    Node node;
    BaseNode::constructor_key k;
    if (isDirected) {
        node.m_ptr = std::make_shared<DNode>(k, row, attrs, coordX, coordY);
    } else {
        node.m_ptr = std::make_shared<UNode>(k, row, attrs, coordX, coordY);
    }
    return node;
}

} // evoplex
