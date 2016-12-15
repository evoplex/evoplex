/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTVIEW_H
#define EXPERIMENTVIEW_H

#include <QMainWindow>

#include "core/mainapp.h"
#include "core/simulation.h"

namespace Ui {
class ExperimentView;
}

class ExperimentView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExperimentView(ProcessesMgr* pMgr, Simulation* sim, QWidget* parent = 0);
    ~ExperimentView();

private:
    Ui::ExperimentView *ui;
    ProcessesMgr* m_processesMgr;
    Simulation* m_simulation;
};

#endif // EXPERIMENTVIEW_H
