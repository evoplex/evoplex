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

#ifndef LINECHART_H
#define LINECHART_H

#include <QDialog>
#include <QDockWidget>
#include <QtCharts/QLineSeries>

#include "core/experiment.h"

#include "outputwidget.h"

class Ui_LineChartSettings;

namespace evoplex {

class LineChart : public QDockWidget
{
    Q_OBJECT

public:
    explicit LineChart(ExperimentPtr exp, QWidget* parent);
    ~LineChart();

public slots:
    void updateSeries();

private slots:
    void slotOutputWidget();
    void slotRestarted();
    void setTrial(quint16 trialId);

private:
    struct Series {
        QtCharts::QLineSeries* series;
        Cache* cache;
    };

    Ui_LineChartSettings* m_settingsDlg;
    ExperimentPtr m_exp;
    QtCharts::QChart* m_chart;
    std::vector<Series> m_series;
    float m_maxY;
    bool m_finished;
    quint16 m_currTrialId;

    const Trial* m_trial;
    int m_currStep;

    void removeAllSeries();
};
}

#endif // LINECHART_H
