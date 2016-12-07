/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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
