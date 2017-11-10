/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include "linechartwidget.h"
#include "titlebar.h"

namespace evoplex {

LineChartWidget::LineChartWidget(Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_exp(exp)
    , m_series(new QtCharts::QSplineSeries())
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

    QtCharts::QChart* chart = new QtCharts::QChart();
    chart->legend()->hide();
    chart->addSeries(m_series);
    chart->createDefaultAxes();

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
