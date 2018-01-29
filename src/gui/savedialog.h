#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>

#include "core/project.h"

class Ui_SaveDialog;

namespace evoplex {

class SaveDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SaveDialog(QWidget* parent);
    ~SaveDialog();

    bool save(Project* project);
    bool saveAs(Project* project);

private slots:
    void browseDir();

private:
    Ui_SaveDialog* m_ui;
    Project* m_currProject;
};
}
#endif // SAVEDIALOG_H
