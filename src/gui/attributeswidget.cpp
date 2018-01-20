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
#include <QRadioButton>

#include "attributeswidget.h"
#include "outputwidget.h"
#include "ui_attributeswidget.h"

#define STRING_NULL_PLUGINID "--"

namespace evoplex {

AttributesWidget::AttributesWidget(MainApp* mainApp, Project* project, QWidget *parent)
    : QDockWidget(parent)
    , m_mainApp(mainApp)
    , m_project(project)
    , m_exp(nullptr)
    , m_selectedGraphId(STRING_NULL_PLUGINID)
    , m_selectedModelId(STRING_NULL_PLUGINID)
    , m_ui(new Ui_AttributesWidget)
{
    m_ui->setupUi(this);

    m_ui->treeWidget->setFocusPolicy(Qt::NoFocus);
    m_ui->bEdit->hide();
    connect(m_ui->bSubmit, SIGNAL(clicked(bool)), SLOT(slotCreateExperiment()));
    connect(m_ui->bEdit, SIGNAL(clicked(bool)), SLOT(slotEditExperiment()));

    // setup the tree widget: general attributes
    m_treeItemGeneral = new QTreeWidgetItem(m_ui->treeWidget);
    m_treeItemGeneral->setText(0, "General");
    m_treeItemGeneral->setExpanded(true);
    // -- add custom widget -- agents from file
    QLineEdit* agentsPath = new QLineEdit(m_project->getDest());
    QPushButton* btBrowseFile = new QPushButton("...");
    btBrowseFile->setMaximumWidth(20);
    connect(btBrowseFile, SIGNAL(clicked(bool)), SLOT(slotAgentFile()));
    QHBoxLayout* agentsLayout = new QHBoxLayout(new QWidget(m_ui->treeWidget));
    agentsLayout->setMargin(0);
    agentsLayout->insertWidget(0, agentsPath);
    agentsLayout->insertWidget(1, btBrowseFile);
    m_widgetFields.insert(GENERAL_ATTRIBUTE_AGENTS, QVariant::fromValue(agentsPath));
    QTreeWidgetItem* item = new QTreeWidgetItem(m_treeItemGeneral);
    item->setText(0, GENERAL_ATTRIBUTE_AGENTS);
    m_ui->treeWidget->setItemWidget(item, 1, agentsLayout->parentWidget());
    // -- seed
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_SEED, QVariant::fromValue(newSpinBox(0, INT32_MAX)));
    // --  stop at
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_STOPAT, QVariant::fromValue(newSpinBox(1, EVOPLEX_MAX_STEPS)));
    // --  trials
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_TRIALS, QVariant::fromValue(newSpinBox(1, EVOPLEX_MAX_STEPS)));
    // --  auto delete
    QCheckBox* chb = new QCheckBox(m_ui->treeWidget);
    chb->setChecked(true);
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_AUTODELETE, QVariant::fromValue(chb));
    // --  models available
    QComboBox* cb = new QComboBox(m_ui->treeWidget);
    connect(cb, SIGNAL(currentIndexChanged(QString)), SLOT(slotModelSelected(QString)));
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_MODELID, QVariant::fromValue(cb));
    // --  graphs available
    cb = new QComboBox(m_ui->treeWidget);
    cb->setEnabled(false);
    connect(cb, SIGNAL(currentIndexChanged(QString)), SLOT(slotGraphSelected(QString)));
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_GRAPHID, QVariant::fromValue(cb));

    // setup the tree widget: outputs
    m_treeItemOutputs = new QTreeWidgetItem(m_ui->treeWidget);
    m_treeItemOutputs->setText(0, "File Outputs");
    m_treeItemOutputs->setExpanded(true);
    // -- enabled
    m_enableOutputs = new QCheckBox("save to file");
    QTreeWidgetItem* itemEnabled = new QTreeWidgetItem(m_treeItemOutputs);
    itemEnabled->setText(0, "enable");
    m_ui->treeWidget->setItemWidget(itemEnabled, 1, m_enableOutputs);
    // -- add custom widget: output directory
    QLineEdit* outDir = new QLineEdit(m_project->getDest());
    QPushButton* outBrowseDir = new QPushButton("...");
    outBrowseDir->setMaximumWidth(20);
    connect(outBrowseDir, SIGNAL(clicked(bool)), SLOT(slotOutputDir()));
    QHBoxLayout* outLayout = new QHBoxLayout(new QWidget(m_ui->treeWidget));
    outLayout->setMargin(0);
    outLayout->insertWidget(0, outDir);
    outLayout->insertWidget(1, outBrowseDir);
    m_widgetFields.insert(OUTPUT_DIR, QVariant::fromValue(outDir));
    QTreeWidgetItem* itemDir = new QTreeWidgetItem(m_treeItemOutputs);
    itemDir->setText(0, OUTPUT_DIR);
    m_ui->treeWidget->setItemWidget(itemDir, 1, outLayout->parentWidget());
    // -- add custom widget: output directory
    QLineEdit* outHeader = new QLineEdit(m_project->getDest());
    QPushButton* outBuildHeader = new QPushButton("...");
    outBuildHeader->setMaximumWidth(20);
    connect(outBuildHeader, SIGNAL(clicked(bool)), SLOT(slotOutputWidget()));
    QHBoxLayout* headerLayout = new QHBoxLayout(new QWidget(m_ui->treeWidget));
    headerLayout->setMargin(0);
    headerLayout->insertWidget(0, outHeader);
    headerLayout->insertWidget(1, outBuildHeader);
    m_widgetFields.insert(OUTPUT_HEADER, QVariant::fromValue(outHeader));
    QTreeWidgetItem* itemHeader = new QTreeWidgetItem(m_treeItemOutputs);
    itemHeader->setText(0, OUTPUT_HEADER);
    m_ui->treeWidget->setItemWidget(itemHeader, 1, headerLayout->parentWidget());

/* TODO: make the buttons to avgTrials and saveSteps work*/
    // -- avgTrials
    QCheckBox* outAvgTrials = new QCheckBox("average trials");
    addTreeWidget(m_treeItemOutputs, OUTPUT_AVGTRIALS, QVariant::fromValue(outAvgTrials));
/*    // -- steps to save
    QRadioButton* outAllSteps = new QRadioButton("all");
    outAllSteps->setChecked(true);
    QRadioButton* outLastSteps = new QRadioButton("last");
    QSpinBox* outNumSteps = new QSpinBox();
    outNumSteps->setButtonSymbols(QSpinBox::NoButtons);
    outNumSteps->setMinimum(0);
    outNumSteps->setMaximum(EVOPLEX_MAX_STEPS);
    QHBoxLayout* outStepsLayout = new QHBoxLayout(new QWidget(m_ui->treeWidget));
    outStepsLayout->setMargin(0);
    outStepsLayout->insertWidget(0, outAllSteps);
    outStepsLayout->insertWidget(1, outLastSteps);
    outStepsLayout->insertWidget(2, outNumSteps);
    outStepsLayout->insertSpacerItem(3, new QSpacerItem(5,5,QSizePolicy::MinimumExpanding));
    QTreeWidgetItem* itemOut = new QTreeWidgetItem(m_treeItemOutputs);
    itemOut->setText(0, OUTPUT_SAVESTEPS);
    m_ui->treeWidget->setItemWidget(itemOut, 1, outStepsLayout->parentWidget());
*/
    connect(m_enableOutputs, &QCheckBox::clicked,
            [outDir, outBrowseDir, outHeader, outBuildHeader](bool b) {
                outDir->setEnabled(b);
                outBrowseDir->setEnabled(b);
                outHeader->setEnabled(b);
                outBuildHeader->setEnabled(b);
//                outAvgTrials->setEnabled(b);
    });
    m_enableOutputs->setChecked(false);

    // create the trees with the plugin stuff
    slotUpdateModelPlugins();
    slotUpdateGraphPlugins();
}

AttributesWidget::~AttributesWidget()
{
    delete m_ui;
}

void AttributesWidget::setExperiment(Experiment* exp)
{
    if (!exp) {
        m_exp = nullptr;
        m_ui->bEdit->hide();
        return;
    }

    m_exp = exp;
    m_ui->bEdit->show();

    std::vector<QString> header = exp->generalAttrs()->names();
    foreach (QString attrName, exp->graphAttrs()->names())
        header.emplace_back(exp->graphId() + "_" + attrName);
    foreach (QString attrName, exp->modelAttrs()->names())
        header.emplace_back(exp->modelId() + "_" + attrName);

    std::vector<Value> values = exp->generalAttrs()->values();
    values.insert(values.end(), exp->graphAttrs()->values().begin(), exp->graphAttrs()->values().end());
    values.insert(values.end(), exp->modelAttrs()->values().begin(), exp->modelAttrs()->values().end());

    // ensure graphId will be filled at the end
    header.emplace_back(GENERAL_ATTRIBUTE_GRAPHID);
    values.emplace_back(Value(exp->graphId()));

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

void AttributesWidget::slotOutputDir()
{
    QLineEdit* lineedit = m_widgetFields.value(OUTPUT_DIR).value<QLineEdit*>();
    QString path = QFileDialog::getExistingDirectory(this, "Output Directory", lineedit->text());
    if (!path.isEmpty()) {
        lineedit->setText(path);
    }
}

void AttributesWidget::slotOutputWidget()
{
    if (m_selectedModelId == STRING_NULL_PLUGINID) {
        QMessageBox::warning(this, "Experiment", "Please, select a valid 'modelId' first.");
        return;
    }

    int numTrials = m_widgetFields.value(GENERAL_ATTRIBUTE_TRIALS).value<QSpinBox*>()->value();
    std::vector<int> trialIds;
    trialIds.reserve(numTrials);
    for (int id = 0; id < numTrials; ++id) {
        trialIds.emplace_back(id);
    }

    std::vector<Output*> currentOutputs;
    const ModelPlugin* model = m_project->getModels().value(m_selectedModelId);
    QString currentHeader = m_widgetFields.value(OUTPUT_HEADER).value<QLineEdit*>()->text();
    if (!currentHeader.isEmpty()) {
        QString errorMsg;
        currentOutputs = Output::parseHeader(currentHeader.split(";"), trialIds, model, errorMsg);
        if (!errorMsg.isEmpty()) {
            QMessageBox::warning(this, "Output Creator", errorMsg);
            qDeleteAll(currentOutputs);
            currentOutputs.clear();
        }
    }

    OutputWidget* ow = new OutputWidget(model);
    ow->setAttribute(Qt::WA_DeleteOnClose, true);
    ow->setWindowModality(Qt::ApplicationModal);
    ow->setTrialIds(trialIds);
    ow->fill(currentOutputs);
    ow->show();

    connect(ow, &OutputWidget::closed,
    [this](std::vector<Output*> outputs) {
        // join all Output objects which have the same function, entity and attribute
        QStringList uniqueOutputs;
        for (int o1 = 0; o1 < outputs.size(); ++o1) {
            if (!outputs.at(o1)) {
                continue;
            }
            QString outputStr = outputs.at(o1)->printableHeader(';');
            for (int o2 = 0; o2 < outputs.size(); ++o2) {
                if (o1 == o2 || !outputs.at(o2) || !outputs.at(o1)->operator ==(outputs.at(o2))) {
                    continue;
                }
                outputStr += "_" + outputs.at(o2)->allInputs().front().toQString();
                delete outputs.at(o2);
                outputs.at(o2) = nullptr;
            }
            uniqueOutputs.push_back(outputStr);
        }

        m_widgetFields.value(OUTPUT_HEADER)
                .value<QLineEdit*>()->setText(uniqueOutputs.join(';'));
    });
}

Experiment::ExperimentInputs* AttributesWidget::readInputs()
{
    if (m_selectedModelId == STRING_NULL_PLUGINID) {
        QMessageBox::warning(this, "Experiment", "Please, select a valid 'modelId'.");
        return nullptr;
    } else if (m_selectedGraphId == STRING_NULL_PLUGINID) {
        QMessageBox::warning(this, "Experiment", "Please, select a valid 'graphId'.");
        return nullptr;
    } else if (m_enableOutputs->isChecked()
               && (m_widgetFields.value(OUTPUT_DIR).value<QLineEdit*>()->text().isEmpty()
                   || m_widgetFields.value(OUTPUT_HEADER).value<QLineEdit*>()->text().isEmpty())) {
        QMessageBox::warning(this, "Experiment", "Please, insert a valid output directory and a output header.");
        return nullptr;
    }

    QStringList header;
    QStringList values;
    QVariantHash::iterator it;
    for (it = m_widgetFields.begin(); it != m_widgetFields.end(); ++it) {
        header << it.key();

        if (!m_enableOutputs->isChecked()
                && (it.key() == OUTPUT_DIR || it.key() == OUTPUT_HEADER)) {
            values << "";
            continue;
        }

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
    Experiment::ExperimentInputs* inputs = Experiment::readInputs(m_mainApp, header, values, errorMsg);
    if (!inputs) {
        QMessageBox::warning(this, "Experiment",
                "Unable to create the experiment.\nError: \"" + errorMsg + "\"");
    }
    return inputs;
}

void AttributesWidget::slotCreateExperiment()
{
    m_exp = m_project->newExperiment(readInputs());
}

void AttributesWidget::slotEditExperiment()
{
    Q_ASSERT(m_exp);
    if (!m_project->editExperiment(m_exp->id(), readInputs())) {
        QMessageBox::warning(this, "Experiment",
                "Unable to edit the experiment.\n"
                "If it is running, you should pause it first.");
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
    foreach (GraphPlugin* graph, m_project->getGraphs()) {
        if (graph->graphAttrSpace().size() > 0 && m_treeItemGraphs.contains(graph->id())) {
            continue;
        }

        // the root node
        QTreeWidgetItem* itemRoot = new QTreeWidgetItem(m_ui->treeWidget);
        itemRoot->setText(0, "Graph");
        itemRoot->setHidden(true);
        m_treeItemGraphs.insert(graph->id(), itemRoot);

        // the graph stuff
        insertPluginAttributes(itemRoot, graph->id(), graph->graphAttrSpace());
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
    foreach (ModelPlugin* model, m_project->getModels()) {
        if (m_treeItemModels.contains(model->id())) {
            continue;
        }

        // the root node
        QTreeWidgetItem* itemRoot = new QTreeWidgetItem(m_ui->treeWidget);
        itemRoot->setText(0, "Model");
        itemRoot->setHidden(true);
        m_treeItemModels.insert(model->id(), itemRoot);

        // the model stuff
        insertPluginAttributes(itemRoot, model->id(), model->modelAttrSpace());
    }
}

void AttributesWidget::insertPluginAttributes(QTreeWidgetItem* itemRoot,
                                              const QString& uid,
                                              const AttributesSpace& attrsSpace)
{
    const QString& uid_ = uid + "_";
    AttributesSpace::const_iterator it = attrsSpace.constBegin();
    for (it; it != attrsSpace.constEnd(); ++it) {
        const ValueSpace* valSpace = it.value();
        QTreeWidgetItem* item = new QTreeWidgetItem(itemRoot);
        item->setText(0, valSpace->attrName());

        QWidget* widget = nullptr;
        if (valSpace->type() == ValueSpace::Double_Interval) {
            const IntervalSpace* iSpace = dynamic_cast<const IntervalSpace*>(valSpace);
            widget = newDoubleSpinBox(iSpace->min().toDouble, iSpace->max().toDouble);
        } else if (valSpace->type() == ValueSpace::Int_Interval) {
            const IntervalSpace* iSpace = dynamic_cast<const IntervalSpace*>(valSpace);
            widget = newSpinBox(iSpace->min().toInt, iSpace->max().toInt);
        } else {
            QLineEdit* le = new QLineEdit();
            le->setText(valSpace->validValue().toQString());
            widget = le;
        }
        m_ui->treeWidget->setItemWidget(item, 1, widget);
        // add the uid as prefix to avoid clashes.
        m_widgetFields.insert(uid_ + valSpace->attrName(), QVariant::fromValue(widget));
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

QDoubleSpinBox* AttributesWidget::newDoubleSpinBox(const double min, const double max)
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
