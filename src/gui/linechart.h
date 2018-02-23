/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef LINECHART_H
#define LINECHART_H

#include <QDialog>
#include <QDockWidget>
#include <QtCharts/QLineSeries>

#include "outputwidget.h"
#include "core/experiment.h"

class Ui_LineChartSettings;

namespace evoplex {

class LineChart : public QDockWidget
{
    Q_OBJECT

public:
    explicit LineChart(Experiment* exp, QWidget* parent);
    ~LineChart();

public slots:
    void updateSeries();

private slots:
    void slotOutputWidget();
    void slotRestarted();
    void setTrial(int trialId);

private:
    struct Series {
        QtCharts::QLineSeries* series;
        Cache* cache;
    };

    Ui_LineChartSettings* m_settingsDlg;
    Experiment* m_exp;
    QtCharts::QChart* m_chart;
    std::vector<Series> m_series;
    float m_maxY;
    bool m_finished;
    int m_currTrialId;

    AbstractModel* m_model;
    int m_currStep;

    void removeAllSeries();
};
}

#endif // LINECHART_H
