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

#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "ui_pluginspage.h"
#include "pluginspage.h"
#include "fontstyles.h"

#include "core/graphplugin.h"
#include "core/modelplugin.h"

namespace evoplex {

PluginsPage::PluginsPage(MainGUI* mainGUI)
    : QWidget(mainGUI)
    , m_ui(new Ui_PluginsPage)
    , m_mainApp(mainGUI->mainApp())
    , m_innerWindow(new QMainWindow())
{
    m_ui->setupUi(this);

    m_innerWindow->setStyleSheet("QMainWindow { background-color: rgb(24,24,24); }");
    m_innerWindow->setCentralWidget(m_ui->table);
    m_ui->dockBrowser->setTitleBarWidget(nullptr);
    m_innerWindow->addDockWidget(Qt::RightDockWidgetArea, m_ui->dockBrowser);
    m_ui->iwLayout->addWidget(m_innerWindow);

    m_ui->labelPlugins->setFont(FontStyles::subtitle1());

    connect(m_ui->bImport, SIGNAL(pressed()), SLOT(importPlugin()));
    connect(m_ui->table, SIGNAL(itemSelectionChanged()), SLOT(rowSelectionChanged()));

    for (GraphPlugin* g : m_mainApp->graphs()) { insertRow(g); }
    for (ModelPlugin* m : m_mainApp->models()) { insertRow(m); }
    connect(m_mainApp, SIGNAL(pluginAdded(const Plugin*)),
            SLOT(insertRow(const Plugin*)));

    QSettings s;
    m_innerWindow->restoreGeometry(s.value("gui/pluginsPageGeometry").toByteArray());
    m_innerWindow->restoreState(s.value("gui/pluginsPageState").toByteArray());
}

PluginsPage::~PluginsPage()
{
    QSettings s;
    s.setValue("gui/pluginsPageGeometry", m_innerWindow->saveGeometry());
    s.setValue("gui/pluginsPageState", m_innerWindow->saveState());
    delete m_ui;
}

void PluginsPage::rowSelectionChanged()
{
    int row = m_ui->table->currentRow();
    if (row < 0) {
        return;
    }

    const Plugin* plugin = m_mainApp->graph(m_ui->table->item(row, UID)->text());
    if (!plugin) {
        plugin = m_mainApp->model(m_ui->table->item(row, UID)->text());
    }
    loadHtml(plugin);
}

void PluginsPage::loadHtml(const Plugin* plugin)
{
    if (!plugin) {
        m_ui->browser->clear();
        return;
    }

    QString html = "<h2>" + plugin->name() + "</h2><br>"
                 + "<b>Author:</b> " + plugin->author() + "<br>"
                 + "<b>Plugin type:</b> " + _enumToString<PluginType>(plugin->type()) + "<br>"
                 + "<b>Description:</b> " + plugin->description() + "<br>";

    m_ui->browser->setHtml(html);
}

void PluginsPage::importPlugin()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Plugin"),
            QDir::homePath(), tr("Evoplex Plugin (*%1)").arg(MainApp::kPluginExtension));

    if (fileName.isEmpty()) {
        return;
    }

    QString error;
    if (!m_mainApp->loadPlugin(fileName, error, true)) {
        QMessageBox::warning(this, "Error", error);
    }
}

void PluginsPage::insertRow(const Plugin* plugin)
{
    if (!plugin) {
        return;
    }

    QTableWidgetItem* typeItem;
    if (plugin->type() == PluginType::Model) {
        typeItem = new QTableWidgetItem("model");
    } else if (plugin->type() == PluginType::Graph) {
        typeItem = new QTableWidgetItem("graph");
    } else {
        qFatal("invalid plugin type! It should never happen.");
    }
    typeItem->setData(Qt::UserRole, static_cast<int>(plugin->type()));

    QPushButton* bUnload = new QPushButton();
    bUnload->setFlat(true);
    bUnload->installEventFilter(new ButtonHoverWatcher(this));

    m_ui->table->setSortingEnabled(false);
    int row = m_ui->table->rowCount();
    m_ui->table->insertRow(row);
    m_ui->table->setItem(row, UID, new QTableWidgetItem(plugin->id()));
    m_ui->table->setItem(row, NAME, new QTableWidgetItem(plugin->name()));
    m_ui->table->setItem(row, TYPE, typeItem);
    m_ui->table->setCellWidget(row, UNLOAD, bUnload);
    m_ui->table->setSortingEnabled(true);

    m_ui->table->horizontalHeader()->setSectionResizeMode(UID, QHeaderView::ResizeToContents);
    m_ui->table->horizontalHeader()->setSectionResizeMode(TYPE, QHeaderView::ResizeToContents);
    m_ui->table->horizontalHeader()->setSectionResizeMode(NAME, QHeaderView::ResizeToContents);
    m_ui->table->horizontalHeader()->setSectionResizeMode(UNLOAD, QHeaderView::ResizeToContents);

    connect(bUnload, &QPushButton::pressed, [this, typeItem, plugin]() {
        int res = QMessageBox::question(this, "Unloading Plugin",
                        QString("Are you sure you want to unload the plugin '%1'?").arg(plugin->name()));
        if (res == QMessageBox::No) {
            return;
        }

        QString error;
        if (m_mainApp->unloadPlugin(plugin, error)) {
            m_ui->table->clearSelection();
            m_ui->table->removeRow(typeItem->row());
        } else {
            QMessageBox::warning(this, "Unloading Plugin", error);
        }
    });
}

bool ButtonHoverWatcher::eventFilter(QObject* watched, QEvent* event)
{
    auto button = qobject_cast<QPushButton*>(watched);
    if (!button) {
        return false;
    } else if (event->type() == QEvent::Enter) { // hovered
        button->setIcon(QIcon(":/icons/x.png"));
        return true;
    } else if (event->type() == QEvent::Leave){
        button->setIcon(QIcon());
        return true;
    }
    return false;
}

}
