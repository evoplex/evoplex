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

#ifndef EDGES_FROM_FILE_H
#define EDGES_FROM_FILE_H

#include <QPair>
#include <vector>

#include <plugininterface.h>

namespace evoplex {
class EdgesFromCSV: public AbstractGraph
{
public:
    bool init() override;
    bool reset() override;

private:
    // graph parameters
    enum GraphAttr { FilePath };
    QString m_filePath;

    bool validateHeader(const QStringList &header) const;
    bool readRow(int row, const QStringList& header, const QStringList& values);
};
}

#endif // EDGES_FROM_FILE_H
