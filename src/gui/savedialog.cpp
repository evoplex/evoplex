/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDir>
#include <QFileDialog>
#include <QProgressDialog>

#include "savedialog.h"
#include "ui_savedialog.h"

namespace evoplex {

SaveDialog::SaveDialog(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::SaveDialog)
    , m_currProject(nullptr)
{
    m_ui->setupUi(this);

    connect(m_ui->browse, SIGNAL(clicked(bool)), SLOT(browseDir()));
    connect(m_ui->btn, SIGNAL(rejected()), SLOT(hide()));
    connect(m_ui->btn, &QDialogButtonBox::accepted, [this]() {
        m_currProject->setName(m_ui->pname->text());
        m_currProject->setDest(m_ui->dest->text());
        save(m_currProject);
    });
}

SaveDialog::~SaveDialog()
{
    delete m_ui;
}

void SaveDialog::save(Project* project)
{
    if (!project)
        return;

    hide();
    QProgressDialog progress("Saving project...", QString(), 0, 100);
    progress.setWindowModality(Qt::ApplicationModal);
    connect(project, SIGNAL(progressSave(int)), &progress, SLOT(setValue(int)));
    progress.setValue(0);
    progress.show();
    project->saveProject();
    m_currProject = project;
    progress.setValue(100);
}

void SaveDialog::saveAs(Project* project)
{
    if (!project)
        return;

    m_ui->pname->setText(project->getName());
    m_ui->dest->setText(project->getDest().isEmpty() ? QDir::homePath() : project->getDest());
    m_currProject = project;
    show();
}

void SaveDialog::browseDir()
{
    m_ui->dest->setText(QFileDialog::getExistingDirectory(this, tr("Project Directory"),
            m_ui->dest->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

}
