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

#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include "basegraphgl.h"

namespace evoplex {

class GridView : public BaseGraphGL
{
    Q_OBJECT
public:
    explicit GridView(QWidget* parent);

public slots:
    void slotUpdateSelection() override;

protected:
    void paintFrame(QPainter& painter) const override;
    Node selectNode(const QPointF& pos, bool center) override;
    Node findNode(const QPointF& pos) override;
    bool selectNode(const Node& node, bool center) override;
    bool deselectNode(const Node& node) override;
    Node selectedNode() const override;
    inline QPointF selectedNodePos() const override;
    void clearSelection() override;
    CacheStatus refreshCache() override;
    inline bool inSelectedNodes(const Node node) const override;
    inline QPointF nodePoint(const QPointF& pos) override;

private:
    struct Cell {
        Node node;
        QRectF rect;
    };
    std::vector<Cell> m_cache;

    std::map<int, Cell> m_selectedCells;
    std::map<int, Node> m_selectedNodes;
    Cell m_selectedCell;

    void drawCell(QPainter& painter, const Cell& cell) const;

    inline QRectF cellRect(const Node& n, double length) const;
};

inline Node GridView::selectedNode() const
{ return m_selectedCell.node; }

inline QPointF GridView::selectedNodePos() const
{ return m_selectedCell.rect.center() + m_origin; }

inline void GridView::clearSelection() { 
    m_selectedCell = Cell(); 
    BaseGraphGL::clearSelection(); 
    m_selectedCells.clear();
}

inline QRectF GridView::cellRect(const Node& n, double length) const 
{ return QRectF(n.x() * length, n.y() * length, length, length); }

inline bool GridView::inSelectedNodes(const Node node) const
{ return m_selectedNodes.count(node.id()) != 0; }

inline QPointF GridView::nodePoint(const QPointF& pos)
{ return QPointF(pos.x() / m_nodeRadius, pos.y() / m_nodeRadius); }

} // evoplex
#endif // GRIDVIEW_H
