/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "gui/experimentview.h"
#include "gui/widgetgraph.h"
#include "ui_experimentview.h"

ExperimentView::ExperimentView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ExperimentView)
{
    ui->setupUi(this);

    QDockWidget* dw = new WidgetGraph(parent);
    addDockWidget(Qt::RightDockWidgetArea, dw);
}

ExperimentView::~ExperimentView()
{
    delete ui;
}
