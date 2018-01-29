/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QMessageBox>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include "linechart.h"
#include "titlebar.h"

namespace evoplex {

LineChart::LineChart(ExperimentsMgr* expMgr, Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_settingsDlg(new Ui_LineChartSettings)
    , m_exp(exp)
    , m_chart(new QtCharts::QChart())
    , m_maxY(0)
    , m_finished(false)
    , m_currentTrialId(0)
    , m_lastSeriesId(-1)
{
    setWindowTitle("Line Chart");
    setAttribute(Qt::WA_DeleteOnClose, true);

    TitleBar* titleBar = new TitleBar(exp, this);
    setTitleBarWidget(titleBar);
    connect(titleBar, SIGNAL(trialSelected(int)), this, SLOT(setSelectedTrial(int)));
    setSelectedTrial(0);

    QDialog* dlg = new QDialog(this);
    m_settingsDlg->setupUi(dlg);
    OutputWidget* outputWidget = new OutputWidget(m_exp->modelPlugin(), this);
    m_settingsDlg->seriesLayout->addWidget(outputWidget);
    connect(outputWidget, SIGNAL(closed(std::vector<Output*>)), SLOT(slotAddSeries(std::vector<Output*>)));
    connect(titleBar, &TitleBar::openSettingsDlg,
            [this, outputWidget, dlg]() { outputWidget->setTrialIds({m_currentTrialId}); dlg->show(); });
    dlg->hide();

    m_chart->legend()->hide();
    m_chart->setAnimationOptions(QtCharts::QChart::GridAxisAnimations);
    m_chart->setTheme(QtCharts::QChart::ChartThemeLight);
    m_chart->setBackgroundVisible(false);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setBackgroundBrush(QColor(239,235,231));
    setWidget(chartView);

    connect(expMgr, SIGNAL(restarted(Experiment*)), SLOT(slotRestarted(Experiment*)));
}

LineChart::~LineChart()
{
    delete m_settingsDlg;
    delete m_chart;
}

void LineChart::slotRestarted(Experiment* exp)
{
    if (exp != m_exp) {
        return;
    }

    for (auto& i : m_series) {
        Series& s = i.second;
        s.series->clear(); // remove all points
    }
    m_finished = false;
}

void LineChart::setSelectedTrial(int trialId)
{
    if (m_series.empty()) {
        m_currentTrialId = trialId;
        return;
    }

    int res = QMessageBox::warning(this, "Line Chart",
                         "Are you sure you want to change the trial?\n"
                         "It'll delete the current chart.",
                         QMessageBox::Yes, QMessageBox::Cancel);
    if (res == QMessageBox::Cancel) {
        return;
    }

    for (auto& i : m_series) {
        Series& s = i.second;
        s.series->clear(); // remove all points
        Values inputs = s.output->inputs(s.cacheIdx, m_currentTrialId); // keep the same inputs
        s.output->removeCache(s.cacheIdx, m_currentTrialId);
        s.cacheIdx = s.output->addCache(inputs, {trialId});
    }

    m_currentTrialId = trialId;
}

void LineChart::removeSeries(int seriesId)
{
    Series& s = m_series.at(seriesId);
    m_chart->removeSeries(s.series);
    s.output->removeCache(s.cacheIdx, m_currentTrialId);
    if (s.output->isEmpty()) {
        m_exp->removeOutput(s.output);
    }
    m_series.erase(seriesId);
}

void LineChart::slotAddSeries(std::vector<Output*> newOutputs)
{
    for (Output* output : newOutputs) {
        Series s;
        s.series = new QtCharts::QLineSeries();
        //s.series->setUseOpenGL(true); TODO: make sure we can call it
        Output* existingOutput = m_exp->searchOutput(output);
        if (existingOutput) {
            s.cacheIdx = existingOutput->addCache({output->allInputs()}, {m_currentTrialId});
            s.output = existingOutput;
            if (existingOutput != output) {
                delete output;
                output = nullptr;
            }
        } else {
            s.cacheIdx = 0;
            s.output = output;
            m_exp->addOutput(output);
        }
        m_series.insert({++m_lastSeriesId, s});
        m_chart->addSeries(s.series);
    }
}

void LineChart::updateSeries()
{
    if (m_series.empty() || m_finished || !this->isVisible()) {
        return;
    }

    float maxY = m_maxY;
    for (auto& it : m_series) {
        Series& s = it.second;
        if (s.output->isEmpty(s.cacheIdx, m_currentTrialId)) {
            continue;
        }

        QVector<QPointF> points = s.series->pointsVector();
        float x = 0.f;
        float y = 0.f;

        // read only the top 10k (max) lines to avoid blocking the UI
        int i = 0;
        bool lastWasDuplicated = false;
        do {
            const Output::Row& row = s.output->readFrontRow(s.cacheIdx, m_currentTrialId);
            Q_ASSERT(row.second.size() == 1);

            x = row.first;
            if (row.second.at(0).type() == Value::INT) {
                y = row.second.at(0).toInt();
            } else if (row.second.at(0).type() == Value::DOUBLE) {
                y = row.second.at(0).toDouble();
            } else {
                qFatal("[LineChart] : the type is invalid!");
            }
            s.output->flushFrontRow(s.cacheIdx, m_currentTrialId);

            // we skip the duplicated rows to reduce the amount of unnecessary points
            if (!points.isEmpty()) {
                bool currIsDuplicated = qFuzzyCompare(y, (float) points.last().y());
                if (lastWasDuplicated && currIsDuplicated) {
                    points.last().setX(x);
                    lastWasDuplicated = currIsDuplicated;
                    continue;
                }
                lastWasDuplicated = currIsDuplicated;
            }

            points.push_back(QPointF(x, y));
            if (y > maxY) {
                maxY = y;
            }
            ++i;
        } while (i < 10000 && !s.output->isEmpty(s.cacheIdx, m_currentTrialId));

        if (lastWasDuplicated) {
            points.push_back(QPointF(x, y));
        }

        s.series->replace(points);
    }

    if (maxY != m_maxY) {
        m_maxY = maxY;
        m_chart->createDefaultAxes();
        m_chart->axisX()->setRange(0, m_exp->stopAt());
        m_chart->axisX()->setGridLineVisible(false);
        m_chart->axisY()->setRange(0, m_maxY + 1);
        m_chart->axisY()->setGridLineVisible(false);
    }

    const Series& s = m_series.cbegin()->second;
    m_finished = s.output->isEmpty(s.cacheIdx, m_currentTrialId)
                    && m_exp->expStatus() == Experiment::FINISHED;
}

}
