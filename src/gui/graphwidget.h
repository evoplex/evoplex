/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>
#include <QLineEdit>
#include <QMouseEvent>
#include <vector>

#include "colormap.h"
#include "graphsettings.h"
#include "maingui.h"
#include "core/experiment.h"

class Ui_GraphWidget;

namespace evoplex {

class GraphWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(MainGUI* mainGUI, Experiment* exp, QWidget* parent);
    ~GraphWidget();

    virtual void updateCache() = 0;

protected:
    Ui_GraphWidget* m_ui;
    GraphSettings* m_settingsDlg;
    Experiment* m_exp;
    AbstractModel* m_model;

    int m_currStep;
    int m_selectedAgent;
    int m_agentAttr;
    ColorMap* m_agentCMap;

    int m_zoomLevel;
    float m_nodeSizeRate;
    float m_nodeRadius;
    QPoint m_origin;

    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void resizeEvent(QResizeEvent* e);

    virtual void paintEvent(QPaintEvent*) = 0;
    virtual const Agent* selectAgent(const QPoint& pos) const = 0;

public slots:
    void updateView();
    void setTrial(int trialId);

private slots:
    void slotRestarted(Experiment* exp);
    void slotStatusChanged(Experiment* exp);
    void zoomIn();
    void zoomOut();
    void resetView();
    void setAgentCMap(ColorMap* cmap);

private:
    QPoint m_posEntered;
    QTimer m_resizeTimer;
    int m_currTrialId;

    std::vector<QLineEdit*> m_attrs;

    void updateInspector(const Agent* agent);
};
}

#endif // GRAPHWIDGET_H
