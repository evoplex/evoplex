/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>
#include <QLineEdit>
#include <vector>

#include "ui_graphsettings.h"
#include "core/experiment.h"

class Ui_GraphWidget;

namespace evoplex {

class GraphWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(ExperimentsMgr* expMgr, Experiment* exp, QWidget* parent = 0);
    ~GraphWidget();

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent* e);

private slots:
    void updateCache();
    void setGraph(int trialId);
    void zoomIn();
    void zoomOut();
    void resetView();
    void setAgentAttr(int idx);
    void setEdgeAttr(int idx);

private:
    Ui_GraphWidget* m_ui;
    Ui_GraphSettings* m_settingsDlg;
    Experiment* m_exp;
    AbstractGraph* m_graph;
    int m_currTrialId;
    int m_agentAttr;
    int m_edgeAttr;

    bool m_showAgents;
    bool m_showEdges;

    int m_zoomLevel;
    float m_nodeSizeRate;
    float m_edgeSizeRate;
    float m_nodeRadius;

    int m_selectedAgent;
    QPoint m_origin;
    QPoint m_posEntered;
    QTimer m_resizeTimer;

    QHash<int, QLineEdit*> m_attrs;

    struct Cache {
        Agent* agent = nullptr;
        QPointF xy;
        std::vector<QLineF> edges;
    };
    std::vector<Cache> m_cache;
};
}

#endif // GRAPHWIDGET_H
