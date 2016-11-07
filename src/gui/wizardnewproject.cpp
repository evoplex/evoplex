#include <QFileDialog>
#include <QMessageBox>
#include <QtDebug>

#include "core/project.h"
#include "gui/maingui.h"
#include "gui/wizardnewproject.h"
#include "ui_wizardnewproject.h"

WizardNewProject::WizardNewProject(MainApp* mainApp, QWidget* parent)
    : QWizard(parent)
    , m_ui(new Ui::WizardNewProject)
    , m_mainApp(mainApp)
{
    m_ui->setupUi(this);

    connect(m_ui->bBrowseModel, SIGNAL(clicked()), this, SLOT(slotBrowseModel()));
    connect(m_ui->bBrowsePath, SIGNAL(clicked(bool)), this, SLOT(slotBrowseProjectDir()));
    connect(m_ui->modelList, SIGNAL(itemSelectionChanged()), this, SLOT(slotModelSelected()));

    m_ui->projectName->setText("project");
    m_ui->projectPath->setText(QDir::homePath() + "/evoplex");
    updateModels();
}

WizardNewProject::~WizardNewProject()
{
    delete m_ui;
    m_ui = NULL;
}

void WizardNewProject::slotBrowseModel()
{
    QString path = QFileDialog::getOpenFileName(
                this, tr("Load Model"), QDir::homePath(),
                tr("Evoplex Model (*.so)"));

    if (!path.isEmpty() && m_mainApp->loadModel(path).isEmpty()) {
        QMessageBox::critical(this, tr("Load Model"), tr("Unable to load the selected model!"));
        return;
    }
    updateModels();
}

void WizardNewProject::slotBrowseProjectDir()
{
    QString dir = QFileDialog::getExistingDirectory(
                this->parentWidget(), tr("Project Directory"), m_ui->projectPath->text(),
                QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    m_ui->projectPath->setText(dir);
}

void WizardNewProject::slotModelSelected()
{
    QString name = m_ui->modelList->currentIndex().data().toString();
    m_ui->modelDescr->setText(m_mainApp->getModel(name)->description);
}

void WizardNewProject::updateModels()
{
    QList<MainApp::Model*> models = m_mainApp->getModels();
    foreach (MainApp::Model* m, models) {
        m_ui->modelList->addItem(m->name);
    }
    m_ui->modelList->setCurrentRow(0);
}

bool WizardNewProject::validateCurrentPage()
{
    if (currentId() == 0 && m_ui->modelList->selectedItems().isEmpty()) {
        QMessageBox::warning(this, "Select Model", "You must choose a model!");
        return false;
    } else if (currentId() == 1 && m_ui->projectName->text().isEmpty()) {
        QMessageBox::warning(this, "Project Details", "The project must have a name!");
        return false;
    } else if (currentId() > 1) {
        QMessageBox::critical(this, "New Project", "Unable to validate the current page!");
        qWarning() << "[WizardNewProject] unable to validate the current page!";
        return false;
    }
    return true;
}

void WizardNewProject::done(int result)
{
    QWizard::done(result);
    if (result == 0) {
        return;
    }

    QString mname = m_ui->modelList->currentIndex().data().toString();
    QString pname = m_ui->projectName->text();
    QString pdescr = m_ui->projectDescr->toPlainText();
    QString pdir = m_ui->projectPath->text();
    Project* project = m_mainApp->newProject(mname, pname, pdescr, pdir);
    emit (newProject(project));
}
