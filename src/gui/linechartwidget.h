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
    explicit LineChartWidget(Experiment* exp, QWidget* parent = 0);
    ~LineChartWidget();

private slots:
    void setSelectedTrial(int trialId);
    void slotAddSeries();
    void slotEntityChanged(bool isAgent);
    void slotFuncChanged(int idx);
    void updateSeries();

private:
    struct Series {
        QtCharts::QLineSeries* series;
        Output* output;
        int cacheIdx = 0;
        int rowsSkipped = 0;
    };

    Ui_LineChartSettings* m_settingsDlg;
    Experiment* m_exp;
    QtCharts::QChart* m_chart;
    std::vector<Series> m_series;
    float m_maxY;
    bool m_finished;
};
}

#endif // LINECHARTWIDGET_H
