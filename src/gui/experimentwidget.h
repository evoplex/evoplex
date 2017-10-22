/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef EXPERIMENTWIDGET_H
#define EXPERIMENTWIDGET_H

#include <QDockWidget>
#include <QMainWindow>

#include "attributeswidget.h"
#include "core/project.h"

namespace evoplex {

class ExperimentWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit ExperimentWidget(Project* project, int expId, QWidget* parent = 0);
    ~ExperimentWidget();

    inline int expId() const { return m_expId; }
    inline int projId() const { return m_project->getId(); }

signals:
    void closed();

protected:
    void closeEvent(QCloseEvent *event);

private:
    const int m_expId;
    Project* m_project;
    QMainWindow* m_innerWindow;
    AttributesWidget* m_attrWidget;
};
}

#endif // EXPERIMENTWIDGET_H
