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

#ifndef EXPERIMENTWIDGET_H
#define EXPERIMENTWIDGET_H

#include <QDockWidget>
#include <QMainWindow>

#include "external/qt-material-widgets/qtmaterialiconbutton.h"
#include "external/qt-material-widgets/qtmaterialslider.h"

#include "experimentdesigner.h"
#include "projectspage.h"
#include "core/project.h"

namespace evoplex
{
class ExperimentWidget : public PPageDockWidget
{
    Q_OBJECT

public:
    explicit ExperimentWidget(ExperimentPtr exp, MainGUI* mainGUI, ProjectsPage* ppage);
    ~ExperimentWidget();

    virtual void clearSelection() { emit (clearSelections()); }
    virtual ProjectPtr project() const { return m_exp->project(); }
    inline ExperimentPtr exp() const { return m_exp; }

signals:
    void closed();
    void clearSelections();
    void updateWidgets(bool forceUpdate);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void slotStatusChanged(Status status);
    void slotProjectNameChanged(const QString& newName);

private:
    const QIcon m_kIcon_play;
    const QIcon m_kIcon_pause;
    const QIcon m_kIcon_next;
    const QIcon m_kIcon_reset;
    const QIcon m_kIcon_stop;

    ExperimentPtr m_exp;
    QMainWindow* m_innerWindow;
    QTimer* m_timer;

    QtMaterialIconButton* m_aPlayPause;
    QtMaterialIconButton* m_aNext;
    QtMaterialIconButton* m_aStop;
    QtMaterialIconButton* m_aReset;
    QtMaterialIconButton* m_aGraph;
    QtMaterialIconButton* m_aGrid;
    QtMaterialIconButton* m_aLineChart;
    QtMaterialSlider* m_delay;

    bool isAutoDeleteOff();
};
} // evoplex
#endif // EXPERIMENTWIDGET_H
