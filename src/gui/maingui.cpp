#include "core/project.h"
#include "gui/maingui.h"
#include "gui/widgetproject.h"
#include "gui/wizardnewproject.h"
#include "ui_maingui.h"

MainGUI::MainGUI(MainApp* mainApp, QWidget* parent)
    : QMainWindow(parent)
    , m_ui(new Ui::MainGUI)
    , m_mainApp(mainApp)
    , m_wizardNewProject(new WizardNewProject(m_mainApp, this))
{
    m_ui->setupUi(this);

    connect(m_wizardNewProject, SIGNAL(newProject(Project*)), this, SLOT(addProject(Project*)));
    connect(m_ui->actionNewProject, SIGNAL(triggered(bool)), m_wizardNewProject, SLOT(show()));
}

MainGUI::~MainGUI()
{
    delete m_ui;
    m_ui = NULL;
}

void MainGUI::addProject(Project* project)
{
    WidgetProject* wp = new WidgetProject(project, m_ui->tableWidget);
    m_ui->tabWidget->addTab(wp, project->getName());
}
