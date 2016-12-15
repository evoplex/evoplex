/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "gui/experimentview.h"
#include "gui/widgetgraph.h"
#include "ui_experimentview.h"

ExperimentView::ExperimentView(ProcessesMgr* pMgr, Simulation* sim, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ExperimentView)
    , m_processesMgr(pMgr)
    , m_simulation(sim)
{
    ui->setupUi(this);

    QDockWidget* dw = new WidgetGraph(this);
    addDockWidget(Qt::RightDockWidgetArea, dw);
}

ExperimentView::~ExperimentView()
{
    delete ui;
}
