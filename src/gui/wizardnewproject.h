/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef WIZARDNEWPROJECT_H
#define WIZARDNEWPROJECT_H

#include <QWizard>

#include "core/mainapp.h"

namespace Ui {
    class WizardNewProject;
}

class WizardNewProject : public QWizard
{
    Q_OBJECT

public:
    explicit WizardNewProject(MainApp* mainApp, QWidget* parent = 0);
    ~WizardNewProject();

    virtual bool validateCurrentPage();
    virtual void done(int result);

signals:
    void newProject(Project*);

private slots:
    void slotBrowseModel();
    void slotBrowseProjectDir();
    void slotModelSelected();

private:
    Ui::WizardNewProject* m_ui;
    MainApp* m_mainApp;

    void updateModels();
};

#endif // WIZARDNEWPROJECT_H
