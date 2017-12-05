/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QDockWidget>
#include <QLineEdit>
#include <vector>

#include "colormap.h"
#include "ui_graphsettings.h"
#include "core/experiment.h"

class Ui_GraphWidget;

namespace evoplex {

class GridWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit GridWidget(ExperimentsMgr* expMgr, Experiment* exp, QWidget* parent = 0);
    ~GridWidget();

protected:
    void mousePressEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);
    void paintEvent(QPaintEvent*);
    void resizeEvent(QResizeEvent* e);

private slots:
    void updateCache();
    void setTrial(int trialId);
    void zoomIn();
    void zoomOut();
    void resetView();
    void setAgentAttr(int idx);

private:
    Ui_GraphWidget* m_ui;
    Ui_GraphSettings* m_settingsDlg;
    Experiment* m_exp;
    AbstractModel* m_model;
    int m_currTrialId;
    int m_agentAttr;
    ColorMap m_agentCMap;

    int m_zoomLevel;
    float m_pixelSizeRate;
    float m_pixelSize;

    int m_selectedAgent;
    QPoint m_origin;
    QPoint m_posEntered;
    QTimer m_resizeTimer;

    QHash<int, QLineEdit*> m_attrs;

    struct Cache {
        Agent* agent = nullptr;
        QRectF rect;
    };
    std::vector<Cache> m_cache;

    void updateInspector(const Agent* agent);
};
}

#endif // GRIDWIDGET_H
