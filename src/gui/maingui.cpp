/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "maingui.h"
#include "projectwidget.h"
#include "savedialog.h"
#include "pluginspage.h"
#include "projectspage.h"
#include "queuepage.h"
#include "settingspage.h"
#include "welcomepage.h"

#include "core/project.h"

namespace evoplex {

MainGUI::MainGUI(MainApp* mainApp, QWidget* parent)
    : QMainWindow(parent)
    , m_mainApp(mainApp)
    , m_colorMapMgr(new ColorMapMgr)
    , m_saveDialog(new SaveDialog(this))
    , m_welcome(new WelcomePage(this))
    , m_queue(new QueuePage(this))
    , m_projectsPage(new ProjectsPage(this))
    , m_plugins(new PluginsPage(this))
    , m_settings(new SettingsPage(this))
    , m_curPage(PAGE_NULL)
{
    // main window
    this->setWindowTitle("Evoplex");
    this->setWindowIconText("Evoplex");
    this->setWindowIcon(QIcon(":/icons/evoplex.svg"));
    this->setGeometry(100,100,800,500);
    this->setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    this->setDockNestingEnabled(true);

    //
    // central widget
    //
    QHBoxLayout* centralLayout = new QHBoxLayout(new QWidget(this));
    centralLayout->addWidget(m_welcome);
    centralLayout->addWidget(m_queue);
    centralLayout->addWidget(m_projectsPage);
    centralLayout->addWidget(m_plugins);
    centralLayout->addWidget(m_settings);
    this->setCentralWidget(centralLayout->parentWidget());
    m_welcome->hide();
    m_queue->hide();
    m_projectsPage->hide();
    m_plugins->hide();
    m_settings->hide();

    //
    // toolbar
    //
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setObjectName("toolbar");
    QActionGroup* actionGroup = new QActionGroup(toolbar);
    QAction* acWelcome = new QAction(QIcon(":/icons/evoplex.svg"), "Welcome", actionGroup);
    acWelcome->setCheckable(true);
    acWelcome->setData(PAGE_WELCOME);
    QAction* acQueue = new QAction(QIcon(":/icons/queue.svg"), "Queue", actionGroup);
    acQueue->setCheckable(true);
    acQueue->setData(PAGE_QUEUE);
    QAction* acProjects = new QAction(QIcon(":/icons/projects.svg"),"Projects", actionGroup);
    acProjects->setCheckable(true);
    acProjects->setData(PAGE_PROJECTS);
    toolbar->addActions(actionGroup->actions());
    auto wspacer = new QWidget(this);
    wspacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(wspacer);
    toolbar->addSeparator();
    QAction* acPlugins = new QAction(QIcon(":/icons/plugins.svg"), "Plugins", actionGroup);
    acPlugins->setCheckable(true);
    acPlugins->setData(PAGE_PLUGINS);
    toolbar->addAction(acPlugins);
    QAction* acSettings = new QAction(QIcon(":/icons/settings.svg"), "Settings", actionGroup);
    acSettings->setCheckable(true);
    acSettings->setData(PAGE_SETTINGS);
    toolbar->addAction(acSettings);

    toolbar->setStyleSheet("font: 8pt;");
    toolbar->setIconSize(QSize(32, 32));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    this->addToolBar(Qt::LeftToolBarArea, toolbar);
    // remove the tooltips
    foreach(QToolButton* btn, toolbar->findChildren<QToolButton*>())
        btn->installEventFilter(this);

    connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotPage(QAction*)));
    connect(m_projectsPage, &ProjectsPage::isEmpty,
        [this, acWelcome, acProjects](bool b) {
            acProjects->setDisabled(b);
            if (b && m_curPage == PAGE_PROJECTS)
                acWelcome->trigger();
    });
    connect(m_queue, &QueuePage::isEmpty,
        [this, acWelcome, acQueue](bool b) {
            acQueue->setDisabled(b);
            if (b && m_curPage == PAGE_QUEUE)
                acWelcome->trigger();
    });

    acProjects->setDisabled(true);
    acQueue->setDisabled(true);
    acWelcome->setChecked(true);
    slotPage(acWelcome);

    //
    // menubar
    //
    m_actNewProject = new QAction("New Project", this);
    connect(this, SIGNAL(newProject()), m_actNewProject, SIGNAL(triggered()));
    connect(m_actNewProject, &QAction::triggered,
            [this, acProjects](){ m_projectsPage->slotNewProject(); slotPage(acProjects); });
    m_actOpenProject = new QAction("Open Project", this);

    connect(this, SIGNAL(openProject()), m_actOpenProject, SIGNAL(triggered()));
    connect(m_actOpenProject, &QAction::triggered,
            [this, acProjects](){ if (m_projectsPage->slotOpenProject()) slotPage(acProjects); });

    QAction* actSaveAll = new QAction("Save All", this);
    connect(actSaveAll, SIGNAL(triggered(bool)), this, SLOT(slotSaveAll()));
    m_actSave = new QAction("Save", this);
    m_actSave->setEnabled(false);
    connect(m_actSave, SIGNAL(triggered(bool)), this, SLOT(slotSave()));
    m_actSaveAs = new QAction("Save as", this);
    m_actSaveAs->setEnabled(false);
    connect(m_actSaveAs, SIGNAL(triggered(bool)), this, SLOT(slotSaveAs()));
    connect(m_projectsPage, SIGNAL(selectionChanged(ProjectWidget*)), SLOT(updateSaveButtons(ProjectWidget*)));
    connect(m_projectsPage, SIGNAL(hasUnsavedChanges(ProjectWidget*)), SLOT(updateSaveButtons(ProjectWidget*)));

    QAction* actQuit = new QAction("Quit", this);
    connect(actQuit, SIGNAL(triggered(bool)), this, SLOT(close()));

    QMenu* menuFile = new QMenu("File", this);
    menuFile->addAction(m_actNewProject);
    menuFile->addAction(m_actOpenProject);
    menuFile->addAction(m_actSave);
    menuFile->addAction(m_actSaveAs);
    menuFile->addAction(actSaveAll);
    menuFile->addSeparator();
    menuFile->addAction(actQuit);
    this->menuBar()->addMenu(menuFile);

    QMenu* menuPlugins = new QMenu("Plugins", this);
        // open dialog with plugin stuff (list of plugins, allow adding)
    this->menuBar()->addMenu(menuPlugins);
    QMenu* menuSettings = new QMenu("Settings", this);
        // generate template
    this->menuBar()->addMenu(menuSettings);
    QMenu* menuAbout = new QMenu("About", this);
    this->menuBar()->addMenu(menuAbout);

    QSettings s;
    restoreGeometry(s.value("gui/geometry").toByteArray());
    restoreState(s.value("gui/windowState").toByteArray());
}

MainGUI::~MainGUI()
{
    delete m_colorMapMgr;
}

bool MainGUI::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::ToolTip)
      return true;    // remove the tooltips

    return QMainWindow::eventFilter(o, e);
}

void MainGUI::closeEvent(QCloseEvent* event)
{
    for (ProjectWidget* pw : m_projectsPage->projects()) {
        if (!pw->close()) {
            event->ignore();
            return;
        }
    }

    QSettings s;
    s.setValue("gui/geometry", saveGeometry());
    s.setValue("gui/windowState", saveState());
    qDebug() << "[MainGUI]: user settings stored at " << s.fileName();

    event->accept();
    QMainWindow::closeEvent(event);
}

void MainGUI::slotPage(QAction* action)
{
    Page page = (Page) action->data().toInt();
    action->setChecked(true);
    if (m_curPage != page) {
        setPageVisible(m_curPage, false);
        setPageVisible(page, true);
        m_curPage = page;
    }
}

void MainGUI::setPageVisible(Page page, bool visible)
{
    switch (page) {
        case PAGE_QUEUE:
            m_queue->setVisible(visible);
            break;
        case PAGE_PROJECTS:
            m_projectsPage->setVisible(visible);
            break;
        case PAGE_PLUGINS:
            m_plugins->setVisible(visible);
            break;
        case PAGE_SETTINGS:
            m_settings->setVisible(visible);
            break;
        default:
            m_welcome->setVisible(visible);
            break;
    }
}

void MainGUI::updateSaveButtons(ProjectWidget* pw)
{
    if (pw && pw == m_projectsPage->currentProject() && pw->project()) {
        Project* project = pw->project();
        m_actSave->setEnabled(project->hasUnsavedChanges());
        m_actSaveAs->setEnabled(true);
        m_actSave->setText(QString("Save \"%1\"").arg(pw->objectName()));
        m_actSaveAs->setText(QString("Save \"%1\" As").arg(pw->objectName()));
    } else {
        m_actSave->setEnabled(false);
        m_actSaveAs->setEnabled(false);
        m_actSave->setText("Save");
        m_actSaveAs->setText("Save As");
    }
}

void MainGUI::slotSaveAs()
{
    if (m_projectsPage->currentProject()) {
        m_saveDialog->saveAs(m_projectsPage->currentProject()->project());
    }
}

void MainGUI::slotSave()
{
    if (m_projectsPage->currentProject()) {
        m_saveDialog->save(m_projectsPage->currentProject()->project());
    }
}

void MainGUI::slotSaveAll()
{
    for (Project* p : m_mainApp->projects()) {
        m_saveDialog->save(p);
    }
}

}
