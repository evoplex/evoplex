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

#ifndef CONTEXTMENU_TABLE_H
#define CONTEXTMENU_TABLE_H

#include <QAction>
#include <QMenu>
#include <QSpinBox>
#include <QWidgetAction>

#include "core/mainapp.h"
#include "core/simulation.h"

class MenuSpinBox: public QMenu
{
    Q_OBJECT
public:
    explicit MenuSpinBox(const QString& title, QWidget* parent = NULL);
    ~MenuSpinBox();

    void mouseReleaseEvent(QMouseEvent* e);
    void keyPressEvent(QKeyEvent* e);

    int value() { return m_spinBox->value(); }
    void setValue(int v) { m_spinBox->setValue(v); }
    void setMaximum(int v) { m_spinBox->setMaximum(v); }
    void setMinimum(int v) { m_spinBox->setMinimum(v); }

signals:
    void okButton();

private:
    QWidgetAction* m_action;
    QSpinBox* m_spinBox;

};

class ContextMenuTable: public QMenu
{
    Q_OBJECT
public:
    explicit ContextMenuTable(MainApp *mainApp, QWidget* parent = Q_NULLPTR);
    ~ContextMenuTable();

    void openMenu(QPoint globalPoint, int processId, Simulation::Status status);

signals:
    void openView(int experimentId);

public slots:
    void slotPlay();
    void slotPause();
    void slotPauseAt();
    void slotStop();
    void slotStopAt();
    void slotOpenView();

private:
    MainApp* m_mainApp;
    int m_curProcessId;

    // controls
    QAction* m_sectionControls;
    QAction* m_actionPlay;
    QAction* m_actionPause;
    MenuSpinBox* m_menuPauseAt;
    QAction* m_actionStop;
    MenuSpinBox* m_menuStopAt;
    // displays
    QAction* m_actionView;

    // playAt and pauseAt are essentially the same thing
    // so, we just set the string based on the current status
    const QString m_stringPauseAt;
    const QString m_stringPlayUntil;
};

#endif // CONTEXTMENU_TABLE_H
