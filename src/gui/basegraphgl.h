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

#ifndef BASEGRAPHGL_H
#define BASEGRAPHGL_H

#include <map>
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
#include "fullinspector.h"

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
    virtual Node selectNode(const QPointF& pos, bool center) = 0;
    virtual bool selectNode(const Node& node, bool center) = 0;
    virtual Node selectedNode() const = 0;
    virtual QPointF selectedNodePos() const = 0;
    virtual void clearSelection() = 0;
    virtual CacheStatus refreshCache() = 0;
};

class BaseGraphGL : public QOpenGLWidget, public GraphGLInterface
{
    Q_OBJECT

public:
    ~BaseGraphGL() override;

    void setup(AbstractGraph* abstractGraph, AttributesScope nodeAttrsScope);

    void paint(QPaintDevice* device, bool paintBackground) const;

    inline int currStep() const { return m_currStep; }
    inline void setInspector(FullInspector* inspector);
    std::map<int, Node> m_selectedNodes; //TODO: This shouldn't be public
//    void updateFullInspector();

protected:
    explicit BaseGraphGL(QWidget* parent);

    Ui_BaseGraphGL* m_ui;
    AbstractGraph* m_abstractGraph;
    AttributesScope m_nodeAttrsScope;
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

    inline void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent* e) override;
    void mouseReleaseEvent(QMouseEvent* e) override;
    void resizeEvent(QResizeEvent* e) override;
    void wheelEvent(QWheelEvent* e) override;
    void keyPressEvent(QKeyEvent* e) override;
    void keyReleaseEvent(QKeyEvent* e) override;

signals:
    void nodeSelected(const Node&);
    void clearedSelected();
    void updateWidgets(bool) const;

public slots:
    virtual void zoomIn();
    virtual void zoomOut();

    void setCurrentStep(int step);
    void setCurrentSelectionMode(SelectionMode m);
    void setNodeScale(int v);
    void slotRestarted();
    void slotStatusChanged(Status s);

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
    std::vector<std::shared_ptr<AttrWidget>> m_attrWidgets;
    FullInspector* m_inspector;
    bool m_fullInspectorVisible;

    void attrValueChanged(int attrId) const;

    void setupInspector();

    void updateInspector(const Node& node);
    void updateEdgeInspector(const Edge& edge);
    void updateEdgesInspector(const Node& pnode, const Node& cnode);
};

inline void BaseGraphGL::setInspector(FullInspector* inspector) {
    m_fullInspectorVisible = true;
    m_inspector = inspector;
}

inline void BaseGraphGL::paintEvent(QPaintEvent*)
{ paint(this, true); }

} // evoplex
#endif // BASEGRAPHGL_H
