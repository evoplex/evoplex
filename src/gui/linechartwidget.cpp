/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

#include "linechartwidget.h"
#include "titlebar.h"

namespace evoplex {

LineChartWidget::LineChartWidget(Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_exp(exp)
{
    setWindowTitle("Line Chart");
    setAttribute(Qt::WA_DeleteOnClose, true);

    TitleBar* titleBar = new TitleBar(exp, this);
    setTitleBarWidget(titleBar);
    connect(titleBar, SIGNAL(trialSelected(int)), this, SLOT(setSelectedTrial(int)));
    setSelectedTrial(0);

    QPalette pal = palette();
    pal.setColor(QPalette::Background, QColor(239,235,231));
    setAutoFillBackground(true);
    setPalette(pal);

    QtCharts::QLineSeries* series = new QtCharts::QLineSeries();
    series->append(0, 6);
    series->append(2, 4);
    series->append(3, 8);
    series->append(7, 4);
    series->append(10, 5);
    *series << QPointF(11, 1) << QPointF(13, 3) << QPointF(17, 6) << QPointF(18, 3) << QPointF(20, 2);

    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->setTitle("Simple line chart example");

    QtCharts::QChartView *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    setWidget(chartView);
}

LineChartWidget::~LineChartWidget()
{
}

void LineChartWidget::setSelectedTrial(int trialId)
{
}


}
