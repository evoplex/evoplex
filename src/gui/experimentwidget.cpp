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
#include "graphwidget.h"

namespace evoplex {

ExperimentWidget::ExperimentWidget(Project* project, int expId, QWidget *parent)
    : QDockWidget(parent)
    , m_project(project)
    , m_innerWindow(new QMainWindow(this))
    //, m_attrWidget(new AttributesWidget(project, this))
{
    this->setWindowTitle(QString("P%1.E%2").arg(m_project->getId()).arg(expId));

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

    QToolBar* tb = new QToolBar("aaa");
    tb->addAction("play");
    tb->addAction("stop");
    tb->addSeparator();
    tb->addAction("graph");
    tb->setMovable(false);
    tb->setFloatable(false);
    tb->setStyleSheet("background: rgb(53,53,53);");

    QVBoxLayout* layout = new QVBoxLayout(new QWidget(this));
    layout->addWidget(m_innerWindow);
    layout->addWidget(tb);
    this->setWidget(layout->parentWidget());

    m_innerWindow->addDockWidget(Qt::TopDockWidgetArea, new GraphWidget(m_project->getExperiment(expId), this));
}

ExperimentWidget::~ExperimentWidget()
{
}

void ExperimentWidget::closeEvent(QCloseEvent* event)
{
    QDockWidget::closeEvent(event);
    this->deleteLater();
}
}
