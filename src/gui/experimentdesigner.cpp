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

#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVariant>
#include <QRadioButton>

#include "experimentdesigner.h"
#include "nodesgeneratordlg.h"
#include "experimentwidget.h"
#include "projectwidget.h"
#include "outputwidget.h"
#include "ui_experimentdesigner.h"

#include "core/attrsgenerator.h"
#include "core/include/enum.h"

#define STRING_NULL_PLUGINID "--"

namespace evoplex {

ExperimentDesigner::ExperimentDesigner(MainApp* mainApp, QWidget *parent)
    : QDockWidget(parent)
    , m_mainApp(mainApp)
    , m_project(nullptr)
    , m_exp(nullptr)
    , m_selectedGraphId(STRING_NULL_PLUGINID)
    , m_selectedModelId(STRING_NULL_PLUGINID)
    , m_ui(new Ui_ExperimentDesigner)
{
    setObjectName("ExperimentDesigner");
    m_ui->setupUi(this);

    m_ui->treeWidget->setFocusPolicy(Qt::NoFocus);
    m_ui->bEdit->hide();
    connect(m_ui->bSubmit, SIGNAL(clicked(bool)), SLOT(slotCreateExperiment()));
    connect(m_ui->bEdit, SIGNAL(clicked(bool)), SLOT(slotEditExperiment()));
    connect(m_ui->cbWidgets, SIGNAL(currentIndexChanged(int)), SLOT(slotSetActiveWidget(int)));

    // setup the tree widget: model
    m_treeItemModels = new QTreeWidgetItem(m_ui->treeWidget);
    m_treeItemModels->setText(0, "Model");
    m_treeItemModels->setExpanded(true);
    // --  models available
    QComboBox* cb = new QComboBox(m_ui->treeWidget);
    cb->insertItem(0, STRING_NULL_PLUGINID);
    connect(cb, SIGNAL(currentIndexChanged(QString)), SLOT(slotModelSelected(QString)));
    addTreeWidget(m_treeItemModels, GENERAL_ATTRIBUTE_MODELID, QVariant::fromValue(cb));

    // setup the tree widget: graph
    m_treeItemGraphs = new QTreeWidgetItem(m_ui->treeWidget);
    m_treeItemGraphs->setText(0, "Graph");
    m_treeItemGraphs->setExpanded(false);
    // -- add custom widget -- nodes creator
    QLineEdit* nodesCmd = new QLineEdit();
    QPushButton* btNodeW = new QPushButton("...");
    btNodeW->setMaximumWidth(20);
    connect(btNodeW, SIGNAL(clicked(bool)), SLOT(slotNodesWidget()));
    QHBoxLayout* nodesLayout = new QHBoxLayout(new QWidget(m_ui->treeWidget));
    nodesLayout->setMargin(0);
    nodesLayout->insertWidget(0, nodesCmd);
    nodesLayout->insertWidget(1, btNodeW);
    m_widgetFields.insert(GENERAL_ATTRIBUTE_NODES, QVariant::fromValue(nodesCmd));
    QTreeWidgetItem* item = new QTreeWidgetItem(m_treeItemGraphs);
    item->setText(0, GENERAL_ATTRIBUTE_NODES);
    m_ui->treeWidget->setItemWidget(item, 1, nodesLayout->parentWidget());
    // --  graphs available
    cb = new QComboBox(m_ui->treeWidget);
    cb->insertItem(0, STRING_NULL_PLUGINID);
    connect(cb, SIGNAL(currentIndexChanged(QString)), SLOT(slotGraphSelected(QString)));
    addTreeWidget(m_treeItemGraphs, GENERAL_ATTRIBUTE_GRAPHID, QVariant::fromValue(cb));
    // --  graph type
    cb = new QComboBox(m_ui->treeWidget);
    cb->insertItem(0, "undirected", static_cast<int>(GraphType::Undirected));
    cb->insertItem(1, "directed", static_cast<int>(GraphType::Directed));
    m_customGraphIdx = m_treeItemGraphs->childCount();
    addTreeWidget(m_treeItemGraphs, GENERAL_ATTRIBUTE_GRAPHTYPE, QVariant::fromValue(cb));

    // setup the tree widget: general attributes
    m_treeItemGeneral = new QTreeWidgetItem(m_ui->treeWidget);
    m_treeItemGeneral->setText(0, "Simulation");
    m_treeItemGeneral->setExpanded(false);
    // -- seed
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_SEED, QVariant::fromValue(newSpinBox(0, INT32_MAX)));
    // --  stop at
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_STOPAT, QVariant::fromValue(newSpinBox(1, EVOPLEX_MAX_STEPS)));
    // --  trials
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_TRIALS, QVariant::fromValue(newSpinBox(1, EVOPLEX_MAX_TRIALS)));
    // --  auto delete
    QCheckBox* chb = new QCheckBox(m_ui->treeWidget);
    chb->setChecked(false);
    addTreeWidget(m_treeItemGeneral, GENERAL_ATTRIBUTE_AUTODELETE, QVariant::fromValue(chb));

    // setup the tree widget: outputs
    m_treeItemOutputs = new QTreeWidgetItem(m_ui->treeWidget);
    m_treeItemOutputs->setText(0, "File Outputs");
    m_treeItemOutputs->setExpanded(false);
    // -- enabled
    m_enableOutputs = new QCheckBox("save to file");
    QTreeWidgetItem* itemEnabled = new QTreeWidgetItem(m_treeItemOutputs);
    itemEnabled->setText(0, "enable");
    m_ui->treeWidget->setItemWidget(itemEnabled, 1, m_enableOutputs);
    // -- add custom widget: output directory
    QLineEdit* outDir = new QLineEdit();
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
    QLineEdit* outHeader = new QLineEdit();
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
    connect(m_enableOutputs, &QCheckBox::toggled,
            [outDir, outBrowseDir, outHeader, outBuildHeader](bool b) {
                outDir->setEnabled(b);
                outBrowseDir->setEnabled(b);
                outHeader->setEnabled(b);
                outBuildHeader->setEnabled(b);
//                outAvgTrials->setEnabled(b);
    });
    m_enableOutputs->setChecked(true);
    m_enableOutputs->setChecked(false);

    for (const GraphPlugin* g : m_mainApp->graphs()) { slotPluginAdded(g); }
    for (const ModelPlugin* m : m_mainApp->models()) { slotPluginAdded(m); }
    connect(m_mainApp, SIGNAL(pluginAdded(const Plugin*)), SLOT(slotPluginAdded(const Plugin*)));
    connect(m_mainApp, SIGNAL(pluginRemoved(QString,Plugin::Type)),
            SLOT(slotPluginRemoved(QString,Plugin::Type)));
}

ExperimentDesigner::~ExperimentDesigner()
{
    delete m_ui;
}

void ExperimentDesigner::addWidgetToList(PPageDockWidget* dw)
{
    m_ui->cbWidgets->addItem(dw->objectName(), QVariant::fromValue(dw));
}

void ExperimentDesigner::removeWidgetFromList(PPageDockWidget *dw)
{
    int id = m_ui->cbWidgets->findData(QVariant::fromValue(dw));
    if (id != -1) {
        m_ui->cbWidgets->removeItem(id);
    }
}

void ExperimentDesigner::setActiveWidget(PPageDockWidget* dw)
{
    int id = m_ui->cbWidgets->findData(QVariant::fromValue(dw));
    if (id == -1 || !dw->project()) {
        qFatal("project is null or qdockwidget is not in the list!");
    }
    m_ui->cbWidgets->setCurrentIndex(id);
    m_project = dw->project();
    ExperimentWidget* ew = qobject_cast<ExperimentWidget*>(dw);
    m_ui->bSubmit->setVisible(!ew);
    setExperiment(ew ? ew->exp() : nullptr);
}

void ExperimentDesigner::slotSetActiveWidget(int idx)
{
    QVariant v = m_ui->cbWidgets->itemData(idx);
    if (v.isValid()) {
        PPageDockWidget* dw = v.value<PPageDockWidget*>();
        dw->show();
        dw->raise();
    } else {
        m_project.clear();
    }
}

void ExperimentDesigner::setExperiment(Experiment* exp)
{
    if (!exp || exp->expStatus() == Status::Invalid) {
        m_exp = nullptr;
        m_ui->bEdit->hide();
        return;
    }

    m_exp = exp;
    m_ui->bEdit->show();
    m_enableOutputs->setChecked(exp->hasOutputs());

    std::vector<QString> header = exp->inputs()->exportAttrNames();
    std::vector<Value> values = exp->inputs()->exportAttrValues();

    // ensure graphId will be filled at the end
    header.emplace_back(GENERAL_ATTRIBUTE_GRAPHID);
    values.emplace_back(Value(exp->graphId()));

    header.shrink_to_fit();
    values.shrink_to_fit();

    for (size_t i = 0; i < header.size(); ++i) {
        // we don't have a field for the expId
        if (header.at(i) == GENERAL_ATTRIBUTE_EXPID) {
            continue;
        }

        QWidget* widget = m_widgetFields.value(header.at(i)).value<QWidget*>();
        Q_ASSERT_X(widget, "ExperimentDesigner", "unable to find the field. It should never happen!");

        QSpinBox* sp = qobject_cast<QSpinBox*>(widget);
        if (sp) {
            sp->setValue(values.at(i).toInt());
            continue;
        }
        QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(widget);
        if (dsp) {
            dsp->setValue(values.at(i).toDouble());
            continue;
        }
        QComboBox* cb = qobject_cast<QComboBox*>(widget);
        if (cb) {
            cb->setCurrentIndex(cb->findText(values.at(i).toQString()));
            continue;
        }
        QCheckBox* chb = qobject_cast<QCheckBox*>(widget);
        if (chb) {
            chb->setChecked(values.at(i).toBool());
            continue;
        }
        QLineEdit* le = qobject_cast<QLineEdit*>(widget);
        if (le) {
            le->setText(values.at(i).toQString());
            continue;
        }
        qFatal("unable to know the widget type.");
    }
}

void ExperimentDesigner::slotNodesWidget()
{
    if (m_selectedModelId == STRING_NULL_PLUGINID) {
        QMessageBox::warning(this, "Experiment", "Please, select a valid 'modelId' first.");
        return;
    }

    const ModelPlugin* model = m_mainApp->models().value(m_selectedModelId);
    const QString& cmd = m_widgetFields.value(GENERAL_ATTRIBUTE_NODES).value<QLineEdit*>()->text();

    NodesGeneratorDlg* adlg = new NodesGeneratorDlg(this, model->nodeAttrsScope(), cmd);
    if (adlg->exec() == QDialog::Accepted) {
        m_widgetFields.value(GENERAL_ATTRIBUTE_NODES).value<QLineEdit*>()->setText(adlg->readCommand());
    }
    adlg->deleteLater();
}

void ExperimentDesigner::slotOutputDir()
{
    QLineEdit* lineedit = m_widgetFields.value(OUTPUT_DIR).value<QLineEdit*>();
    QString path = QFileDialog::getExistingDirectory(this, "Output Directory", lineedit->text());
    if (!path.isEmpty()) {
        lineedit->setText(path);
    }
}

void ExperimentDesigner::slotOutputWidget()
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

    std::vector<Cache*> currFileCaches;
    const ModelPlugin* model = m_mainApp->models().value(m_selectedModelId);
    QString currentHeader = m_widgetFields.value(OUTPUT_HEADER).value<QLineEdit*>()->text();
    if (!currentHeader.isEmpty()) {
        QString errorMsg;
        currFileCaches = Output::parseHeader(currentHeader.split(";", QString::SkipEmptyParts), trialIds, model, errorMsg);
        if (!errorMsg.isEmpty()) {
            QMessageBox::warning(this, "Output Creator", errorMsg);
            for (Cache* c : currFileCaches) c->deleteCache();
        }
    }

    OutputWidget* ow = new OutputWidget(model, trialIds, this, currFileCaches);
    if (ow->exec() == QDialog::Accepted) {
        std::unordered_set<OutputPtr> outputs;
        for (auto& it : ow->caches()) outputs.insert(it.second->output());

        QString outputStr;
        for (OutputPtr o : outputs) outputStr += o->printableHeader('_', true) + ";";
        outputStr.chop(1);

        m_widgetFields.value(OUTPUT_HEADER).value<QLineEdit*>()->setText(outputStr);
    }
    ow->deleteLater();
    for (Cache* c : currFileCaches) c->deleteCache();
}

ExpInputs* ExperimentDesigner::readInputs(const int expId, QString& error) const
{
    if (m_selectedModelId == STRING_NULL_PLUGINID) {
        error = "Please, select a valid 'modelId'.";
        return nullptr;
    } else if (m_selectedGraphId == STRING_NULL_PLUGINID) {
        error = "Please, select a valid 'graphId'.";
        return nullptr;
    } else if (m_enableOutputs->isChecked()
               && (m_widgetFields.value(OUTPUT_DIR).value<QLineEdit*>()->text().isEmpty()
                   || m_widgetFields.value(OUTPUT_HEADER).value<QLineEdit*>()->text().isEmpty())) {
        error = "Please, insert a valid output directory and a output header.";
        return nullptr;
    }

    QStringList header;
    QStringList values;

    header << GENERAL_ATTRIBUTE_EXPID;
    values << QString::number(expId);

    QVariantHash::const_iterator it;
    for (it = m_widgetFields.constBegin(); it != m_widgetFields.constEnd(); ++it) {
        QWidget* widget = it.value().value<QWidget*>();
        Q_ASSERT_X(widget, "ExperimentDesigner", "unable to find the field. It should never happen!");
        if (!widget->isEnabled()) {
            continue;
        }

        header << it.key();

        QSpinBox* sp = qobject_cast<QSpinBox*>(widget);
        if (sp) {
            values << QString::number(sp->value());
            continue;
        }
        QDoubleSpinBox* dsp = qobject_cast<QDoubleSpinBox*>(widget);
        if (dsp) {
            values << QString::number(dsp->value(), 'g', 8);
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

        qFatal("unable to know the widget type.");
    }

    if (!m_enableOutputs->isChecked()) {
        header << OUTPUT_DIR << OUTPUT_HEADER;
        values << "" << "";
    }

    QString errorMsg;
    ExpInputs* inputs = ExpInputs::parse(m_mainApp, header, values, errorMsg);
    if (!inputs) {
        error = "Unable to create the experiment.\nError: \"" + errorMsg + "\"";
    }
    return inputs;
}

void ExperimentDesigner::slotCreateExperiment()
{
    QString error;
    ExpInputs* inputs = readInputs(m_project->generateExpId(), error);
    if (inputs) {
        setExperiment(m_project->newExperiment(inputs, error));
    }

    if (!error.isEmpty()) {
        QMessageBox::warning(this, "Experiment", error);
        delete inputs;
    }
}

void ExperimentDesigner::slotEditExperiment()
{
    Q_ASSERT_X(m_exp, "ExperimentDesigner", "tried to edit a null experiment");
    QString error;
    ExpInputs* inputs = readInputs(m_exp->id(), error);
    if (!inputs || !m_project->editExperiment(m_exp->id(), inputs, error)) {
        QMessageBox::warning(this, "Experiment", error);
        delete inputs;
    }
}

void ExperimentDesigner::slotGraphSelected(const QString& graphId)
{
    m_selectedGraphId = graphId;
    pluginSelected(m_treeItemGraphs, graphId);

    bool validGraph = graphId != STRING_NULL_PLUGINID;
    m_treeItemGraphs->child(m_customGraphIdx)->setHidden(!validGraph || graphId == "customGraph");
    m_treeItemGeneral->setExpanded(validGraph);
    m_treeItemOutputs->setExpanded(validGraph);
}

void ExperimentDesigner::slotModelSelected(const QString& modelId)
{
    m_selectedModelId = modelId;
    pluginSelected(m_treeItemModels, modelId);

    bool nullModel = modelId == STRING_NULL_PLUGINID;
    m_treeItemGeneral->setHidden(nullModel);
    m_treeItemOutputs->setHidden(nullModel);
    m_treeItemGraphs->setHidden(nullModel);
    m_treeItemGraphs->setExpanded(!nullModel);
}

void ExperimentDesigner::pluginSelected(QTreeWidgetItem* itemRoot, const QString& pluginId)
{
    for (int i = 0; i < itemRoot->childCount(); ++i) {
        QTreeWidgetItem* row = itemRoot->child(i);
        QString pId = row->data(0, Qt::UserRole).toString();
        bool hide = !pId.isEmpty() && pId != pluginId;
        row->setHidden(hide);
        m_ui->treeWidget->itemWidget(row, 1)->setDisabled(hide);
    }
}

void ExperimentDesigner::slotPluginAdded(const Plugin* plugin)
{
    QComboBox* cb;
    if (plugin->type() == PluginType::Graph) {
        addPluginAttrs(m_treeItemGraphs, plugin);
        slotGraphSelected(STRING_NULL_PLUGINID); // to hide all fields
        cb = m_widgetFields.value(GENERAL_ATTRIBUTE_GRAPHID).value<QComboBox*>();
    } else if (plugin->type() == PluginType::Model) {
        addPluginAttrs(m_treeItemModels, plugin);
        slotModelSelected(STRING_NULL_PLUGINID); // to hide all fields
        cb = m_widgetFields.value(GENERAL_ATTRIBUTE_MODELID).value<QComboBox*>();
    } else {
        qFatal("invalid plugin type!");
    }

    cb->blockSignals(true);
    cb->insertItem(cb->count(), plugin->id());
    cb->setCurrentIndex(0); // moves to --
    cb->blockSignals(false);
}

void ExperimentDesigner::slotPluginRemoved(const QString& id, PluginType type)
{
    QTreeWidgetItem* tree;
    QComboBox* cb;
    if (type == PluginType::Graph) {
        tree = m_treeItemGraphs;
        cb = m_widgetFields.value(GENERAL_ATTRIBUTE_GRAPHID).value<QComboBox*>();
    } else if (type == PluginType::Model) {
        tree = m_treeItemModels;
        cb = m_widgetFields.value(GENERAL_ATTRIBUTE_MODELID).value<QComboBox*>();
    } else {
        qFatal("invalid plugin type!");
    }

    cb->blockSignals(true);
    cb->removeItem(cb->findText(id));
    cb->blockSignals(false);

    // remove all fields which belog to this plugin
    QVariantHash::iterator i = m_widgetFields.begin();
    while (i != m_widgetFields.end()) {
        if (i.key().startsWith(id + "_")) {
            i = m_widgetFields.erase(i);
        } else {
            ++i;
        }
    }
    for (int i = 0; i < tree->childCount(); ++i) {
        if (tree->child(i)->data(0, Qt::UserRole).toString() == id) {
            delete tree->takeChild(i);
            --i;
        }
    }
}

void ExperimentDesigner::addPluginAttrs(QTreeWidgetItem* tree, const Plugin* plugin)
{
    if (plugin->pluginAttrsNames().size() <= 0) {
        return; // nothing to add
    }

    QTreeWidgetItemIterator it(m_ui->treeWidget);
    while (*it) {
        if ((*it)->parent() == tree && (*it)->data(0, Qt::UserRole).toString() == plugin->id()) {
            return; // plugins already exists
        }
        ++it;
    }

    const QString uid_ = plugin->id() + "_";
    for (const QString& attrName : plugin->pluginAttrsNames()) {
        QTreeWidgetItem* item = new QTreeWidgetItem(tree);
        item->setText(0, attrName);
        item->setData(0, Qt::UserRole, plugin->id());

        const AttributeRange* attrRange = plugin->pluginAttrRange(attrName);
        QWidget* widget = nullptr;
        switch (attrRange->type()) {
        case AttributeRange::Double_Range: {
            widget = newDoubleSpinBox(attrRange->min().toDouble(), attrRange->max().toDouble());
            break;
        }
        case AttributeRange::Int_Range: {
            widget = newSpinBox(attrRange->min().toInt(), attrRange->max().toInt());
            break;
        }
        case AttributeRange::Double_Set:
        case AttributeRange::Int_Set:
        case AttributeRange::String_Set: {
            const SetOfValues* sov = dynamic_cast<const SetOfValues*>(attrRange);
            QComboBox* cb = new QComboBox();
            for (Value v : sov->values()) {
                cb->addItem(v.toQString());
            }
            widget = cb;
            break;
        }
        case AttributeRange::Bool: {
            widget = new QCheckBox();
            break;
        }
        default:
            QLineEdit* le = new QLineEdit();
            le->setText(attrRange->min().toQString());
            widget = le;
        }

        widget->setAutoFillBackground(true); // important! seed qt docs
        item->setHidden(true);
        m_ui->treeWidget->setItemWidget(item, 1, widget);
        // add the uid as prefix to avoid clashes.
        m_widgetFields.insert(uid_ + attrRange->attrName(), QVariant::fromValue(widget));
    }
}

QSpinBox* ExperimentDesigner::newSpinBox(const int min, const int max)
{
    QSpinBox* sp = new QSpinBox();
    sp->setMaximum(max);
    sp->setMinimum(min);
    sp->setButtonSymbols(QSpinBox::NoButtons);
    return sp;
}

QDoubleSpinBox* ExperimentDesigner::newDoubleSpinBox(const double min, const double max)
{
    QDoubleSpinBox* sp = new QDoubleSpinBox();
    sp->setMaximum(max);
    sp->setMinimum(min);
    sp->setDecimals(8);
    sp->setButtonSymbols(QDoubleSpinBox::NoButtons);
    return sp;
}

void ExperimentDesigner::addTreeWidget(QTreeWidgetItem* itemRoot, const QString& label, const QVariant& widget)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(itemRoot);
    m_widgetFields.insert(label, widget);
    item->setText(0, label);
    m_ui->treeWidget->setItemWidget(item, 1, widget.value<QWidget*>());
}

}
