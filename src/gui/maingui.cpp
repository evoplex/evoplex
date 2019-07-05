/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a platform for agent-based modeling on networks.
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
#include <QStatusBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>

#include "../config.h"
#include "core/logger.h"
#include "core/project.h"

#include "external/qt-material-widgets/qtmaterialflatbutton.h"

#include "maingui.h"
#include "fontstyles.h"
#include "consolewidget.h"
#include "projectwidget.h"
#include "savedialog.h"
#include "graphdesignerpage.h"
#include "pluginspage.h"
#include "projectspage.h"
#include "queuepage.h"
#include "settingspage.h"
#include "welcomepage.h"

namespace evoplex {

// By default, a QDialog should have only the close button
// It also ensures that the '?' button is not shown on Windows
const QFlags<Qt::WindowType> MainGUI::kDefaultDlgFlags = Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint | Qt::WindowSystemMenuHint | Qt::WindowTitleHint;

MainGUI::MainGUI(MainApp* mainApp)
    : QMainWindow(nullptr),
      m_mainApp(mainApp),
      m_colorMapMgr(new ColorMapMgr),
      m_saveDialog(new SaveDialog(this)),
      m_welcome(new WelcomePage(this)),
    // m_queue(new QueuePage(this)),
      m_projectsPage(new ProjectsPage(this)),
      m_plugins(new PluginsPage(this)),
      m_settings(new SettingsPage(this)),
      m_console(new ConsoleWidget(this)),
      m_graphs(new GraphDesignerPage(this)),
      m_curPage(PAGE_NULL)
{
    // main window
    setObjectName("MainGUI");
    setWindowTitle("Evoplex");
    setWindowIconText("Evoplex");
    setWindowIcon(QIcon(":/icons/evoplex.svg"));
    setGeometry(100,100,800,500);
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::South);
    setDockNestingEnabled(true);
    setContextMenuPolicy(Qt::NoContextMenu);

    //
    // central widget
    //
    QHBoxLayout* centralLayout = new QHBoxLayout(new QWidget(this));
    centralLayout->setContentsMargins(0,0,0,0);
    centralLayout->addWidget(m_welcome);
    //centralLayout->addWidget(m_queue);
    centralLayout->addWidget(m_projectsPage);
    centralLayout->addWidget(m_plugins);
    centralLayout->addWidget(m_settings);
    centralLayout->addWidget(m_graphs);
    setCentralWidget(centralLayout->parentWidget());
    m_welcome->hide();
    //m_queue->hide();
    m_projectsPage->hide();
    m_plugins->hide();
    m_settings->hide();
    m_graphs->hide();

    //
    // left toolbar
    //
    QToolBar* toolbar = new QToolBar(this);
    toolbar->setObjectName("mainToolbar");
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
    QAction* acGraphs = new QAction(QIcon(":/icons/projects.svg"), "Graphs", actionGroup);
    acGraphs->setCheckable(true);
    acGraphs->setData(PAGE_GRAPHS);
    toolbar->addActions(actionGroup->actions());
    wspacer = new QWidget(this);
    wspacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    toolbar->addWidget(wspacer);
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
    addToolBar(Qt::LeftToolBarArea, toolbar);
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
    menuBar()->addMenu(menuFile);

    QMenu* menuHelp = new QMenu("Help", this);
    QAction* acLog = new QAction("Log", this);
    connect(acLog, SIGNAL(triggered(bool)), SLOT(slotShowLog()));
    menuHelp->addAction(acLog);
    QAction* acAbout = new QAction("About", this);
    connect(acAbout, SIGNAL(triggered(bool)), SLOT(slotShowAbout()));
    menuHelp->addAction(acAbout);
    QAction* acUpdates = new QAction("Check for Updates", this);
    connect(acUpdates, SIGNAL(triggered(bool)), m_mainApp, SLOT(checkForUpdates()));
    menuHelp->addAction(acUpdates);
    menuBar()->addMenu(menuHelp);

    QSettings userPrefs;
    restoreGeometry(userPrefs.value("gui/geometry").toByteArray());
    restoreState(userPrefs.value("gui/windowState").toByteArray());

    //
    // status bar
    //
    setStatusBar(new QStatusBar(this));
    auto bWarning = new QtMaterialFlatButton(this);
    bWarning->setCheckable(true);
    bWarning->setFont(FontStyles::caption());
    bWarning->setForegroundColor(Qt::white);
    bWarning->setOverlayColor(Qt::lightGray);
    bWarning->setIcon(QIcon(":/icons/material/warning_white_18"));
    bWarning->setText("0");
    bWarning->setMaximumHeight(18);
    bWarning->setChecked(false);
    m_console->setVisible(false);
    statusBar()->addPermanentWidget(bWarning);
    addDockWidget(Qt::BottomDockWidgetArea, m_console);
    connect(bWarning, SIGNAL(toggled(bool)), m_console, SLOT(setVisible(bool)));
    connect(m_console, SIGNAL(visibilityChanged(bool)), bWarning, SLOT(setChecked(bool)));
    connect(m_console, &ConsoleWidget::warningsCountChanged, [bWarning](int v) {
        bWarning->setText(QString::number(v));
    });

    // checks for updates
    connect(m_mainApp, SIGNAL(newVersionAvailable(QVariantMap)),
            SLOT(slotCheckedForUpdates(QVariantMap)));
    if (m_mainApp->checkUpdatesAtStart()) {
        QTimer::singleShot(1000, m_mainApp, SLOT(checkForUpdates()));
    }
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
        case PAGE_GRAPHS:
            m_graphs->setVisible(visible);
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

void MainGUI::slotCheckedForUpdates(const QVariantMap& data)
{
    auto url = data.value("url").toString();
    auto vs = data.value("version").toString();
    if (url.isEmpty()) {
        url = QCoreApplication::organizationDomain();
    }
    QMessageBox::information(this, "Updates",
            "A new version of Evoplex has been released!<br><br>"
            "You can download this version (" + vs + ") using the link:<br>"
            "<a href='" + url + "'>" + url + "</a>");
}

void MainGUI::slotShowLog()
{
    QDialog* d = new QDialog(this, MainGUI::kDefaultDlgFlags);
    QVBoxLayout* l = new QVBoxLayout(d);
    l->addWidget(new QLabel("Use the following to provide more detailed information about your system to bug reports:"));

    QPlainTextEdit* text = new QPlainTextEdit(Logger::instance()->log(), d);
    text->setReadOnly(true);
    text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l->addWidget(text);

    l->addWidget(new QLabel("Writing log file to: " + Logger::instance()->logFileName()));

    QHBoxLayout* bts = new QHBoxLayout();
    bts->addSpacerItem(new QSpacerItem(5, 5, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum));
    QPushButton* refresh = new QPushButton("Refresh");
    connect(refresh, &QPushButton::pressed, [text](){ text->setPlainText(Logger::instance()->log()); });
    bts->addWidget(refresh);
    QPushButton* copy = new QPushButton("Copy to Clipboard");
    connect(copy, &QPushButton::pressed, [text](){ text->selectAll(); text->copy(); });
    bts->addWidget(copy);
    QPushButton* location = new QPushButton("Open Location");
    connect(location, &QPushButton::pressed, [](){
        QDesktopServices::openUrl(QUrl("file:///"+Logger::instance()->logDir(), QUrl::TolerantMode)); });
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
    QString txt =
        "Evoplex is a fast, robust and extensible platform for developing "
        "agent-based models and multi-agent systems on networks. Each agent "
        "is represented as a node and interacts with its neighbors, as "
        "defined by the network structure."
        "<br><br>"
        "Based on Qt " QT_VERSION_STR " (" COMPILER_VERSION " " COMPILER_ARCHITECTURE ")"
        "<br><br>"
        "Built on " EVOPLEX_BUILDDATE
        "<br><br>"
        "From revision " EVOPLEX_GIT_COMMIT_HASH " (" EVOPLEX_GIT_BRANCH ")"
        "<br><br>"
        "Copyright 2016-" + QDate::currentDate().toString("yyyy") + " Marcos Cardinot et al.<br>"
        "Website: <a href='" + qApp->organizationDomain() + "'>" + qApp->organizationDomain() + "</a><br>"
        "Follow us on Twitter: <a href='https://twitter.com/EvoplexOrg'>@EvoplexOrg</a>";
    QMessageBox::about(this, version, txt);
}

} // evoplex
