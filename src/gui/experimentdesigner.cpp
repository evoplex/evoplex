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

#include <QDebug>
#include <QMessageBox>
#include <QVariant>

#include "core/include/attrsgenerator.h"
#include "core/include/enum.h"

#include "experimentdesigner.h"
#include "attrsgendlg.h"
#include "linebutton.h"
#include "experimentwidget.h"
#include "projectwidget.h"
#include "outputwidget.h"
#include "titlebar.h"
#include "ui_experimentdesigner.h"

namespace evoplex {

ExperimentDesigner::ExperimentDesigner(MainApp* mainApp, QWidget *parent)
    : QDockWidget(parent),
      m_mainApp(mainApp),
      m_project(nullptr),
      m_ui(new Ui_ExperimentDesigner),
      m_bRemove(new QtMaterialIconButton(QIcon(":/icons/material/delete_white_24"), this)),
      m_bEdit(new QtMaterialIconButton(QIcon(":/icons/material/edit_white_24"), this)),
      m_bAdd(new QtMaterialIconButton(QIcon(":/icons/material/add_white_24"), this))
{
    setWindowTitle("Experiment Designer");
    setObjectName("ExperimentDesigner");
    m_ui->setupUi(this);

    auto titleBar = new TitleBar(this);
    titleBar->setSubtitle("EXPERIMENT DESIGNER");
    titleBar->setTitle(windowTitle());
    setTitleBarWidget(titleBar);

    m_bRemove->setColor(Qt::white);
    m_bRemove->setIconSize(QSize(24,24));
    m_ui->widget->layout()->addWidget(m_bRemove);
    m_bRemove->hide();
    connect(m_bRemove, SIGNAL(pressed()), SLOT(slotRemoveExperiment()));

    m_bEdit->setColor(Qt::white);
    m_bEdit->setIconSize(QSize(24,24));
    m_ui->widget->layout()->addWidget(m_bEdit);
    m_bEdit->hide();
    connect(m_bEdit, SIGNAL(pressed()), SLOT(slotEditExperiment()));

    m_bAdd->setColor(Qt::white);
    m_bAdd->setIconSize(QSize(24,24));
    m_ui->widget->layout()->addWidget(m_bAdd);
    connect(m_bAdd, SIGNAL(pressed()), SLOT(slotCreateExperiment()));

    m_ui->treeWidget->setFocusPolicy(Qt::NoFocus);
    connect(m_ui->cbWidgets, SIGNAL(currentIndexChanged(int)), SLOT(slotActiveWidget(int)));

    auto newTreeItem = [this](const QString& title, bool expand) {
        auto t = new QTreeWidgetItem(m_ui->treeWidget);
        t->setText(0, title);
        t->setToolTip(0, title);
        t->setExpanded(expand);
        return t;
    };

    // setup the tree widget: model
    m_treeItemModels = newTreeItem("Model", true);
    // --  models available
    QComboBox* cb = new QComboBox(m_ui->treeWidget);
    cb->insertItem(0, "--");
    connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(slotModelSelected(int)));
    addGeneralAttr(m_treeItemModels, GENERAL_ATTR_MODELID, cb);

    // setup the tree widget: graph
    m_treeItemGraphs = newTreeItem("Graph", false);
    // -- nodes generator
    LineButton* nodesCmd = new LineButton(this, LineButton::None);
    connect(nodesCmd->button(), SIGNAL(pressed()), SLOT(slotNodesWidget()));
    addGeneralAttr(m_treeItemGraphs, GENERAL_ATTR_NODES, nodesCmd);
    // --  graphs available
    cb = new QComboBox(m_ui->treeWidget);
    cb->insertItem(0, "--");
    connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(slotGraphSelected(int)));
    addGeneralAttr(m_treeItemGraphs, GENERAL_ATTR_GRAPHID, cb);
    // --  graph type
    cb = new QComboBox(m_ui->treeWidget);
    cb->insertItem(0, _enumToString<GraphType>(GraphType::Undirected),
                   static_cast<int>(GraphType::Undirected));
    cb->insertItem(1, _enumToString<GraphType>(GraphType::Directed),
                   static_cast<int>(GraphType::Directed));
    m_graphTypeIdx = m_treeItemGraphs->childCount();
    addGeneralAttr(m_treeItemGraphs, GENERAL_ATTR_GRAPHTYPE, cb);
    // -- edges generator
    LineButton* edgesCmd = new LineButton(this, LineButton::None);
    connect(edgesCmd->button(), SIGNAL(pressed()), SLOT(slotEdgesWidget()));
    m_edgesAttrsIdx = m_treeItemGraphs->childCount();
    addGeneralAttr(m_treeItemGraphs, GENERAL_ATTR_EDGEATTRS, edgesCmd);

    // setup the tree widget: general attributes
    m_treeItemGeneral = newTreeItem("Simulation", false);
    // -- seed
    addGeneralAttr(m_treeItemGeneral, GENERAL_ATTR_SEED)->setValue(100);
    // --  stop at
    addGeneralAttr(m_treeItemGeneral, GENERAL_ATTR_STOPAT)->setValue(1000);
    // --  trials
    addGeneralAttr(m_treeItemGeneral, GENERAL_ATTR_TRIALS);
    // --  auto delete
    addGeneralAttr(m_treeItemGeneral, GENERAL_ATTR_AUTODELETE);

    // setup the tree widget: outputs
    m_treeItemOutputs = newTreeItem("File Outputs", false);
    // -- enabled
    m_enableOutputs = new AttrWidget(AttributeRange::parse(-1, "enabled", "bool"), this);
    m_enableOutputs->setToolTip("save to file");
    QTreeWidgetItem* itemEnabled = new QTreeWidgetItem(m_treeItemOutputs);
    itemEnabled->setText(0, "enable");
    m_ui->treeWidget->setItemWidget(itemEnabled, 1, m_enableOutputs);
    // -- add custom widget: output directory
    LineButton* outDir = new LineButton(this, LineButton::SelectDir);
    addGeneralAttr(m_treeItemOutputs, OUTPUT_DIR, outDir);
    // -- add custom widget: output directory
    LineButton* outHeader = new LineButton(this, LineButton::None);
    connect(outHeader->button(), SIGNAL(pressed()), SLOT(slotOutputWidget()));
    addGeneralAttr(m_treeItemOutputs, OUTPUT_HEADER, outHeader);

/* TODO: make the buttons to avgTrials and saveSteps work*/
/*    // -- avgTrials
    QtMaterialCheckBox* outAvgTrials = new QtMaterialCheckBox("average trials");
    addGeneralAttr(m_treeItemOutputs, OUTPUT_AVGTRIALS, QVariant::fromValue(outAvgTrials));
    // -- steps to save
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
    connect(m_enableOutputs, &AttrWidget::valueChanged,
        [this, outDir, outHeader]() {
            bool b = m_enableOutputs->value().toBool();
            outDir->setEnabled(b);
            outHeader->setEnabled(b);
//          outAvgTrials->setEnabled(b);
        });
    m_enableOutputs->setValue(true);
    m_enableOutputs->setValue(false);

    for (const Plugin* p : m_mainApp->plugins()) { slotPluginAdded(p); }
    connect(m_mainApp, SIGNAL(pluginAdded(const Plugin*)),
            SLOT(slotPluginAdded(const Plugin*)));
    connect(m_mainApp, SIGNAL(pluginRemoved(PluginKey,PluginType)),
            SLOT(slotPluginRemoved(PluginKey,PluginType)));
}

ExperimentDesigner::~ExperimentDesigner()
{
    delete m_ui;
}

void ExperimentDesigner::addWidgetToList(PPageDockWidget* dw)
{
    int id = m_ui->cbWidgets->findData(QVariant::fromValue(dw));
    if (id == -1) {
        m_ui->cbWidgets->addItem(dw->windowTitle(), QVariant::fromValue(dw));
        connect(dw->project().get(), &Project::nameChanged,
            [this, dw](const QString& newName) {
                if (!dw) return;
                int id = m_ui->cbWidgets->findData(QVariant::fromValue(dw));
                if (id >= 0) m_ui->cbWidgets->setItemText(id, newName);
                setWindowTitle(m_ui->cbWidgets->currentText());
            });
    }
}

void ExperimentDesigner::removeWidgetFromList(PPageDockWidget *dw)
{
    int id = m_ui->cbWidgets->findData(QVariant::fromValue(dw));
    if (id != -1) {
        m_ui->cbWidgets->removeItem(id);
        if (m_ui->cbWidgets->count() == 0) {
            // if all projects were closed,
            // make sure we also hide the ExpDesigner
            setFloating(false);
        }
    }
}

PPageDockWidget* ExperimentDesigner::activeWidget() const
{
    return m_ui->cbWidgets->currentData().value<PPageDockWidget*>();
}

void ExperimentDesigner::setActiveWidget(PPageDockWidget* dw)
{
    int id = m_ui->cbWidgets->findData(QVariant::fromValue(dw));
    if (id == -1 || !dw->project()) {
        qWarning() << "project is null or qdockwidget is not in the list!";
    }
    m_ui->cbWidgets->setCurrentIndex(id);
}

void ExperimentDesigner::slotActiveWidget(int idx)
{
    QVariant v = m_ui->cbWidgets->itemData(idx);
    if (v.isValid() && !v.isNull()) {
        PPageDockWidget* dw = v.value<PPageDockWidget*>();
        dw->show();
        dw->raise();
        m_project = dw->project();
        ExperimentWidget* ew = qobject_cast<ExperimentWidget*>(dw);
        m_bAdd->setVisible(!ew);
        setExperiment(ew ? ew->exp() : nullptr);
        setWindowTitle(dw->windowTitle());
    } else {
        m_project = nullptr;
        m_bAdd->setVisible(false);
        setWindowTitle(windowTitle());
    }
}

void ExperimentDesigner::setExperiment(ExperimentPtr exp)
{
    if (!exp) {
        m_bEdit->hide();
        m_bRemove->hide();
        return;
    }

    m_exp = exp;
    m_bEdit->show();
    m_bRemove->show();
    m_enableOutputs->setValue(exp->hasOutputs());

    std::vector<QString> header = exp->inputs()->exportAttrNames(true);
    std::vector<Value> values = exp->inputs()->exportAttrValues();

    // ensure graphId will be filled at the end
    header.emplace_back(GENERAL_ATTR_GRAPHID);
    values.emplace_back(Value(exp->graphId()));

    header.shrink_to_fit();
    values.shrink_to_fit();

    QString invalidFields;
    for (size_t i = 0; i < header.size(); ++i) {
        // we don't have a field for the expId, modelVersion and grapVersion
        if (header.at(i) == GENERAL_ATTR_EXPID ||
                header.at(i) == GENERAL_ATTR_GRAPHVS ||
                header.at(i) == GENERAL_ATTR_MODELVS) {
            continue;
        }

        if (!values.at(i).isValid()) {
            invalidFields += "  - " + header.at(i) + "\n";
            continue;
        }

        AttrWidget* aw = m_nodeAttrWidgets.value(header.at(i));
        Q_ASSERT_X(aw, "ExperimentDesigner",
                   "unable to find the field. It should never happen!");

        QComboBox* cb = qobject_cast<QComboBox*>(aw->widget());
        if (cb) {
            int idx = -1;
            if (header.at(i) == GENERAL_ATTR_GRAPHID) {
                idx = cb->findData(QVariant::fromValue(exp->graphPlugin()->key()));
            } else if (header.at(i) == GENERAL_ATTR_MODELID) {
                idx = cb->findData(QVariant::fromValue(exp->modelPlugin()->key()));
            } else {
                idx = cb->findText(values.at(i).toQString());
            }
            // when opening an experiment with errors, the value might be invalid,
            // then idx might be equal to -1 (item not found).
            // passing -1 will show an empty combobox, which is fine here
            cb->setCurrentIndex(idx);
            if (idx < 0) invalidFields += "  - " + header.at(i) + "\n";
            continue;
        }

        aw->setValue(values.at(i));
    }

    if (!invalidFields.isEmpty()) {
        parentWidget()->blockSignals(true);
        QMessageBox::information(this, "Experiment",
            "Could not find a valid value for the attributes below:\n" + invalidFields +
            "\nPlease, use the Experiment Designer to fix them, click on Edit and try again!");
        parentWidget()->blockSignals(false);
    }
}

void ExperimentDesigner::slotEdgesWidget()
{
    if (m_selectedModelKey == PluginKey() || m_selectedGraphKey == PluginKey()) {
        QMessageBox::warning(this, "Experiment",
            "Please, select a valid 'graphId' first.");
        return;
    }

    const ModelPlugin* model = m_mainApp->model(m_selectedModelKey);
    if (model->edgeAttrsScope().empty()) {
        QMessageBox::information(this, "Attributes Generator",
            "Cannot open the attributes generator for the current model."
            " The edges do not have attributes");
        return;
    }

    const QString& cmd = m_nodeAttrWidgets.value(GENERAL_ATTR_EDGEATTRS)->value().toQString();
    AttrsGenDlg* adlg = new AttrsGenDlg(this, AttrsGenDlg::Mode::Edges,
                                        model->edgeAttrsScope(), cmd);

    if (adlg->exec() == QDialog::Accepted) {
        m_nodeAttrWidgets.value(GENERAL_ATTR_EDGEATTRS)->setValue(adlg->readCommand());
    }
    adlg->deleteLater();
}

void ExperimentDesigner::slotNodesWidget()
{
    if (m_selectedModelKey == PluginKey()) {
        QMessageBox::warning(this, "Experiment",
            "Please, select a valid 'modelId' first.");
        return;
    }

    const ModelPlugin* model = m_mainApp->model(m_selectedModelKey);
    const QString& cmd = m_nodeAttrWidgets.value(GENERAL_ATTR_NODES)->value().toQString();

    AttrsGenDlg* adlg = new AttrsGenDlg(this, AttrsGenDlg::Mode::Nodes,
                                        model->nodeAttrsScope(), cmd);

    if (adlg->exec() == QDialog::Accepted) {
        m_nodeAttrWidgets.value(GENERAL_ATTR_NODES)->setValue(adlg->readCommand());
    }
    adlg->deleteLater();
}

void ExperimentDesigner::slotOutputWidget()
{
    if (m_selectedModelKey == PluginKey()) {
        QMessageBox::warning(this, "Experiment", "Please, select a valid 'modelId' first.");
        return;
    }

    int numTrials = m_nodeAttrWidgets.value(GENERAL_ATTR_TRIALS)->value().toInt();
    std::vector<int> trialIds;
    trialIds.reserve(static_cast<size_t>(numTrials));
    for (int id = 0; id < numTrials; ++id) {
        trialIds.emplace_back(id);
    }

    std::vector<Cache*> currFileCaches;
    const ModelPlugin* model = m_mainApp->model(m_selectedModelKey);
    QString currentHeader = m_nodeAttrWidgets.value(OUTPUT_HEADER)->value().toQString();
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
        for (auto const& o : outputs) {
            outputStr += o->printableHeader('_', true) + ";";
        }
        outputStr.chop(1);

        m_nodeAttrWidgets.value(OUTPUT_HEADER)->setValue(outputStr);
    }
    ow->deleteLater();
    for (Cache* c : currFileCaches) c->deleteCache();
}

std::unique_ptr<ExpInputs> ExperimentDesigner::readInputs(const int expId, QString& error) const
{
    if (m_selectedModelKey == PluginKey()) {
        error = "Please, select a valid 'modelId'.";
        return nullptr;
    } else if (m_selectedGraphKey == PluginKey()) {
        error = "Please, select a valid 'graphId'.";
        return nullptr;
    } else if (m_enableOutputs->value().toBool()
               && (m_nodeAttrWidgets.value(OUTPUT_DIR)->value().toQString().isEmpty()
                   || m_nodeAttrWidgets.value(OUTPUT_HEADER)->value().toQString().isEmpty())) {
        error = "Please, insert a valid output directory and a output header.";
        return nullptr;
    }

    QStringList header;
    QStringList values;

    header << GENERAL_ATTR_EXPID;
    values << QString::number(expId);

    const QString modelKey_ = Plugin::keyStr(m_selectedModelKey) + "_";
    const QString graphKey_ = Plugin::keyStr(m_selectedGraphKey) + "_";
    for (auto it = m_nodeAttrWidgets.cbegin(); it != m_nodeAttrWidgets.cend(); ++it) {
        if (!it.value()->isEnabled()) {
            continue;
        }

        // modelId and graphId are added later
        if (it.key() == GENERAL_ATTR_GRAPHID ||
                it.key() == GENERAL_ATTR_MODELID) {
            continue;
        }

        // if it's a plugin field, we need to remove the
        // version number from the key
        if (it.key().startsWith(modelKey_)) {
            header << m_selectedModelKey.first + "_" + it.value()->attrName();
        } else if (it.key().startsWith(graphKey_)) {
            header << m_selectedGraphKey.first + "_" + it.value()->attrName();
        } else {
            header << it.value()->attrName();
        }

        values << it.value()->value().toQString('g', 8);
    }

    if (!m_enableOutputs->value().toBool()) {
        header << OUTPUT_DIR << OUTPUT_HEADER;
        values << "" << "";
    }

    header << GENERAL_ATTR_GRAPHID << GENERAL_ATTR_GRAPHVS;
    values << m_selectedGraphKey.first << QString::number(m_selectedGraphKey.second);

    header << GENERAL_ATTR_MODELID << GENERAL_ATTR_MODELVS;
    values << m_selectedModelKey.first << QString::number(m_selectedModelKey.second);

    QString errorMsg;
    auto inputs = ExpInputs::parse(m_mainApp, header, values, errorMsg);
    if (!inputs || !errorMsg.isEmpty()) {
        error += "Unable to create the experiment.\nError: \"" + errorMsg + "\"";
        return nullptr; // this GUI should never create experiments with errors
    }

    return inputs;
}

void ExperimentDesigner::slotCreateExperiment()
{
    QString error;
    ExperimentPtr exp = nullptr;
    ExpInputsPtr inputs = readInputs(m_project->generateExpId(), error);
    if (inputs) { exp = m_project->newExperiment(std::move(inputs), error); }
    if (!error.isEmpty()) { QMessageBox::warning(this, "Experiment", error); }
    setExperiment(exp);
}

void ExperimentDesigner::slotRemoveExperiment()
{
    QString error;
    ExperimentPtr exp = m_exp.lock();
    if (exp && !m_project->removeExperiment(exp->id(), error)) {
        QMessageBox::warning(this, "Experiment", error);
    }
    setExperiment(nullptr);
}

void ExperimentDesigner::slotEditExperiment()
{
    ExperimentPtr exp = m_exp.lock();
    if (!exp) {
        QMessageBox::warning(this, "Experiment",
                "This experiment is no longer part of this project.");
        setExperiment(nullptr);
        return;
    }
    QString error;
    ExpInputsPtr inputs = readInputs(exp->id(), error);
    if (!inputs || !m_project->editExperiment(exp->id(), std::move(inputs), error)) {
        QMessageBox::warning(this, "Experiment", error);
    }
}

void ExperimentDesigner::slotGraphSelected(int cbIdx)
{
    auto cb = qobject_cast<QComboBox*>(m_nodeAttrWidgets.value(GENERAL_ATTR_GRAPHID)->widget());
    m_selectedGraphKey = cb->itemData(cbIdx).value<PluginKey>();
    pluginSelected(m_treeItemGraphs, m_selectedGraphKey);

    const GraphPlugin* graph = m_mainApp->graph(m_selectedGraphKey);
    m_treeItemGeneral->setExpanded(graph);
    m_treeItemOutputs->setExpanded(graph);
    if (graph) {
        m_treeItemGraphs->child(m_graphTypeIdx)->setHidden(
                graph->validGraphTypes().size() < 2);
        const ModelPlugin* model = m_mainApp->model(m_selectedModelKey);
        m_treeItemGraphs->child(m_edgesAttrsIdx)->setHidden(
                !graph->supportsEdgeAttrsGen() ||
                !model || model->edgeAttrsScope().empty());
    } else {
        m_treeItemGraphs->child(m_graphTypeIdx)->setHidden(true);
        m_treeItemGraphs->child(m_edgesAttrsIdx)->setHidden(true);
    }
}

void ExperimentDesigner::slotModelSelected(int cbIdx)
{
    auto cb = qobject_cast<QComboBox*>(m_nodeAttrWidgets.value(GENERAL_ATTR_MODELID)->widget());
    m_selectedModelKey = cb->itemData(cbIdx).value<PluginKey>();
    pluginSelected(m_treeItemModels, m_selectedModelKey);

    const ModelPlugin* model = m_mainApp->model(m_selectedModelKey);
    m_treeItemGeneral->setHidden(!model);
    m_treeItemOutputs->setHidden(!model);
    m_treeItemGraphs->setHidden(!model);
    m_treeItemGraphs->setExpanded(model);

    if (model) {
        const GraphPlugin* graph = m_mainApp->graph(m_selectedGraphKey);
        m_treeItemGraphs->child(m_edgesAttrsIdx)->setHidden(
                model->edgeAttrsScope().empty() ||
                !graph || !graph->supportsEdgeAttrsGen());
    } else {
        m_treeItemGraphs->child(m_edgesAttrsIdx)->setHidden(true);
    }
}

void ExperimentDesigner::pluginSelected(QTreeWidgetItem* itemRoot, const PluginKey& key)
{
    for (int i = 0; i < itemRoot->childCount(); ++i) {
        QTreeWidgetItem* row = itemRoot->child(i);
        PluginKey _key = row->data(0, Qt::UserRole).value<PluginKey>();
        bool hide = !_key.first.isEmpty() && _key != key;
        row->setHidden(hide);
        m_ui->treeWidget->itemWidget(row, 1)->setDisabled(hide);
    }
}

void ExperimentDesigner::slotPluginAdded(const Plugin* plugin)
{
    int numVersions = 0;
    QComboBox* cb;
    if (plugin->type() == PluginType::Graph) {
        addPluginAttrs(m_treeItemGraphs, plugin);
        slotGraphSelected(0); // to hide all fields
        cb = qobject_cast<QComboBox*>(m_nodeAttrWidgets.value(GENERAL_ATTR_GRAPHID)->widget());
        numVersions = m_mainApp->graphs().values(plugin->id()).size();
    } else if (plugin->type() == PluginType::Model) {
        addPluginAttrs(m_treeItemModels, plugin);
        slotModelSelected(0); // to hide all fields
        cb = qobject_cast<QComboBox*>(m_nodeAttrWidgets.value(GENERAL_ATTR_MODELID)->widget());
        numVersions = m_mainApp->models().values(plugin->id()).size();
    } else {
        qFatal("invalid plugin type!");
    }

    // let's append the version number only when there's
    // more than one version available
    QString label;
    if (numVersions == 1) {
        label = plugin->id();
    } else if (numVersions == 2) {
        // this is the first time we have 2 versions of the same plugin
        // so, we must fix the label of the existing plugin as well
        int existingIdx = cb->findText(plugin->id());
        PluginKey existing = cb->itemData(existingIdx).value<PluginKey>();
        Q_ASSERT(existingIdx >= 0 && existing.first == plugin->id());
        cb->setItemText(existingIdx, Plugin::keyStr(existing));
        label = Plugin::keyStr(plugin->key());
    } else if (numVersions > 2) {
        label = Plugin::keyStr(plugin->key());
    } else {
        qFatal("the plugin was not loaded correctly! It should never happen!");
    }

    cb->blockSignals(true);
    cb->insertItem(cb->count(), label, QVariant::fromValue(plugin->key()));
    cb->model()->sort(0);
    cb->setCurrentText("--"); // moves to --
    cb->blockSignals(false);
}

void ExperimentDesigner::slotPluginRemoved(PluginKey key, PluginType type)
{
    QTreeWidgetItem* tree;
    QComboBox* cb;
    if (type == PluginType::Graph) {
        tree = m_treeItemGraphs;
        cb = qobject_cast<QComboBox*>(m_nodeAttrWidgets.value(GENERAL_ATTR_GRAPHID)->widget());
    } else if (type == PluginType::Model) {
        tree = m_treeItemModels;
        cb = qobject_cast<QComboBox*>(m_nodeAttrWidgets.value(GENERAL_ATTR_MODELID)->widget());
    } else {
        qFatal("invalid plugin type!");
    }

    cb->blockSignals(true);
    cb->removeItem(cb->findData(QVariant::fromValue(key)));
    cb->blockSignals(false);

    // remove all fields which belog to this plugin
    const QString keyStr_ = Plugin::keyStr(key) + "_";
    auto it = m_nodeAttrWidgets.begin();
    while (it != m_nodeAttrWidgets.end()) {
        if (it.key().startsWith(keyStr_)) {
            it = m_nodeAttrWidgets.erase(it);
        } else {
            ++it;
        }
    }
    for (int i = 0; i < tree->childCount(); ++i) {
        if (tree->child(i)->data(0, Qt::UserRole).value<PluginKey>() == key) {
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
        if ((*it)->parent() == tree &&
            (*it)->data(0, Qt::UserRole).value<PluginKey>() == plugin->key()) {
            return; // plugins already exists
        }
        ++it;
    }

    const QString key_ = Plugin::keyStr(plugin->key()) + "_";
    for (const QString& attrName : plugin->pluginAttrsNames()) {
        QTreeWidgetItem* item = new QTreeWidgetItem(tree);
        item->setText(0, attrName);
        item->setToolTip(0, attrName);
        item->setData(0, Qt::UserRole, QVariant::fromValue(plugin->key()));
        item->setHidden(true);
        auto attrWidget = new AttrWidget(plugin->pluginAttrRange(attrName), this);
        m_ui->treeWidget->setItemWidget(item, 1, attrWidget);
        // add the uid as prefix to avoid clashes.
        m_nodeAttrWidgets.insert(key_ + attrWidget->attrName(), attrWidget);
    }
}

AttrWidget* ExperimentDesigner::addGeneralAttr(QTreeWidgetItem* itemRoot,
        const QString& attrName, QWidget* customWidget)
{
    AttrWidget* widget = new AttrWidget(
            m_mainApp->generalAttrsScope().value(attrName), this, customWidget);
    m_nodeAttrWidgets.insert(attrName, widget);

    QTreeWidgetItem* item = new QTreeWidgetItem(itemRoot);
    item->setText(0, attrName);
    item->setToolTip(0, attrName);
    m_ui->treeWidget->setItemWidget(item, 1, widget);

    return widget;
}

} // evoplex
