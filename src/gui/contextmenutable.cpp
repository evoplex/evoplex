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

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QtDebug>

#include "gui/contextmenutable.h"
#include "core/mainapp.h"

MenuSpinBox::MenuSpinBox(const QString& title, QWidget* parent)
    : QMenu(title, parent)
{
    m_action = new QWidgetAction(this);

    QWidget* w = new QWidget(this);
    QHBoxLayout* l = new QHBoxLayout(w);
    l->insertWidget(0, new QLabel("Step"));

    m_spinBox = new QSpinBox(w);
    m_spinBox->setMaximum(MAX_STEP);
    m_spinBox->setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
    l->insertWidget(1, m_spinBox);

    QPushButton* okButton = new QPushButton("ok", w);
    okButton->setMaximumWidth(20);
    l->insertWidget(2, okButton);

    w->setLayout(l);
    m_action->setDefaultWidget(w);
    addAction(m_action);

    connect(okButton, SIGNAL(pressed()), this, SIGNAL(okButton()));
}

MenuSpinBox::~MenuSpinBox()
{
    delete m_action;
}

void MenuSpinBox::mouseReleaseEvent(QMouseEvent* e)
{
    QAction* action = activeAction();
    if (action && action->isEnabled()) {
        action->setEnabled(false);
        QMenu::mouseReleaseEvent(e);
        action->setEnabled(true);
    } else {
        QMenu::mouseReleaseEvent(e);
    }
}

void MenuSpinBox::keyPressEvent(QKeyEvent* e)
{
    QAction* action = activeAction();
    if (action && action->isEnabled()) {
        action->setEnabled(false);
        QMenu::keyPressEvent(e);
        action->setEnabled(true);
    } else {
        QMenu::keyPressEvent(e);
    }
}

ContextMenuTable::ContextMenuTable(MainApp* mainApp, QWidget* parent)
    : QMenu(parent)
    , m_mainApp(mainApp)
    , m_stringPauseAt("Pause at")
    , m_stringPlayUntil("Play until")
{
    //
    // create all actions
    //
    m_actionPlay = new QAction("Play", this);
    m_actionPause = new QAction("Pause", this);
    m_actionStop = new QAction("Stop", this);
    m_actionView = new QAction("View", this);
    m_menuPauseAt = new MenuSpinBox(m_stringPauseAt);
    m_menuStopAt = new MenuSpinBox("Stop at");

    //
    // add actions
    //
    m_sectionControls = addSection("Controls");
    addAction(m_actionPlay);
    addAction(m_actionPause);
    addMenu(m_menuPauseAt);
    addAction(m_actionStop);
    addMenu(m_menuStopAt);
    addSection("Displays");
    addAction(m_actionView);

    //
    // connect signals
    //
    connect(m_actionPlay, SIGNAL(triggered(bool)), this, SLOT(slotPlay()));
    connect(m_actionPause, SIGNAL(triggered(bool)), this, SLOT(slotPause()));
    connect(m_actionStop, SIGNAL(triggered(bool)), this, SLOT(slotStop()));
    connect(m_menuPauseAt, SIGNAL(okButton()), this, SLOT(slotPauseAt()));
    connect(m_menuStopAt, SIGNAL(okButton()), this, SLOT(slotStopAt()));
    connect(m_actionView, SIGNAL(triggered(bool)), this, SLOT(slotOpenView()));
}

ContextMenuTable::~ContextMenuTable()
{
    delete m_actionPlay;
    delete m_actionPause;
    delete m_menuPauseAt;
    delete m_actionStop;
    delete m_menuStopAt;
    delete m_actionView;
}

void ContextMenuTable::openMenu(QPoint globalPoint, int processId, Simulation::Status status)
{
    m_curProcessId = -1;
    if (status == Simulation::RUNNING) {
        m_sectionControls->setVisible(true);
        m_actionPlay->setVisible(false);
        m_actionPause->setVisible(true);
        m_menuPauseAt->setTitle(m_stringPauseAt);
        m_menuPauseAt->menuAction()->setVisible(true);
        m_actionStop->setVisible(true);
        m_menuStopAt->menuAction()->setVisible(true);
    } else if (status == Simulation::READY) {
        m_sectionControls->setVisible(true);
        m_actionPlay->setVisible(true);
        m_actionPause->setVisible(false);
        m_menuPauseAt->setTitle(m_stringPlayUntil);
        m_menuPauseAt->menuAction()->setVisible(true);
        m_actionStop->setVisible(true);
        m_menuStopAt->menuAction()->setVisible(true);
    } else if (status == Simulation::FINISHING
                   || status == Simulation::FINISHED) {
        m_sectionControls->setVisible(false);
        m_actionPlay->setVisible(false);
        m_actionPause->setVisible(false);
        m_menuPauseAt->menuAction()->setVisible(false);
        m_actionStop->setVisible(false);
        m_menuStopAt->menuAction()->setVisible(false);
    } else {
        return;
    }

    Simulation* s = m_mainApp->getProcessesMgr()->getProcess(processId);
    if (s) {
        m_curProcessId = processId;

        m_menuPauseAt->setValue(s->getPauseAt());
        m_menuPauseAt->setMinimum(s->getCurrentStep() + 1);
        m_menuPauseAt->setMaximum(s->getStopAt());

        m_menuStopAt->setValue(s->getStopAt());
        m_menuStopAt->setMinimum(s->getCurrentStep() + 1);

        exec(globalPoint);
    }
}

void ContextMenuTable::slotPlay()
{
    m_mainApp->getProcessesMgr()->play(m_curProcessId);
}

void ContextMenuTable::slotPause()
{
    m_mainApp->getProcessesMgr()->pause(m_curProcessId);
}

void ContextMenuTable::slotPauseAt()
{
    int step = m_menuPauseAt->value();
    m_mainApp->getProcessesMgr()->pauseAt(m_curProcessId, step);
    close();
}

void ContextMenuTable::slotStop()
{
    m_mainApp->getProcessesMgr()->stop(m_curProcessId);
}

void ContextMenuTable::slotStopAt()
{
    int step = m_menuPauseAt->value();
    m_mainApp->getProcessesMgr()->stopAt(m_curProcessId, step);
    close();
}

void ContextMenuTable::slotOpenView()
{
    emit (openView(m_curProcessId));
}
