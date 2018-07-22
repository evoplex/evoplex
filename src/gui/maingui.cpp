/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QDesktopServices>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSettings>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "maingui.h"
#include "fontstyles.h"
#include "projectwidget.h"
#include "savedialog.h"
#include "pluginspage.h"
#include "projectspage.h"
#include "queuepage.h"
#include "settingspage.h"
#include "welcomepage.h"

#include "../config.h"
#include "core/logger.h"
#include "core/project.h"

namespace evoplex {

MainGUI::MainGUI(MainApp* mainApp)
    : QMainWindow(nullptr)
    , m_mainApp(mainApp)
    , m_colorMapMgr(new ColorMapMgr)
    , m_saveDialog(new SaveDialog(this))
    , m_welcome(new WelcomePage(this))
    //, m_queue(new QueuePage(this))
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
    this->setContextMenuPolicy(Qt::NoContextMenu);

    //
    // central widget
    //
    QHBoxLayout* centralLayout = new QHBoxLayout(new QWidget(this));
    centralLayout->addWidget(m_welcome);
    //centralLayout->addWidget(m_queue);
    centralLayout->addWidget(m_projectsPage);
    centralLayout->addWidget(m_plugins);
    centralLayout->addWidget(m_settings);
    this->setCentralWidget(centralLayout->parentWidget());
    m_welcome->hide();
    //m_queue->hide();
    m_projectsPage->hide();
    m_plugins->hide();
    m_settings->hide();

    //
    // left toolbar
    //
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setObjectName("toolbar");
    QActionGroup* actionGroup = new QActionGroup(toolbar);
    QAction* acWelcome = new QAction(QIcon(":/icons/evoplex.svg"), "Welcome", actionGroup);
    acWelcome->setCheckable(true);
    acWelcome->setData(PAGE_WELCOME);
    /* FIXME: queue widget is broken yet -- let's hide the button
    QAction* acQueue = new QAction(QIcon(":/icons/queue.svg"), "Queue", actionGroup);
    acQueue->setCheckable(true);
    acQueue->setData(PAGE_QUEUE);
    */
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

    toolbar->setIconSize(QSize(32, 32));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    toolbar->setMovable(false);
    toolbar->setFloatable(false);
    this->addToolBar(Qt::LeftToolBarArea, toolbar);
    // remove the tooltips
    for (QToolButton* btn : toolbar->findChildren<QToolButton*>()) {
        btn->installEventFilter(this);
    }

    connect(actionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotPage(QAction*)));
    connect(m_projectsPage, &ProjectsPage::isEmpty, [this, acWelcome, acProjects](bool b) {
        acProjects->setDisabled(b);
        if (b && m_curPage == PAGE_PROJECTS)
            acWelcome->trigger();
    });
    /* FIXME: queue widget is broken yet -- let's hide the button
    connect(m_queue, &QueuePage::isEmpty, [this, acWelcome, acQueue](bool b) {
        acQueue->setDisabled(b);
        if (b && m_curPage == PAGE_QUEUE)
            acWelcome->trigger();
    });
    acQueue->setDisabled(true);
    */
    acProjects->setDisabled(true);
    acWelcome->setChecked(true);
    slotPage(acWelcome);

    //
    // menubar
    //
    m_actNewProject = new QAction("New Project", this);
    connect(this, SIGNAL(newProject()), m_actNewProject, SIGNAL(triggered()));
    connect(m_actNewProject, &QAction::triggered, [this, acProjects]() {
        if (m_projectsPage->slotNewProject()) slotPage(acProjects);
    });
    m_actOpenProject = new QAction("Open Project", this);

    connect(m_actOpenProject, &QAction::triggered, [this]() { emit(openProject("")); });
    connect(this, &MainGUI::openProject, [this, acProjects](const QString& path) {
        if (m_projectsPage->slotOpenProject(path)) slotPage(acProjects);
    });

    QAction* actSaveAll = new QAction("Save All", this);
    connect(actSaveAll, SIGNAL(triggered(bool)), this, SLOT(slotSaveAll()));
    m_actSave = new QAction("Save", this);
    m_actSave->setEnabled(false);
    connect(m_actSave, SIGNAL(triggered(bool)), this, SLOT(slotSave()));
    m_actSaveAs = new QAction("Save as", this);
    m_actSaveAs->setEnabled(false);
    connect(m_actSaveAs, SIGNAL(triggered(bool)), this, SLOT(slotSaveAs()));
    connect(m_projectsPage, SIGNAL(activeProjectChanged(int)), SLOT(updateSaveButtons(int)));
    connect(m_projectsPage, SIGNAL(hasUnsavedChanges(int)), SLOT(updateSaveButtons(int)));

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

    QMenu* menuHelp = new QMenu("Help", this);
    QAction* acLog = new QAction("Log", this);
    connect(acLog, SIGNAL(triggered(bool)), SLOT(slotShowLog()));
    menuHelp->addAction(acLog);
    QAction* acAbout = new QAction("About", this);
    connect(acAbout, SIGNAL(triggered(bool)), SLOT(slotShowAbout()));
    menuHelp->addAction(acAbout);
    this->menuBar()->addMenu(menuHelp);

    QSettings userPrefs;
    restoreGeometry(userPrefs.value("gui/geometry").toByteArray());
    restoreState(userPrefs.value("gui/windowState").toByteArray());
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
    for (ProjectWidget* pw : m_projectsPage->projWidgets()) {
        if (!pw->close()) {
            event->ignore();
            return;
        }
    }

    QSettings userPrefs;
    userPrefs.setValue("gui/geometry", saveGeometry());
    userPrefs.setValue("gui/windowState", saveState());
    qDebug() << "user settings stored at " << userPrefs.fileName();

    event->accept();
    QMainWindow::closeEvent(event);
}

void MainGUI::slotPage(QAction* action)
{
    Page page = static_cast<Page>(action->data().toInt());
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
            //m_queue->setVisible(visible);
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

void MainGUI::updateSaveButtons(int projId)
{
    ProjectPtr proj = m_mainApp->project(projId);
    if (proj) {
        m_actSave->setEnabled(proj->hasUnsavedChanges());
        m_actSaveAs->setEnabled(true);
        m_actSave->setText(QString("Save \"%1\"").arg(proj->name()));
        m_actSaveAs->setText(QString("Save \"%1\" As").arg(proj->name()));
    } else {
        m_actSave->setEnabled(false);
        m_actSaveAs->setEnabled(false);
        m_actSave->setText("Save");
        m_actSaveAs->setText("Save As");
    }
}

void MainGUI::slotSaveAs()
{
    if (m_projectsPage->activeProject()) {
        m_saveDialog->saveAs(m_projectsPage->activeProject());
    }
}

void MainGUI::slotSave()
{
    if (m_projectsPage->activeProject()) {
        m_saveDialog->save(m_projectsPage->activeProject());
    }
}

void MainGUI::slotSaveAll()
{
    for (auto& p : m_mainApp->projects()) {
        m_saveDialog->save(p.second);
    }
}

void MainGUI::slotShowLog()
{
    QDialog* d = new QDialog(this);
    QVBoxLayout* l = new QVBoxLayout(d);
    l->addWidget(new QLabel("Use the following to provide more detailed information about your system to bug reports:"));

    QPlainTextEdit* text = new QPlainTextEdit(Logger::log(), d);
    text->setReadOnly(true);
    text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l->addWidget(text);

    l->addWidget(new QLabel("Writing log file to: " + Logger::logFileName()));

    QHBoxLayout* bts = new QHBoxLayout();
    bts->addSpacerItem(new QSpacerItem(5, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    QPushButton* refresh = new QPushButton("Refresh");
    connect(refresh, &QPushButton::pressed, [text](){ text->setPlainText(Logger::log()); });
    bts->addWidget(refresh);
    QPushButton* copy = new QPushButton("Copy to Clipboard");
    connect(copy, &QPushButton::pressed, [text](){ text->selectAll(); text->copy(); });
    bts->addWidget(copy);
    QPushButton* location = new QPushButton("Open Location");
    connect(location, &QPushButton::pressed, [](){
        QDesktopServices::openUrl(QUrl("file:///"+Logger::logDir(), QUrl::TolerantMode)); });
    bts->addWidget(location);
    QPushButton* close = new QPushButton("Close");
    connect(close, &QPushButton::pressed, [d](){ d->close(); });
    bts->addWidget(close);
    l->addLayout(bts);
    close->setFocus();

    d->setModal(true);
    d->setLayout(l);
    d->show();
}

void MainGUI::slotShowAbout()
{
    QString version = "Evoplex " + qApp->applicationVersion();
    QString txt = "Evoplex is a multi-agent system for networks (graphs).\n\n"
                  "Based on Qt " QT_VERSION_STR " (" COMPILER_VERSION " " COMPILER_ARCHITECTURE ")\n\n"
                  "Built on " EVOPLEX_BUILDDATE "\n\n"
                  "From revision " EVOPLEX_GIT_COMMIT_HASH " (" EVOPLEX_GIT_BRANCH ")\n\n"
                  "Copyright 2016-" + QDate::currentDate().toString("yyyy") + " Marcos Cardinot et al.";
    QMessageBox::about(this, version, txt);
}

} // evoplex
