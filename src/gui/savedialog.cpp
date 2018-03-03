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
        m_currProject->setFilePath(QString("%1/%2.csv")
                .arg(m_ui->dest->text()).arg(m_ui->pname->text()));
        save(m_currProject);
    });
}

SaveDialog::~SaveDialog()
{
    delete m_ui;
}

bool SaveDialog::save(ProjectSP project)
{
    if (!project) {
        return false;
    } else if (project->filepath().isEmpty()) {
        return saveAs(project);
    }

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
        return false;
    }
    return true;
}

bool SaveDialog::saveAs(ProjectSP project)
{
    if (!project) {
        return false;
    }
    m_ui->pname->setText(project->name());
    m_ui->dest->setText(project->filepath().isEmpty() ? QDir::homePath() : QFileInfo(project->filepath()).absolutePath());
    m_currProject = project;
    return exec() == QDialog::Accepted;
}

void SaveDialog::browseDir()
{
    m_ui->dest->setText(QFileDialog::getExistingDirectory(this, tr("Project Directory"),
            m_ui->dest->text(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

}
