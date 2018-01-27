/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QToolBar>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QDebug>
#include <QLabel>

#include "experimentwidget.h"
#include "linechart.h"
#include "graphwidget.h"
#include "gridwidget.h"

namespace evoplex {

ExperimentWidget::ExperimentWidget(MainGUI* mainGUI, Experiment* exp, ProjectsPage* ppage)
    : QDockWidget(ppage)
    , m_kIcon_play(QIcon(":/icons/play.svg"))
    , m_kIcon_pause(QIcon(":/icons/pause.svg"))
    , m_kIcon_next(QIcon(":/icons/next.svg"))
    , m_kIcon_reset(QIcon(":/icons/reset.svg"))
    , m_kIcon_stop(QIcon(":/icons/stop.svg"))
    , m_exp(exp)
    , m_innerWindow(new QMainWindow(this))
    //, m_attrWidget(new AttributesWidget(project, this))
    , m_timer(new QTimer)
{
    setObjectName(QString("P%1.E%2").arg(m_exp->projId()).arg(m_exp->id()));
    setWindowTitle(objectName());

    // setup the inner qmainwindow
    m_innerWindow->setDockOptions(QMainWindow::AllowTabbedDocks | QMainWindow::GroupedDragging);
    m_innerWindow->setDockNestingEnabled(true);
    m_innerWindow->setAnimated(true);
    m_innerWindow->setStyleSheet("QMainWindow { background-color: rgb(24,24,24); }");
    m_innerWindow->setCentralWidget(0);

    /*
    // setup the AttributesWidget
    m_innerWindow->addDockWidget(Qt::RightDockWidgetArea, m_attrWidget);
    m_attrWidget->setTitleBarWidget(new QWidget());
    m_attrWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    */

    QToolBar* tb = new QToolBar("Controls");
    m_aPlayPause = tb->addAction(m_kIcon_play, "Play/Pause");
    m_aNext = tb->addAction(m_kIcon_next, "Next step");
    m_aStop = tb->addAction(m_kIcon_stop, "Stop");
    m_aReset = tb->addAction(m_kIcon_reset, "Reset");
    tb->addSeparator();
    m_aGraph = tb->addAction("graph");
    m_aGrid = tb->addAction("grid");
    m_aLineChart = tb->addAction("line");
    tb->setMovable(false);
    tb->setFloatable(false);
    tb->setIconSize(QSize(16,16));
    tb->setStyleSheet("background: rgb(53,53,53);");

    ExperimentsMgr* expMgr = ppage->getMainApp()->getExperimentsMgr();
    connect(expMgr, SIGNAL(statusChanged(Experiment*)), SLOT(slotStatusChanged(Experiment*)));
    connect(m_aPlayPause, &QAction::triggered, [this]() { m_exp->toggle(); });
    connect(m_aNext, &QAction::triggered, [this]() { m_exp->playNext(); });
    connect(m_aStop, &QAction::triggered, [this]() { m_exp->stop(); });
    connect(m_aReset, &QAction::triggered, [this]() { m_exp->reset(); });
    slotStatusChanged(exp); // just to init the controls

    QVBoxLayout* layout = new QVBoxLayout(new QWidget(this));
    layout->addWidget(m_innerWindow);
    layout->addWidget(tb);
    setWidget(layout->parentWidget());
    connect(m_aGraph, &QAction::triggered, [this, mainGUI]() {
        GraphWidget* graph = new GraphWidget(mainGUI, m_exp, this);
        m_innerWindow->addDockWidget(Qt::TopDockWidgetArea, graph);
        connect(m_timer, SIGNAL(timeout()), graph, SLOT(update()));
    });
    connect(m_aGrid, &QAction::triggered, [this, mainGUI]() {
        GridWidget* grid = new GridWidget(mainGUI, m_exp, this);
        m_innerWindow->addDockWidget(Qt::TopDockWidgetArea, grid);
        connect(m_timer, SIGNAL(timeout()), grid, SLOT(update()));
    });
    connect(m_aLineChart, &QAction::triggered, [this, expMgr]() {
        LineChart* lineChart = new LineChart(expMgr, m_exp, this);
        m_innerWindow->addDockWidget(Qt::TopDockWidgetArea, lineChart);
        connect(m_timer, SIGNAL(timeout()), lineChart, SLOT(updateSeries()));
    });

    m_timer->start(100);
}

ExperimentWidget::~ExperimentWidget()
{
    delete m_timer;
}

void ExperimentWidget::closeEvent(QCloseEvent* event)
{
    QDockWidget::closeEvent(event);
    emit (closed());
}

void ExperimentWidget::slotStatusChanged(Experiment* exp)
{
    Experiment::Status status = exp->expStatus();
    if (status == Experiment::READY) {
        m_aPlayPause->setIcon(m_kIcon_play);
        m_aPlayPause->setEnabled(true);
        m_aNext->setEnabled(true);
        m_aStop->setEnabled(true);
        m_aReset->setEnabled(true);
    } else if (status == Experiment::RUNNING || status == Experiment::QUEUED) {
        m_aPlayPause->setIcon(m_kIcon_pause);
        m_aPlayPause->setEnabled(true);
        m_aNext->setEnabled(false);
        m_aStop->setEnabled(true);
        m_aReset->setEnabled(false);
    } else if (status == Experiment::FINISHED || status == Experiment::INVALID) {
        m_aPlayPause->setIcon(m_kIcon_play);
        m_aPlayPause->setEnabled(false);
        m_aNext->setEnabled(false);
        m_aStop->setEnabled(false);
        m_aReset->setEnabled(true);
    }
}
}
