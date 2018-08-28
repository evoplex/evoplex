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

#include <QFileDialog>
#include <QMessageBox>

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
{
    m_ui->setupUi(this);

    m_ui->labelPlugins->setFont(FontStyles::subtitle1());

    connect(m_ui->bImport, SIGNAL(pressed()), SLOT(importPlugin()));
    connect(m_ui->table, SIGNAL(itemSelectionChanged()),
            SLOT(rowSelectionChanged()));

    for (Plugin* p : m_mainApp->plugins()) {
        insertRow(p);
    }
    connect(m_mainApp, SIGNAL(pluginAdded(const Plugin*)),
            SLOT(insertRow(const Plugin*)));

    m_ui->bUnload->setVisible(false);
    m_ui->bReload->setVisible(false);
    connect(m_ui->bUnload, SIGNAL(pressed()), SLOT(slotUnload()));
    connect(m_ui->bReload, SIGNAL(pressed()), SLOT(slotReload()));
}

PluginsPage::~PluginsPage()
{
    delete m_ui;
}

void PluginsPage::slotUnload()
{
    auto key = m_ui->table->currentItem()->data(Qt::UserRole).value<PluginKey>();
    Plugin* plugin = m_mainApp->plugins().value(key);
    if (!plugin) {
        return;
    }

    int res = QMessageBox::question(this, "Unloading Plugin",
        "Are you sure you want to unload the plugin '"+plugin->id()+"'?");
    if (res == QMessageBox::No) {
        return;
    }

    QString error;
    if (m_mainApp->unloadPlugin(plugin, error)) {
        int row = m_ui->table->currentRow();
        m_ui->table->clearSelection();
        m_ui->table->removeRow(row);
    } else {
        QMessageBox::warning(this, "Unloading Plugin", error);
    }
}

void PluginsPage::slotReload()
{
    auto key = m_ui->table->currentItem()->data(Qt::UserRole).value<PluginKey>();
    Plugin* plugin = m_mainApp->plugins().value(key);
    if (!plugin) {
        return;
    }

    QString error;
    if (m_mainApp->reloadPlugin(plugin, error)) {
        int row = m_ui->table->currentRow();
        m_ui->table->clearSelection();
        m_ui->table->removeRow(row);
    } else {
        QMessageBox::warning(this, "Reloading Plugin", error);
    }
}

void PluginsPage::rowSelectionChanged()
{
    bool rowIsSelected = !m_ui->table->selectedItems().empty();
    m_ui->bUnload->setVisible(rowIsSelected);
    m_ui->bReload->setVisible(rowIsSelected);
    if (!rowIsSelected) { return; }

    auto key = m_ui->table->currentItem()->data(Qt::UserRole).value<PluginKey>();
    const Plugin* plugin = m_mainApp->graph(key);
    if (!plugin) {
        plugin = m_mainApp->model(key);
    }

    loadHtml(plugin);
}

void PluginsPage::loadHtml(const Plugin* plugin)
{
    if (!plugin) {
        m_ui->browser->clear();
        return;
    }

    QString html =
        "<h2>" + plugin->title() + "</h2><br>"
        "<b>Author:</b> " + plugin->author() + "<br>"
        "<b>Plugin type:</b> " + _enumToString<PluginType>(plugin->type()) + "<br>"
        "<b>Version:</b> " + QString::number(plugin->version()) + "<br>"
        "<br>"
        "<b>Description:</b><br>" + plugin->description() + "<br>"
        "<br>"
        "<b>Location:</b><br>" + plugin->path() + "<br>";

    if (!plugin->compactMetaData().isEmpty()) {
        html += "<br><b>Meta data:</b><br>" + plugin->compactMetaData() + "<br>";
    }

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

    QTableWidgetItem* uidItem = new QTableWidgetItem(plugin->id());
    auto keyv = QVariant::fromValue(plugin->key());
    typeItem->setData(Qt::UserRole, keyv);
    uidItem->setData(Qt::UserRole, keyv);

    m_ui->table->setSortingEnabled(false);
    int row = m_ui->table->rowCount();
    m_ui->table->insertRow(row);
    m_ui->table->setItem(row, TYPE, typeItem);
    m_ui->table->setItem(row, UID, uidItem);
    m_ui->table->setSortingEnabled(true);

    m_ui->table->horizontalHeader()->setSectionResizeMode(UID, QHeaderView::ResizeToContents);
    m_ui->table->horizontalHeader()->setSectionResizeMode(TYPE, QHeaderView::ResizeToContents);
}

} // evoplex
