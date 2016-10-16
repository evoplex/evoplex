#ifndef WIDGETPROJECT_H
#define WIDGETPROJECT_H

#include <QWidget>

#include "core/project.h"

namespace Ui {
class WidgetProject;
}

class WidgetProject : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetProject(Project* project, QWidget *parent = 0);
    ~WidgetProject();

private:
    Ui::WidgetProject* m_ui;
    Project* m_project;
};

#endif // WIDGETPROJECT_H
