/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <functional>

#include "savedialog.h"
#include "ui_savedialog.h"

namespace evoplex {

SaveDialog::SaveDialog(QWidget *parent)
    : QDialog(parent)
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
    QProgressDialog progress("Saving project...", QString(), 0, 100, this);
    progress.setWindowModality(Qt::ApplicationModal);
    progress.show();

    QString errMsg;
    std::function<void(int)> f = [&progress](int p){ progress.setValue(p); };
    project->saveProject(errMsg, f);
    m_currProject = project;
    if (!errMsg.isEmpty()) {
        QMessageBox::warning(this, "Saving project...", errMsg);
    }
}

void SaveDialog::saveAs(Project* project)
{
    if (!project)
        return;

    m_ui->pname->setText(project->name());
    m_ui->dest->setText(project->dest().isEmpty() ? QDir::homePath() : project->dest());
    m_currProject = project;
    show();
}

void SaveDialog::browseDir()
{
    m_ui->dest->setText(QFileDialog::getExistingDirectory(this, tr("Project Directory"),
            m_ui->dest->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

}
