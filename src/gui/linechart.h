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
#include "ui_linechartsettings.h"
#include "core/experiment.h"

namespace evoplex {

class LineChart : public QDockWidget
{
    Q_OBJECT

public:
    explicit LineChart(ExperimentsMgr* expMgr, Experiment* exp, QWidget* parent);
    ~LineChart();

private slots:
    void slotRestarted();
    void setSelectedTrial(int trialId);
    void slotAddSeries(std::vector<Output*> newOutputs);
    void updateSeries();
    void removeSeries(int seriesId);

private:
    struct Series {
        QtCharts::QLineSeries* series;
        Output* output;
        int cacheIdx = 0;
    };

    Ui_LineChartSettings* m_settingsDlg;
    Experiment* m_exp;
    QtCharts::QChart* m_chart;
    std::unordered_map<int, Series> m_series;
    float m_maxY;
    bool m_finished;
    int m_currentTrialId;
    int m_lastSeriesId;
};
}

#endif // LINECHART_H
