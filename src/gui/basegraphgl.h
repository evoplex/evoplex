/**
 * This file is part of Evoplex.
 *
 * Evoplex is a multi-agent system for networks.
 * Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef BASEGRAPHGL_H
#define BASEGRAPHGL_H

#include <memory>
#include <vector>

#include <QOpenGLWidget>
#include <QMouseEvent>
#include <QMutex>
#include <QPainter>
#include <QListWidget>
#include <QTimer>

#include "core/experiment.h"

#include "colormap.h"
#include "experimentwidget.h"

#include "maingui.h"

class Ui_BaseGraphGL;

namespace evoplex {

class GraphWidget;

enum class CacheStatus {
    Ready,
    Updating,
    Scheduled
};

enum class SelectionMode {
    Select = 0,     // Select mode
    NodeEdit = 1,   // Node tool
    EdgeEdit = 2    // Edge tool
};

class GraphGLInterface
{
protected:
    virtual ~GraphGLInterface() = default;
    virtual void paintFrame(QPainter& painter) const = 0;
    virtual Node findNode(const QPointF& pos) const = 0;
    virtual Node selectNode(const QPointF& pos, bool center) = 0;
    virtual bool selectNode(const Node& node, bool center) = 0;
    virtual void selectEdge(const Edge& edge) = 0;
    virtual bool deselectNode(const Node& node) = 0;
    virtual bool deselectEdge(const Edge& edge) = 0;
    virtual Node selectedNode() const = 0;
    virtual QPointF selectedNodePos() const = 0;
    virtual void clearSelection() = 0;
    virtual CacheStatus refreshCache() = 0;
    virtual inline bool inSelectedNodes(const Node& node) const = 0;
    virtual inline bool inSelectedEdges(const Edge& edge) const = 0;
    virtual inline QPointF nodePoint(const QPointF& pos) = 0;

};

class BaseGraphGL : public QOpenGLWidget, public GraphGLInterface
{
    Q_OBJECT

public:
    ~BaseGraphGL() override;

    void setup(AbstractGraph* abstractGraph, AttributesScope nodeAttrsScope, AttributesScope edgeAttrsScope);

    void paint(QPaintDevice* device, bool paintBackground) const;

    inline int currStep() const { return m_currStep; }

protected:
    explicit BaseGraphGL(QWidget* parent);

    Ui_BaseGraphGL* m_ui;
    AbstractGraph* m_abstractGraph;
    AttributesScope m_nodeAttrsScope;
    AttributesScope m_edgeAttrsScope;
    bool m_isReadOnly;

    int m_currStep;
    int m_nodeAttr;
    ColorMap* m_nodeCMap;

    QBrush m_background;
    float m_zoomLevel;
    qreal m_nodeScale;
    qreal m_nodeRadius;
    QPointF m_origin;
    
    CacheStatus m_cacheStatus;

    CacheStatus refreshCache() override { return CacheStatus::Ready; }

    void clearSelection() override;

    void updateCache(bool force=false);
    
    void createNode(const QPointF& pos);
    void deleteNode(const QPointF pos);
    void createEdge(const Node& orig, const Node& neigh);
    
    bool deselectNode(const Node& node) override;
    bool deselectEdge(const Edge& edge) override;
    inline SelectionMode curSelectionMode() const;
    inline void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;

signals:
    void nodeSelected(const Node&);
    void edgeSelected(const Edge&);
    void nodeDeselected(const Node&);
    void edgeDeselected(const Edge&);
    void clearedSelected();
    void nodesMoved();
    void updateWidgets(bool) const;

public slots:
    virtual void zoomIn();
    virtual void zoomOut();
    virtual void slotUpdateSelection() = 0;

    void slotFullInspectorVisible(int visible);
    void setCurrentStep(int step);
    void setCurrentSelectionMode(SelectionMode m);
    void setNodeScale(int v);
    void slotRestarted();
    void slotStatusChanged(Status s);
    void slotDeleteSelectedNodes();
    void slotDeleteSelectedEdges();

private slots:
    void slotSelectNode(int nodeid);
    void resetView();
    void setNodeCMap(ColorMap* cmap);
    void edgesListItemClicked(QListWidgetItem* item);
    void removeEdgeEvent();

private:
    QtMaterialIconButton* m_bCenter;
    QtMaterialIconButton* m_bRefresh;

    QTimer m_updateCacheTimer;
    QPoint m_posEntered;
    QMutex m_mutex;
    QRect m_inspGeo; // inspector geometry with margin
    SelectionMode m_curMode;
    std::vector<std::shared_ptr<AttrWidget>> m_edgeAttrWidgets;
    std::vector<std::shared_ptr<AttrWidget>> m_nodeAttrWidgets;
    std::map<int, Node> m_selectedNodes;
    std::map<int, Edge> m_selectedEdges;
    bool m_fullInspectorVisible;
    QSet<int> sneighbors;
    QSet<int> sedges;
    Attributes m_nodeAttrs;
    Attributes m_edgeAttrs;

    void attrValueChanged(int attrId) const;

    void moveSelectedNodes(const Node& node, const QPointF pos);
    void moveNode(Node& node, const QPointF pos);

    void setupInspector();

    void updateInspector(const Node& node);
    void updateEdgeInspector(const Edge& edge);
    void updateNodesInspector(const Node& node);
};

inline SelectionMode BaseGraphGL::curSelectionMode() const
{ return m_curMode; }

inline void BaseGraphGL::paintEvent(QPaintEvent*)
{ paint(this, true); }

} // evoplex
#endif // BASEGRAPHGL_H
