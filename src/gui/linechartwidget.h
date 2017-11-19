/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef LINECHARTWIDGET_H
#define LINECHARTWIDGET_H

#include <QDialog>
#include <QDockWidget>
#include <QtCharts/QLineSeries>

#include "core/experiment.h"
#include "ui_linechartsettings.h"

namespace evoplex {

class LineChartWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LineChartWidget(ExperimentsMgr* expMgr, Experiment* exp, QWidget* parent);
    ~LineChartWidget();

private slots:
    void slotRestarted(Experiment* exp);
    void setSelectedTrial(int trialId);
    void slotAddSeries();
    void slotEntityChanged(bool isAgent);
    void slotFuncChanged(int idx);
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
};
}

#endif // LINECHARTWIDGET_H
