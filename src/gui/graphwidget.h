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

#include "colormap.h"
#include "experimentwidget.h"
#include "graphsettings.h"
#include "maingui.h"
#include "core/experiment.h"

class Ui_GraphWidget;

namespace evoplex {

class GraphWidgetInterface
{
protected:
    virtual void paintEvent(QPaintEvent*) = 0;
    virtual NodePtr selectNode(const QPoint& pos) const = 0;
    virtual int refreshCache() = 0;
};

class GraphWidget : public QDockWidget, public GraphWidgetInterface
{
    Q_OBJECT

public:
    explicit GraphWidget(MainGUI* mainGUI, Experiment* exp, ExperimentWidget* parent);
    ~GraphWidget();

protected:
    Ui_GraphWidget* m_ui;
    GraphSettings* m_settingsDlg;
    Experiment* m_exp;
    AbstractModel* m_model;

    int m_currStep;
    int m_selectedNode;
    int m_nodeAttr;
    ColorMap* m_nodeCMap;

    int m_zoomLevel;
    float m_nodeSizeRate;
    float m_nodeRadius;
    QPoint m_origin;

    enum CacheStatus {
        Ready,
        Updating,
        Scheduled
    };
    CacheStatus m_cacheStatus;

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void resizeEvent(QResizeEvent* e);

    virtual int refreshCache() { return Ready; }

public slots:
    void updateView(bool forceUpdate);
    void setTrial(int trialId);
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
    QPoint m_posEntered;
    int m_currTrialId;

    std::vector<QLineEdit*> m_attrs;

    void updateInspector(const NodePtr& node);

    void updateCache(bool force=false);
};
}

#endif // GRAPHWIDGET_H
