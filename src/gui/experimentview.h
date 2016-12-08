/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTVIEW_H
#define EXPERIMENTVIEW_H

#include <QMainWindow>

namespace Ui {
class ExperimentView;
}

class ExperimentView : public QMainWindow
{
    Q_OBJECT

public:
    explicit ExperimentView(QWidget *parent = 0);
    ~ExperimentView();

private:
    Ui::ExperimentView *ui;
};

#endif // EXPERIMENTVIEW_H
