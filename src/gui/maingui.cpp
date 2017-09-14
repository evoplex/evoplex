/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "gui/maingui.h"

MainGUI::MainGUI(MainApp* mainApp, QWidget* parent)
    : QMainWindow(parent)
    , m_mainApp(mainApp)
    , m_welcome(new WelcomeWidget(this))
    , m_queue(new QueueWidget(mainApp->getExperimentsMgr(), this))
    , m_projects(new ProjectsWindow(mainApp, this))
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
    centralLayout->addWidget(m_projects);
    this->setCentralWidget(centralLayout->parentWidget());
    m_welcome->hide();
    m_queue->hide();
    m_projects->hide();

    //
    // toolbar
    //
    QToolBar* toolbar = new QToolBar(this);
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
    toolbar->addAction("Plugins");
    toolbar->addAction("Settings");
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
    connect(m_projects, SIGNAL(isEmpty(bool)), acProjects, SLOT(setDisabled(bool)));
    connect(m_queue, SIGNAL(isEmpty(bool)), acQueue, SLOT(setDisabled(bool)));
    acProjects->setDisabled(true);
    acQueue->setDisabled(true);
    acWelcome->setChecked(true);
    slotPage(acWelcome);

    //
    // menubar
    //
    QAction* actNewProject = new QAction("New Project", this);
    connect(actNewProject, &QAction::triggered, [this, acProjects](){ m_projects->slotNewProject(); slotPage(acProjects); });
    QAction* actOpenProject = new QAction("Open Project", this);
    connect(actOpenProject, SIGNAL(triggered(bool)), this, SLOT(slotOpenProject()));
    QAction* actSave = new QAction("Save Project", this);
    connect(actSave, SIGNAL(triggered(bool)), this, SLOT(slotSave()));
    QAction* actSaveAs = new QAction("Save Project As ...", this);
    connect(actSaveAs, SIGNAL(triggered(bool)), this, SLOT(slotSaveAs()));
    QAction* actSaveAll = new QAction("Save All", this);
    connect(actSaveAll, SIGNAL(triggered(bool)), this, SLOT(slotSaveAll()));
    QAction* actQuit = new QAction("Quit", this);
    connect(actQuit, SIGNAL(triggered(bool)), this, SLOT(close()));

    QMenu* menuFile = new QMenu("File", this);
    menuFile->addAction(actNewProject);
    menuFile->addAction(actOpenProject);
    menuFile->addAction(actSave);
    menuFile->addAction(actSaveAs);
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
}

bool MainGUI::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::ToolTip)
      return true;    // remove the tooltips

    return QMainWindow::eventFilter(o, e);
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
            m_projects->setVisible(visible);
            break;
        default:
            m_welcome->setVisible(visible);
            break;
    }
}
