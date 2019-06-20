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

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include "basegraphgl.h"
#include "graphsettings.h"

namespace evoplex {

class GraphView : public BaseGraphGL
{
    Q_OBJECT

public:
    explicit GraphView(ColorMapMgr* cMgr, ExperimentPtr exp, GraphWidget* parent);

public slots:
    inline void zoomIn() override;
    inline void zoomOut() override;
    inline void openSettings() override;
    void setEdgeScale(int v);
    void setEdgeWidth(int v);

protected:
    void paintFrame(QPainter& painter) const override;
    Node selectNode(const QPointF &pos, bool center) override;
    bool selectNode(const Node& node, bool center) override;
    inline Node selectedNode() const override;
    inline QPointF selectedNodePos() const override;
    inline void clearSelection() override;
    CacheStatus refreshCache() override;
    void setSelectedNode(const Node& node, bool ctrl) override;

private slots:
    void setEdgeCMap(ColorMap* cmap);

private:
    GraphSettings* m_settingsDlg;

    int m_edgeAttr;
    ColorMap* m_edgeCMap;
    qreal m_edgeScale;

    bool m_showNodes;
    bool m_showEdges;

    QPen m_edgePen;
    QPen m_nodePen;
    void updateNodePen();

    struct Star {
        Node node;
        QPointF xy;
        std::vector<std::pair<Edge,QLineF>> edges;
        Star() {}
        Star(Node n, QPointF xy, std::vector<std::pair<Edge,QLineF>> e)
            : node(n), xy(xy), edges(e) {}
    };
    std::vector<Star> m_cache;
    Star m_selectedStar;
    Node m_selectedNodeBase;
    Node m_selectedNodeTar;
    
    Star createStar(const Node& node, const qreal& edgeSizeRate, const QPointF& xy);

    void drawNode(QPainter& painter, const Star& s, double r) const;
    void drawEdges(QPainter& painter) const;
    void drawNodes(QPainter& painter, double nodeRadius) const;
    void drawSelectedEdge(QPainter& painter, double nodeRadius) const;
    void drawSelectedStar(QPainter& painter, double nodeRadius) const;

    inline qreal currEdgeSize() const;
    inline QPointF nodePoint(const Node& node, const qreal& edgeSizeRate) const;
};

inline Node GraphView::selectedNode() const
{ return m_selectedStar.node; }

inline QPointF GraphView::selectedNodePos() const
{ return m_selectedStar.xy + m_origin; }

inline void GraphView::clearSelection()
{ 
    m_selectedStar = Star();
    m_selectedNodeTar = Node();
    BaseGraphGL::clearSelection(); 
}

inline void GraphView::zoomIn()
{ updateNodePen(); BaseGraphGL::zoomIn(); }

inline void GraphView::zoomOut()
{ updateNodePen(); BaseGraphGL::zoomOut(); }

inline void GraphView::openSettings()
{ m_settingsDlg->show(); }

inline qreal GraphView::currEdgeSize() const
{ return m_edgeScale * std::pow(1.25f, m_zoomLevel); }

inline QPointF GraphView::nodePoint(const Node& node, const qreal& edgeSizeRate) const
{
    return QPointF(edgeSizeRate * node.x(), edgeSizeRate * node.y());
}

} // evoplex
#endif // GRAPHVIEW_H
