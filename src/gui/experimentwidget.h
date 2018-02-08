/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTWIDGET_H
#define EXPERIMENTWIDGET_H

#include <QDockWidget>
#include <QMainWindow>

#include "experimentdesigner.h"
#include "projectspage.h"
#include "core/project.h"

namespace evoplex {

class ExperimentWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ExperimentWidget(MainGUI* mainGUI, Experiment* exp, ProjectsPage* ppage);
    ~ExperimentWidget();

    inline Experiment* experiment() const { return m_exp; }

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void slotStatusChanged(Experiment* exp);

private:
    const QIcon m_kIcon_play;
    const QIcon m_kIcon_pause;
    const QIcon m_kIcon_next;
    const QIcon m_kIcon_reset;
    const QIcon m_kIcon_stop;

    Experiment* m_exp;
    QMainWindow* m_innerWindow;
    QTimer* m_timer;

    QAction* m_aPlayPause;
    QAction* m_aNext;
    QAction* m_aStop;
    QAction* m_aReset;
    QAction* m_aGraph;
    QAction* m_aGrid;
    QAction* m_aLineChart;
    QSlider* m_delay;
};
}

#endif // EXPERIMENTWIDGET_H
