/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QDebug>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QVariantHash>
#include <QTableWidgetItem>

#include "gui/projectwidget.h"

ProjectWidget::ProjectWidget(Project* project, ExperimentsMgr* expMgr, QWidget* parent)
    : QDockWidget(parent)
    , m_ui(new Ui_ProjectWidget)
    , m_innerWindow(new QMainWindow(this, Qt::FramelessWindowHint))
    , m_attrWidget(new AttributesWidget(project, m_innerWindow))
    , m_project(project)
{
    this->setWindowTitle(m_project->getName());  
    QHBoxLayout* lh = new QHBoxLayout(new QWidget(this));
    lh->addWidget(m_innerWindow);
    this->setWidget(lh->parentWidget());

    QWidget* projectWidget = new QWidget(m_innerWindow);
    m_ui->setupUi(projectWidget);
    m_innerWindow->setCentralWidget(projectWidget);
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
    m_ui->table->insertColumns(m_headerIdx.keys());

    connect(m_ui->table, SIGNAL(itemDoubleClicked(QTableWidgetItem*)),
            this, SLOT(onItemDoubleClicked(QTableWidgetItem*)));

    connect(expMgr, SIGNAL(statusChanged(Experiment*)),
            m_ui->table->viewport(), SLOT(update()));
    connect(expMgr, SIGNAL(progressUpdated(Experiment*)),
            m_ui->table->viewport(), SLOT(update()));
}

ProjectWidget::~ProjectWidget()
{
}

void ProjectWidget::insertRow(const int& expId)
{
    Experiment* exp = m_project->getExperiment(expId);
    const int row = m_ui->table->insertRow();

    // play/pause button
    m_ui->table->insertPlayButton(row, m_headerIdx.value(TableWidget::H_BUTTON), exp);

    // general stuff
    const Attributes* gep = exp->getGeneralAttrs();
    insertItem(row, TableWidget::H_EXPID, QString::number(exp->getId()));
    insertItem(row, TableWidget::H_SEED, gep->value(GENERAL_ATTRIBUTE_SEED).toQString());
    insertItem(row, TableWidget::H_STOPAT, gep->value(GENERAL_ATTRIBUTE_STOPAT).toQString());
    insertItem(row, TableWidget::H_TRIALS, gep->value(GENERAL_ATTRIBUTE_TRIALS).toQString());
    bool isRandom;
    gep->value(GENERAL_ATTRIBUTE_AGENTS).toQString().toInt(&isRandom);
    if (isRandom)
        insertItem(row, TableWidget::H_AGENTS, "R", "Agents with random attributes.");
    else
        insertItem(row, TableWidget::H_AGENTS, "F", "Agents from file.");

    // lambda function to add the attributes of a plugin (ie, model or graph)
    auto pluginAtbs = [this, row](TableWidget::Header header, QString pluginId, const Attributes* attrs)
    {
        QString pluginAttrs = "id:" + pluginId;
        pluginId += "_";
        for (int i = 0; i < attrs->size(); ++i) {
            pluginAttrs += QString(" | %1:%2")
                    .arg(QString(attrs->name(i)).remove(pluginId))
                    .arg(attrs->value(i).toQString());
        }
        QTableWidgetItem* item = new QTableWidgetItem(pluginAttrs);
        item->setTextAlignment(Qt::AlignCenter);
        QFont font = item->font();
        font.setItalic(true);
        item->setFont(font);
        m_ui->table->setItem(row, m_headerIdx.value(header), item);
    };

    // model stuff
    pluginAtbs(TableWidget::H_MODEL, exp->getModelId(), exp->getModelAttrs());

    // graph stuff
    pluginAtbs(TableWidget::H_GRAPH, exp->getGraphId(), exp->getGraphAttrs());
}

void ProjectWidget::insertItem(int row, TableWidget::Header header, QString label, QString tooltip)
{
    QTableWidgetItem* item = new QTableWidgetItem(label);
    item->setTextAlignment(Qt::AlignCenter);
    item->setToolTip(tooltip);
    m_ui->table->setItem(row, m_headerIdx.value(header), item);
}

void ProjectWidget::onItemDoubleClicked(QTableWidgetItem* item)
{
    int expId = m_ui->table->item(item->row(), m_headerIdx.value(TableWidget::H_EXPID))->text().toInt();
    emit (openExperiment(m_project->getId(), expId));
}
