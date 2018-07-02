/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QMessageBox>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>

#include "linechart.h"
#include "titlebar.h"
#include "ui_linechartsettings.h"

namespace evoplex {

LineChart::LineChart(Experiment* exp, QWidget* parent)
    : QDockWidget(parent)
    , m_settingsDlg(new Ui_LineChartSettings)
    , m_exp(exp)
    , m_chart(new QtCharts::QChart())
    , m_maxY(0)
    , m_finished(false)
    , m_currTrialId(0)
    , m_model(nullptr)
    , m_currStep(0)
{
    setWindowTitle("Line Chart");
    setAttribute(Qt::WA_DeleteOnClose, true);

    Q_ASSERT_X(!m_exp->autoDeleteTrials(), "LineChart",
               "tried to build a LineChart for a experiment that will be auto-deleted!");

    connect(m_exp, SIGNAL(restarted()), SLOT(slotRestarted()));

    QDialog* dlg = new QDialog(this);
    m_settingsDlg->setupUi(dlg);
    connect(m_settingsDlg->bEditSeries, SIGNAL(clicked(bool)), SLOT(slotOutputWidget()));

    TitleBar* titleBar = new TitleBar(exp, this);
    setTitleBarWidget(titleBar);
    setTrial(0); // init
    connect(titleBar, SIGNAL(trialSelected(int)), SLOT(setTrial(int)));
    connect(titleBar, SIGNAL(openSettingsDlg()), dlg, SLOT(show()));

    connect(exp, &Experiment::trialCreated, [this](int trialId) {
        if (trialId == m_currTrialId) {
            m_model = m_exp->trial(trialId);
            m_currStep = 0;
        }
    });

    m_chart->legend()->hide();
    m_chart->setAnimationOptions(QtCharts::QChart::NoAnimation);
    m_chart->setTheme(QtCharts::QChart::ChartThemeLight);
    m_chart->setBackgroundVisible(false);

    QtCharts::QChartView *chartView = new QtCharts::QChartView(m_chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setBackgroundBrush(QColor(239,235,231));
    setWidget(chartView);
}

LineChart::~LineChart()
{
    removeAllSeries();
    delete m_settingsDlg;
    delete m_chart;
}

void LineChart::slotOutputWidget()
{
    if (m_exp->expStatus() == Experiment::RUNNING
            || m_exp->expStatus() == Experiment::QUEUED) {
        QMessageBox::warning(this, "Line Chart",
                    "You cannot edit the series of a running experiment.\n"
                    "Please, pause it and try again.");
        return;
    }

    std::vector<Cache*> caches;
    if (!m_series.empty()) {
        caches.reserve(m_series.size());
        for (Series& s : m_series) {
            caches.emplace_back(s.cache);
        }
    }

    OutputWidget* ow = new OutputWidget(m_exp->modelPlugin(), {m_currTrialId}, this, caches);
    if (ow->exec() == QDialog::Rejected) {
        return; // nothing to do
    }

    removeAllSeries();

    for (auto& it : ow->caches()) {
        Cache* cache = it.second;
        Series s;
        s.series = new QtCharts::QLineSeries();
        //s.series->setUseOpenGL(true); TODO: make sure we can call it
        OutputPtr existingOutput = m_exp->searchOutput(cache->output());
        if (existingOutput && existingOutput != cache->output()) {
            s.cache = existingOutput->addCache(cache->inputs(), {m_currTrialId});
            cache->deleteCache();
        } else {
            s.cache = cache;
            m_exp->addOutput(cache->output());
        }
        m_series.emplace_back(s);
        m_chart->addSeries(s.series);
    }

    ow->deleteLater();
}

void LineChart::slotRestarted()
{
    if (m_exp->autoDeleteTrials()) {
        close();
        return;
    }

    for (Series& s : m_series) {
        s.series->clear(); // remove all points
        m_exp->addOutput(s.cache->output()); // make sure we reinsert everything
    }
    m_finished = false;
}

void LineChart::setTrial(int trialId)
{
    if (m_exp->expStatus() == Experiment::RUNNING
               || m_exp->expStatus() == Experiment::QUEUED) {
        QMessageBox::warning(this, "Line Chart",
                "Tried to change the trial in a running experiment.\n"
                "Please, pause it and try again.");
        return;
    }

    if (!m_series.empty()) {
        int res = QMessageBox::warning(this, "Line Chart",
                    "Are you sure you want to change the trial?\n"
                    "It'll delete the current chart.\n"
                    "If you don't want to delete it now, you could just open another line chart.",
                    QMessageBox::Yes, QMessageBox::Cancel);
        if (res == QMessageBox::Cancel) {
            return;
        }
    }

    m_currTrialId = trialId;
    m_model = m_exp->trial(trialId);

    for (Series& s : m_series) {
        s.series->clear(); // remove all points
        Values inputs = s.cache->inputs(); // keep the same inputs
        OutputPtr parent = s.cache->output(); // keep the same parent
        s.cache->deleteCache();
        s.cache = parent->addCache(inputs, {trialId});
    }
}

void LineChart::removeAllSeries()
{
    for (Series& s : m_series) {
        OutputPtr parent = s.cache->output();
        s.cache->deleteCache();
        if (parent->isEmpty()) {
            m_exp->removeOutput(parent);
        }
    }
    m_chart->removeAllSeries();
    m_series.clear();
}

void LineChart::updateSeries()
{
    if (!m_model || !m_chart->isVisible() || m_series.empty() || m_model->currStep() == m_currStep) {
        return;
    }

    float minX = EVOPLEX_MAX_STEPS;
    float maxY = m_maxY;
    for (Series& s : m_series) {
        if (s.cache->isEmpty(m_currTrialId)) {
            continue;
        }

        QVector<QPointF> points = s.series->pointsVector();
        float x = 0.f;
        float y = 0.f;

        // read only the top 10k (max) lines to avoid blocking the UI
        int i = 0;
        bool lastWasDuplicated = false;
        do {
            const Cache::Row& row = s.cache->readFrontRow(m_currTrialId);
            Q_ASSERT_X(row.second.size() == 1, "LineChart", "it must have only one column");

            x = row.first;
            if (row.second.at(0).type() == Value::INT) {
                y = row.second.at(0).toInt();
            } else if (row.second.at(0).type() == Value::DOUBLE) {
                y = row.second.at(0).toDouble();
            } else {
                qFatal("the type is invalid!");
            }
            s.cache->flushFrontRow(m_currTrialId);

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
            if (x < minX) minX = x;
            if (y > maxY) maxY = y;
            ++i;
        } while (i < 10000 && !s.cache->isEmpty(m_currTrialId));

        if (lastWasDuplicated) {
            points.push_back(QPointF(x, y));
        }

        s.series->replace(points);
    }

    if (minX < EVOPLEX_MAX_STEPS) {
        m_currStep = minX;
    }

    if (maxY != m_maxY) {
        m_maxY = maxY;
        m_chart->createDefaultAxes();
        m_chart->axisX()->setRange(0, m_exp->stopAt());
        m_chart->axisX()->setGridLineVisible(false);
        m_chart->axisY()->setRange(0, m_maxY + 1);
        m_chart->axisY()->setGridLineVisible(false);
    }

    m_finished = m_series.front().cache->isEmpty(m_currTrialId)
            && m_exp->expStatus() == Experiment::FINISHED;
}

} // evoplex
