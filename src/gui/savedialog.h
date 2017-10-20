#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QWidget>

#include "core/project.h"

class Ui_SaveDialog;

namespace evoplex {

class SaveDialog : public QWidget
{
    Q_OBJECT

public:
    explicit SaveDialog(QWidget* parent = 0);
    ~SaveDialog();

    void save(Project* project);
    void saveAs(Project* project);

private slots:
    void browseDir();

private:
    Ui_SaveDialog* m_ui;
    Project* m_currProject;

};
}
#endif // SAVEDIALOG_H
