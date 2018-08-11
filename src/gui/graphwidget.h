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

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>
#include <QLineEdit>
#include <QMouseEvent>
#include <QTimer>
#include <vector>

#include "core/experiment.h"

#include "colormap.h"
#include "experimentwidget.h"
#include "graphsettings.h"
#include "maingui.h"

class Ui_GraphWidget;

namespace evoplex {

enum class CacheStatus {
    Ready,
    Updating,
    Scheduled
};

class GraphWidgetInterface
{
protected:
    virtual ~GraphWidgetInterface() = default;
    virtual void paintEvent(QPaintEvent*) = 0;
    virtual Node selectNode(const QPoint& pos) const = 0;
    virtual CacheStatus refreshCache() = 0;
};

class GraphWidget : public QDockWidget, public GraphWidgetInterface
{
    Q_OBJECT

protected:
    explicit GraphWidget(MainGUI* mainGUI, ExperimentPtr exp, ExperimentWidget* parent);
    ~GraphWidget();

    Ui_GraphWidget* m_ui;
    GraphSettings* m_settingsDlg;
    ExperimentPtr m_exp;
    const Trial* m_trial;

    int m_currStep;
    int m_selectedNode;
    int m_nodeAttr;
    ColorMap* m_nodeCMap;

    int m_zoomLevel;
    qreal m_nodeSizeRate;
    qreal m_nodeRadius;
    QPointF m_origin;

    CacheStatus m_cacheStatus;

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void resizeEvent(QResizeEvent* e);

    virtual CacheStatus refreshCache() { return CacheStatus::Ready; }

public slots:
    void updateView(bool forceUpdate);
    void setTrial(quint16 trialId);
    void clearSelection();

private slots:
    void slotRestarted();
    void zoomIn();
    void zoomOut();
    void resetView();
    void setNodeCMap(ColorMap* cmap);

private:
    ExperimentWidget* m_expWidget; // parent
    QTimer m_updateCacheTimer;
    QPointF m_posEntered;
    quint16 m_currTrialId;

    std::vector<QLineEdit*> m_attrs;

    void updateInspector(const Node& node);

    void updateCache(bool force=false);
};
}

#endif // GRAPHWIDGET_H
