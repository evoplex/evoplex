/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFileDialog>
#include <QMessageBox>

#include "ui_pluginswidget.h"
#include "pluginswidget.h"

namespace evoplex {

PluginsWidget::PluginsWidget(MainApp* mainApp, QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_PluginsWidget)
    , m_mainApp(mainApp)
{
    m_ui->setupUi(this);

    m_ui->table->horizontalHeader()->setSectionResizeMode(UID, QHeaderView::ResizeToContents);
    m_ui->table->horizontalHeader()->setSectionResizeMode(TYPE, QHeaderView::ResizeToContents);

    connect(m_ui->bImport, SIGNAL(pressed()), SLOT(importPlugin()));
    connect(m_ui->table, SIGNAL(itemSelectionChanged()), SLOT(rowSelectionChanged()));

    QHash<QString, GraphPlugin*>::const_iterator it;
    for (it = m_mainApp->getGraphs().begin(); it != m_mainApp->getGraphs().end(); ++it) {
        insertRow(it.value()->id(), it.value()->name(), Graph);
    }

    QHash<QString, ModelPlugin*>::const_iterator it2;
    for (it2 = m_mainApp->getModels().begin(); it2 != m_mainApp->getModels().end(); ++it2) {
        insertRow(it2.value()->id(), it2.value()->name(), Model);
    }
}

PluginsWidget::~PluginsWidget()
{
    delete m_ui;
}

void PluginsWidget::rowSelectionChanged()
{
    int row = m_ui->table->currentRow();
    int type = m_ui->table->item(row, TYPE)->data(Qt::UserRole).toInt();
    if (type == Graph) {
        loadHtml(m_mainApp->getGraph(m_ui->table->item(row, UID)->text()));
    } else if (type == Model) {
        loadHtml(m_mainApp->getModel(m_ui->table->item(row, UID)->text()));
    } else {
        qFatal("[PluginsWidget]: invalid plugin type! It should never happen.");
    }
}

void PluginsWidget::loadHtml(const GraphPlugin* plugin)
{
    if (!plugin) {
        qFatal("[PluginsWidget]: invalid plugin object! It should never happen.");
    }

    QString html = "<h2>" + plugin->name() + "</h2><br>"
                 + "<b>Author:</b> " + plugin->author() + "<br>"
                 + "<b>Plugin Type:</b> Graph<br>"
                 + "<b>Description:</b> " + plugin->description() + "<br>";

    m_ui->browser->setHtml(html);
}

void PluginsWidget::loadHtml(const ModelPlugin* plugin)
{
    if (!plugin) {
        qFatal("[PluginsWidget]: invalid plugin object! It should never happen.");
    }

    QString html = "<h2>" + plugin->name() + "</h2><br>"
                 + "<b>Author:</b> " + plugin->author() + "<br>"
                 + "<b>Plugin Type:</b> Model<br>"
                 + "<b>Description:</b> " + plugin->description() + "<br>";

    m_ui->browser->setHtml(html);
}

void PluginsWidget::importPlugin()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import Plugin"),
                                                    QDir::homePath(),
                                                    tr("Plugin (*.so)"));
    if (fileName.isEmpty()) {
        return;
    }

    QString error;
    QString uid = m_mainApp->loadPlugin(fileName, error);

    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Error", error);
        return;
    }

    const ModelPlugin* mPlugin = m_mainApp->getModel(uid);
    if (mPlugin) {
        insertRow(uid, mPlugin->name(), Model);
    } else {
        const GraphPlugin* gPlugin = m_mainApp->getGraph(uid);
        insertRow(uid, gPlugin->name(), Graph);
    }
}

void PluginsWidget::insertRow(const QString& uid, const QString& name, PluginType type)
{
    QTableWidgetItem* typeItem;
    if (type == Model) {
        typeItem = new QTableWidgetItem("model");
    } else if (type == Graph) {
        typeItem = new QTableWidgetItem("graph");
    } else {
        qFatal("[PluginsWidget]: invalid plugin type! It should never happen.");
    }
    typeItem->setData(Qt::UserRole, type);

    int row = m_ui->table->rowCount();
    m_ui->table->insertRow(row);
    m_ui->table->setItem(row, UID, new QTableWidgetItem(uid));
    m_ui->table->setItem(row, NAME, new QTableWidgetItem(name));
    m_ui->table->setItem(row, TYPE, typeItem);
}

}
