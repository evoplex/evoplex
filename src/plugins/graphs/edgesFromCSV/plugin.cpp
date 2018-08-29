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

#include "plugin.h"

namespace evoplex {

bool EdgesFromCSV::init()
{
    m_filePath = attrs()->value(FilePath).toString();
    if (m_filePath.isEmpty()) {
        qWarning() << "file path cannot be empty.";
        return false;
    }
    return true;
}

bool EdgesFromCSV::reset()
{
    removeAllEdges();

    QFile file(m_filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "unable to read csv file with the set of nodes." << m_filePath;
        return false;
    }

    QTextStream in(&file);

    // read and validate header
    QStringList header = in.readLine().split(",");
    if (!validateHeader(header)) {
        return false;
    }

    // create edges
    int row = 0;
    while (!in.atEnd()) {
        QStringList values = in.readLine().split(",");
        if (!readRow(++row, header, values)) {
            file.close();
            removeAllEdges();
            return false;
        }
    }
    file.close();

    return true;
}

bool EdgesFromCSV::validateHeader(const QStringList& header) const
{
    if (header.isEmpty()) {
        qWarning() << "the header is invalid."
                   << "It should have at least two columns: 'origin' and 'target'."
                   << m_filePath;
        return false;
    }

    if (header.at(0) != "origin" || header.at(1) != "target") {
        qWarning() << "the header is invalid."
                   << "The first and second columns must be named as 'origin' and 'target' respectively."
                   << m_filePath;
        return false;
    }

    int duplicates = header.size() - header.toSet().size();
    if (duplicates > 0) {
        qWarning() << "the header is invalid."
                   << QString("There are '%1' duplicated keys.").arg(duplicates);
        return false;
    }

    if (m_edgeAttrsGen) {
        auto const& ascope = m_edgeAttrsGen->attrsScope();
        for (auto const& attrRange : ascope) {
            if (header.contains(attrRange->attrName())) {
                continue;
            }

            const QStringList expectedAttrs = ascope.keys();
            qWarning() << QString("the header is incompatible with the model.\n"
                        "Expected attributes: %1").arg(expectedAttrs.join(", "));
            return false;
        }
    }

    return true;
}

bool EdgesFromCSV::readRow(int row, const QStringList& header, const QStringList& values)
{
    if (values.size() != header.size()) {
        qWarning() << "rows must have the same number of columns!"
                   << m_filePath << "Row: " << row;
        return false;
    }

    bool ok1, ok2;
    int originId = values.at(0).toInt(&ok1);
    int targetId = values.at(1).toInt(&ok2);
    if (!ok1 || !ok2) {
        qWarning() << "'origin' and 'target' must be integers."
                   << m_filePath << "Row: " << row;
        return false;
    }

    Attributes* attrs = new Attributes();
    if (m_edgeAttrsGen) {
        auto const& ascope = m_edgeAttrsGen->attrsScope();
        attrs->resize(ascope.size());
        for (int col = 2; col < values.size(); ++col) {
            auto const& attrRange = ascope.value(header.at(col), nullptr);
            if (!attrRange) { // is null if the column is not required
                continue;
            }

            Value value = attrRange->validate(values.at(col));
            if (value.isValid()) {
                attrs->replace(attrRange->id(), header.at(col), value);
            } else {
                qWarning() << QString("invalid value at column %1 ('%2') row %3!\n"
                                      "Expected: %4; Actual: %5")
                              .arg(col).arg(header.at(col)).arg(row)
                              .arg(attrRange ? attrRange->attrRangeStr() : "a number")
                              .arg(values.at(col));
                delete attrs;
                return false;
            }
        }
    }

    try {
        addEdge(originId, targetId, attrs);
    } catch (std::out_of_range) {
        qWarning() << QString("'origin'(%1) or 'target'(%2) are not"
                      " in the set of nodes. Check the row %3 (%4)")
                      .arg(originId).arg(targetId).arg(row).arg(m_filePath);
        return false;
    }

    return true;
}

} // evoplex
REGISTER_PLUGIN(EdgesFromCSV)
#include "plugin.moc"
