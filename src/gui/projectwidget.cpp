/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMainWindow>
#include <QPushButton>
#include <QRect>
#include <QVariantHash>
#include <QTableWidgetItem>

#include "gui/projectwidget.h"

ProjectWidget::ProjectWidget(Project* project, QWidget* parent)
    : QDockWidget(parent)
    , m_innerWindow(new QMainWindow(this, Qt::FramelessWindowHint))
    , m_table(new TableWidget(m_innerWindow))
    , m_attrWidget(new AttributesWidget(project, m_innerWindow))
    , m_project(project)
{
    this->setWindowTitle(m_project->getName());  

    QHBoxLayout* lh = new QHBoxLayout(new QWidget(this));
    lh->addWidget(m_innerWindow);
    this->setWidget(lh->parentWidget());
    m_innerWindow->setCentralWidget(m_table);
    m_innerWindow->setStyleSheet("QMainWindow { background-color: rgb(24,24,24); }");

    m_innerWindow->addDockWidget(Qt::RightDockWidgetArea, m_attrWidget);
    m_attrWidget->setTitleBarWidget(new QWidget());
    m_attrWidget->setFeatures(QDockWidget::NoDockWidgetFeatures);
    connect(m_attrWidget, SIGNAL(experimentCreated(int)), this, SLOT(insertRow(int)));

    int col = 0;
    m_headerIdx.insert(TableWidget::H_BUTTON, col++);
    m_headerIdx.insert(TableWidget::H_EXPID, col++);
    m_headerIdx.insert(TableWidget::H_SEED, col++);
    m_headerIdx.insert(TableWidget::H_STOPAT, col++);
    m_headerIdx.insert(TableWidget::H_AGENTS, col++);
    m_headerIdx.insert(TableWidget::H_MODEL, col++);
    m_headerIdx.insert(TableWidget::H_GRAPH, col++);
    m_headerIdx.insert(TableWidget::H_TRIALS, col++);
    m_table->insertColumns(m_headerIdx.values());

    connect(m_table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QTableWidgetItem*)));
}

ProjectWidget::~ProjectWidget()
{
}

void ProjectWidget::insertRow(const int& expId)
{
    Experiment* exp = m_project->getExperiment(expId);
    const int row = m_table->rowCount();
    m_table->insertRow(row);

    // play/pause button
    m_table->insertPlayButton(row, m_headerIdx.value(TableWidget::H_BUTTON), exp);

    // general stuff
    const QVariantHash& gep = exp->getGeneralParams();
    insertItem(row, TableWidget::H_EXPID, QString::number(exp->getId()));
    insertItem(row, TableWidget::H_SEED, gep.value(GENERAL_ATTRIBUTE_SEED).toString());
    insertItem(row, TableWidget::H_STOPAT, gep.value(GENERAL_ATTRIBUTE_STOPAT).toString());
    insertItem(row, TableWidget::H_TRIALS, gep.value(GENERAL_ATTRIBUTE_TRIALS).toString());
    bool isRandom;
    gep.value(GENERAL_ATTRIBUTE_AGENTS).toInt(&isRandom);
    if (isRandom)
        insertItem(row, TableWidget::H_AGENTS, "R", "Agents with random attributes.");
    else
        insertItem(row, TableWidget::H_AGENTS, "F", "Agents from file.");

    // lambda function to add the attributes of a plugin (ie, model or graph)
    auto pluginAtbs = [this, row](TableWidget::Header header, QString pluginId, const QVariantHash& params)
    {
        QString pluginAttrs = "id:" + pluginId;
        pluginId += "_";
        QVariantHash::const_iterator it = params.begin();
        while (it != params.end()) {
            pluginAttrs += QString(" | %1:%2").arg(QString(it.key()).remove(pluginId)).arg(it.value().toString());
            ++it;
        }
        QTableWidgetItem* item = new QTableWidgetItem(pluginAttrs);
        item->setTextAlignment(Qt::AlignCenter);
        QFont font = item->font();
        font.setItalic(true);
        item->setFont(font);
        int col = m_headerIdx.value(header);
        m_table->setItem(row, col, item);
        m_table->stretchColumn(col);
    };

    // model stuff
    pluginAtbs(TableWidget::H_MODEL, gep.value(GENERAL_ATTRIBUTE_MODELID).toString(), exp->getModelParams());

    // graph stuff
    pluginAtbs(TableWidget::H_GRAPH, gep.value(GENERAL_ATTRIBUTE_GRAPHID).toString(), exp->getGraphParams());
}

void ProjectWidget::insertItem(int row, TableWidget::Header header, QString label, QString tooltip)
{
    QTableWidgetItem* item = new QTableWidgetItem(label);
    item->setTextAlignment(Qt::AlignCenter);
    item->setToolTip(tooltip);
    m_table->setItem(row, m_headerIdx.value(header), item);
}

void ProjectWidget::onItemDoubleClicked(QTableWidgetItem* item)
{
    int expId = m_table->data(item->row(), m_headerIdx.value(TableWidget::H_EXPID)).toInt();
    emit (openExperiment(m_project->getId(), expId));
}
