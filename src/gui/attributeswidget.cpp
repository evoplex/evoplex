/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVariant>

#include "attributeswidget.h"
#include "ui_attributeswidget.h"

#define STRING_NULL_PLUGINID "--"

namespace evoplex {

AttributesWidget::AttributesWidget(Project* project, QWidget *parent)
    : QDockWidget(parent)
    , m_project(project)
    , m_selectedGraphId(STRING_NULL_PLUGINID)
    , m_selectedModelId(STRING_NULL_PLUGINID)
    , m_ui(new Ui_AttributesWidget)
{
    m_ui->setupUi(this);

    connect(m_ui->btnSubmit, SIGNAL(clicked(bool)), this, SLOT(slotCreateExperiment()));
    m_ui->treeWidget->setFocusPolicy(Qt::NoFocus);

    //
    // setup the tree widget: general attributes
    //
    m_treeItemGeneral = new QTreeWidgetItem(m_ui->treeWidget);
    m_treeItemGeneral->setText(0, "General");
    m_treeItemGeneral->setExpanded(true);

    // add custom widget -- agents from file
    QLineEdit* agentsPath = new QLineEdit(m_project->getDest());
    QPushButton* btBrowseFile = new QPushButton("...");
    btBrowseFile->setMaximumWidth(20);
    connect(btBrowseFile, SIGNAL(clicked(bool)), this, SLOT(slotAgentFile()));
    QHBoxLayout* agentsLayout = new QHBoxLayout(new QWidget(m_ui->treeWidget));
    agentsLayout->setMargin(0);
    agentsLayout->insertWidget(0, agentsPath);
    agentsLayout->insertWidget(1, btBrowseFile);
    m_widgetFields.insert(GENERAL_ATTRIBUTE_AGENTS, QVariant::fromValue(agentsPath));
    QTreeWidgetItem* item = new QTreeWidgetItem(m_treeItemGeneral);
    item->setText(0, GENERAL_ATTRIBUTE_AGENTS);
    m_ui->treeWidget->setItemWidget(item, 1, agentsLayout->parentWidget());
    // seed
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_SEED, QVariant::fromValue(newSpinBox(0, INT32_MAX)));
    // stop at
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_STOPAT, QVariant::fromValue(newSpinBox(1, EVOPLEX_MAX_STEPS)));
    // trials
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_TRIALS, QVariant::fromValue(newSpinBox(1, EVOPLEX_MAX_STEPS)));
    // models available
    QComboBox* cb = new QComboBox(m_ui->treeWidget);
    connect(cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotModelSelected(QString)));
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_MODELID, QVariant::fromValue(cb));
    // graphs available
    cb = new QComboBox(m_ui->treeWidget);
    cb->setEnabled(false);
    connect(cb, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotGraphSelected(QString)));
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_GRAPHID, QVariant::fromValue(cb));
    // auto delete
    QCheckBox* chb = new QCheckBox(m_ui->treeWidget);
    chb->setChecked(true);
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_AUTODELETE, QVariant::fromValue(chb));

    // create the trees with the plugin stuff
    slotUpdateModelPlugins();
    slotUpdateGraphPlugins();
}

AttributesWidget::~AttributesWidget()
{
    delete m_ui;
}

void AttributesWidget::fill(Experiment* exp)
{
    std::vector<QString> header = exp->getGeneralAttrs()->names();
    foreach (QString attrName, exp->getGraphAttrs()->names())
        header.push_back(exp->getGraphId() + "_" + attrName);
    foreach (QString attrName, exp->getModelAttrs()->names())
        header.push_back(exp->getModelId() + "_" + attrName);

    std::vector<Value> values = exp->getGeneralAttrs()->values();
    values.insert(values.end(), exp->getGraphAttrs()->values().begin(), exp->getGraphAttrs()->values().end());
    values.insert(values.end(), exp->getModelAttrs()->values().begin(), exp->getModelAttrs()->values().end());

    // ensure graphId will be filled at the end
    header.push_back(GENERAL_ATTRIBUTE_GRAPHID);
    values.push_back(Value(exp->getGraphId()));

    header.shrink_to_fit();
    values.shrink_to_fit();

    for (int i = 0; i < header.size(); ++i) {
        QWidget* widget = m_widgetFields.value(header.at(i)).value<QWidget*>();
        Q_ASSERT(widget);

        QSpinBox* sp = qobject_cast<QSpinBox*>(widget);
        if (sp) {
            sp->setValue(values.at(i).toInt);
            continue;
        }
        QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(widget);
        if (dsp) {
            dsp->setValue(values.at(i).toDouble);
            continue;
        }
        QComboBox* cb = qobject_cast<QComboBox*>(widget);
        if (cb) {
            cb->setCurrentIndex(cb->findText(values.at(i).toQString()));
            continue;
        }
        QCheckBox* chb = qobject_cast<QCheckBox*>(widget);
        if (chb) {
            chb->setChecked(values.at(i).toBool);
            continue;
        }
        QLineEdit* le = qobject_cast<QLineEdit*>(widget);
        if (le) {
            le->setText(values.at(i).toQString());
            continue;
        }
        qFatal("[AttributesWidget]: unable to know the widget type.");
    }
}

void AttributesWidget::slotAgentFile()
{
    QLineEdit* lineedit = m_widgetFields.value(GENERAL_ATTRIBUTE_AGENTS).value<QLineEdit*>();
    QString path = QFileDialog::getOpenFileName(this, "Initial Population", lineedit->text(), "Text Files (*.csv *.txt)");
    if (!path.isEmpty()) {
        lineedit->setText(path);
    }
}

void AttributesWidget::slotCreateExperiment()
{
    if (m_selectedModelId == STRING_NULL_PLUGINID) {
        QMessageBox::warning(this, "Experiment", "Please, select a valid 'modelId'.");
        return;
    } else if (m_selectedGraphId == STRING_NULL_PLUGINID) {
        QMessageBox::warning(this, "Experiment", "Please, select a valid 'graphId'.");
        return;
    }

    QStringList header;
    QStringList values;
    QVariantHash::iterator it;
    for (it = m_widgetFields.begin(); it != m_widgetFields.end(); ++it) {
        header << it.key();
        QWidget* widget = it.value().value<QWidget*>();
        Q_ASSERT(widget);

        QSpinBox* sp = qobject_cast<QSpinBox*>(widget);
        if (sp) {
            values << QString::number(sp->value());
            continue;
        }
        QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(widget);
        if (dsp) {
            values << QString::number(dsp->value(), 'f', 2);
            continue;
        }
        QComboBox* cb = qobject_cast<QComboBox*>(widget);
        if (cb) {
            values << cb->currentText();
            continue;
        }
        QCheckBox* chb = qobject_cast<QCheckBox*>(widget);
        if (chb) {
            values << QString::number(chb->isChecked());
            continue;
        }
        QLineEdit* le = qobject_cast<QLineEdit*>(widget);
        if (le) {
            values << le->text();
            continue;
        }

        qFatal("[AttributesWidget]: unable to know the widget type.");
    }

    QString errorMsg;
    int expId = m_project->newExperiment(header, values, errorMsg);
    if (expId >= 0) {
        emit (experimentCreated(expId));
    } else {
        QMessageBox::warning(this, "Experiment",
                "Unable to create the experiment.\nError: \"" + errorMsg + "\"");
    }
}

void AttributesWidget::slotGraphSelected(const QString& graphId)
{
    // hide last and show the selected graph tree
    QTreeWidgetItem* item = m_treeItemGraphs.value(m_selectedGraphId);
    if (item) { item->setHidden(true); }
    item = m_treeItemGraphs.value(graphId);
    if (item) {
        item->setHidden(false);
        item->setExpanded(true);
    }
    m_selectedGraphId = graphId;
}

void AttributesWidget::slotModelSelected(const QString& modelId)
{
    QTreeWidgetItem* itemModel = m_treeItemModels.value(m_selectedModelId);
    if (itemModel) itemModel->setHidden(true);

    QComboBox* cb = m_widgetFields.value(GENERAL_ATTRIBUTE_GRAPHID).value<QComboBox*>();
    cb->setCurrentIndex(0); // reset graphId
    cb->setEnabled(modelId != STRING_NULL_PLUGINID);

    itemModel = m_treeItemModels.value(modelId);
    if (itemModel) {
        itemModel->setHidden(false);
        itemModel->setExpanded(true);
    }
    m_selectedModelId = modelId;
}

void AttributesWidget::slotUpdateGraphPlugins()
{
    // list all graphs
    QComboBox* cb = m_widgetFields.value(GENERAL_ATTRIBUTE_GRAPHID).value<QComboBox*>();
    cb->blockSignals(true);
    cb->clear();
    cb->insertItem(0, STRING_NULL_PLUGINID);
    cb->insertItems(1, m_project->getGraphs().keys());
    cb->blockSignals(false);

    // create the trees to hold the graphs' attributes
    foreach (MainApp::GraphPlugin* graph, m_project->getGraphs()) {
        if (graph->graphAttrMin.size() > 0 && m_treeItemGraphs.contains(graph->uid)) {
            continue;
        }

        // the root node
        QTreeWidgetItem* itemRoot = new QTreeWidgetItem(m_ui->treeWidget);
        itemRoot->setText(0, "Graph");
        itemRoot->setHidden(true);
        m_treeItemGraphs.insert(graph->uid, itemRoot);

        // the graph stuff
        insertPluginAttributes(itemRoot, graph->uid, graph->graphAttrMin, graph->graphAttrMax);
    }
}

void AttributesWidget::slotUpdateModelPlugins()
{
    // list all models
    QComboBox* cb = m_widgetFields.value(GENERAL_ATTRIBUTE_MODELID).value<QComboBox*>();
    cb->blockSignals(true);
    cb->clear();
    cb->insertItem(0, STRING_NULL_PLUGINID);
    cb->insertItems(1, m_project->getModels().keys());
    cb->blockSignals(false);

    // create the trees to hold the models' attributes
    foreach (MainApp::ModelPlugin* model, m_project->getModels()) {
        if (m_treeItemModels.contains(model->uid)) {
            continue;
        }

        // the root node
        QTreeWidgetItem* itemRoot = new QTreeWidgetItem(m_ui->treeWidget);
        itemRoot->setText(0, "Model");
        itemRoot->setHidden(true);
        m_treeItemModels.insert(model->uid, itemRoot);

        // the model stuff
        insertPluginAttributes(itemRoot, model->uid, model->modelAttrMin, model->modelAttrMax);
    }
}

void AttributesWidget::insertPluginAttributes(QTreeWidgetItem* itemRoot, const QString& uid,
                                              const Attributes& min, const Attributes& max)
{
    const QString& uid_ = uid + "_";
    for (int i = 0; i < min.size(); ++i) {
        QTreeWidgetItem* item = new QTreeWidgetItem(itemRoot);
        item->setText(0, min.name(i));

        QWidget* widget = nullptr;
        if (min.value(i).type == Value::DOUBLE) {
            widget = newDoubleSpinBox(min.value(i).toDouble, max.value(i).toDouble);
        } else if (min.value(i).type == Value::INT) {
            widget = newSpinBox(min.value(i).toInt, max.value(i).toInt);
        } else {
            QLineEdit* le = new QLineEdit();
            le->setText(min.value(i).toQString());
            widget = le;
        }
        m_ui->treeWidget->setItemWidget(item, 1, widget);
        // add the uid as prefix to avoid clashes.
        m_widgetFields.insert(uid_ + min.name(i), QVariant::fromValue(widget));
    }
}

QSpinBox* AttributesWidget::newSpinBox(const int min, const int max)
{
    QSpinBox* sp = new QSpinBox(m_ui->treeWidget);
    sp->setMaximum(max);
    sp->setMinimum(min);
    sp->setButtonSymbols(QSpinBox::NoButtons);
    return sp;
}

QDoubleSpinBox* AttributesWidget::newDoubleSpinBox(const int min, const int max)
{
    QDoubleSpinBox* sp = new QDoubleSpinBox(m_ui->treeWidget);
    sp->setMaximum(max);
    sp->setMinimum(min);
    sp->setButtonSymbols(QDoubleSpinBox::NoButtons);
    return sp;
}

void AttributesWidget::addTreeWidget(QTreeWidgetItem* itemRoot, const QString& label, const QVariant& widget)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(itemRoot);
    m_widgetFields.insert(label, widget);
    item->setText(0, label);
    m_ui->treeWidget->setItemWidget(item, 1, widget.value<QWidget*>());
}

}
