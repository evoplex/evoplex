/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef LINECHARTWIDGET_H
#define LINECHARTWIDGET_H

#include <QDockWidget>
#include <QtCharts/QSplineSeries>

#include "core/experiment.h"

namespace evoplex {

class LineChartWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit LineChartWidget(Experiment* exp, QWidget* parent = 0);
    ~LineChartWidget();

private slots:
    void setSelectedTrial(int trialId);

private:
    Experiment* m_exp;
    QtCharts::QSplineSeries* m_series;
};
}

#endif // LINECHARTWIDGET_H
