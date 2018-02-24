/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QCloseEvent>
#include <QMessageBox>

#include "outputwidget.h"
#include "ui_outputwidget.h"

namespace evoplex {

OutputWidget::OutputWidget(const ModelPlugin* modelPlugin, const std::vector<int>& trialIds,
                           QWidget *parent, const std::vector<Cache*>& init)
    : QDialog(parent)
    , m_ui(new Ui_OutputWidget)
    , m_modelPlugin(modelPlugin)
    , m_trialIds(trialIds)
    , m_hasChanges(false)
{
    setWindowModality(Qt::ApplicationModal);
    m_ui->setupUi(this);

    connect(m_ui->bBox, &QDialogButtonBox::accepted, [this]() { slotClose(false); });
    connect(m_ui->bBox, &QDialogButtonBox::rejected, [this]() { slotClose(true); });

    for (QString funcName : DefaultOutput::availableFunctions()) {
        m_ui->func->addItem(funcName, DefaultFunc);
    }
    for (QString funcName : m_modelPlugin->customOutputs()) {
        m_ui->func->addItem(funcName, CustomFunc);
    }

    m_ui->entityAgent->setChecked(true);
    slotEntityChanged(true); // init
    m_ui->entityEdge->setDisabled(modelPlugin->edgeAttrNames().empty());

    connect(m_ui->add, SIGNAL(clicked(bool)), SLOT(slotAdd()));
    connect(m_ui->func, SIGNAL(currentIndexChanged(int)), SLOT(slotFuncChanged(int)));
    connect(m_ui->entityAgent, SIGNAL(toggled(bool)), SLOT(slotEntityChanged(bool)));

    m_ui->table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_ui->table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_ui->table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_ui->table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_ui->table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);

    // init table
    for (const Cache* cache : init) {
        const int rootId = m_ui->table->rowCount();
        DefaultOutputSP df = std::dynamic_pointer_cast<DefaultOutput>(cache->output());
        if (df) {
            const QString entityStr = df->entity() == DefaultOutput::E_Agents
                                    ? m_ui->entityAgent->text() : m_ui->entityEdge->text();

            for (Value input : cache->inputs()) {
                RowInfo rowInfo;
                rowInfo.id = m_ui->table->rowCount();
                rowInfo.equalToId = rowInfo.id == rootId ? -1 : rootId;
                insertRow(rowInfo, df->functionStr(), DefaultFunc, entityStr, df->entity(),
                          df->valueSpace()->attrName(), input.toQString());
            }
        } else if (std::dynamic_pointer_cast<CustomOutput>(cache->output())) {
            for (Value func : cache->inputs()) {
                RowInfo rowInfo;
                rowInfo.id = m_ui->table->rowCount();
                rowInfo.equalToId = rowInfo.id == rootId ? -1 : rootId;
                insertRow(rowInfo, func.toQString(), CustomFunc);
            }
        } else {
            qFatal("[OutputWidget] : error! invalid Output object.");
        }
    }
}

OutputWidget::~OutputWidget()
{
    delete m_ui;
}

void OutputWidget::closeEvent(QCloseEvent*)
{
    slotClose(true);
}

void OutputWidget::slotClose(bool canceled)
{
    if (!m_hasChanges) {
        reject(); // reject() to let other know that nothing was changed
        return;
    } else if (canceled) {
        int res = QMessageBox::warning(this, "Evoplex",
                    "Are you sure you want to want to discard your changes?",
                    QMessageBox::Yes, QMessageBox::Cancel);
        if (res == QMessageBox::Yes) {
            reject();
            return;
        } else {
            return;
        }
    }

    Q_ASSERT(m_allCaches.empty());

    for (int row = 0; row < m_ui->table->rowCount(); ++row) {
        QString funcStr = m_ui->table->item(row, 1)->text();
        FuncType funcType = (FuncType) m_ui->table->item(row, 1)->data(Qt::UserRole).toInt();
        DefaultOutput::Entity entity = (DefaultOutput::Entity) m_ui->table->item(row, 2)->data(Qt::UserRole).toInt();
        QString attr = m_ui->table->item(row, 3)->text();
        QString inputStr = m_ui->table->item(row, 4)->text();
        RowInfo rinfo = m_ui->table->item(row, 4)->data(Qt::UserRole).value<RowInfo>();

        const ValueSpace* entityValSpace;
        if (entity == DefaultOutput::E_Agents) {
            entityValSpace = m_modelPlugin->agentAttrSpace().value(attr);
        } else {
            entityValSpace = m_modelPlugin->edgeAttrSpace().value(attr);
        }

        if (rinfo.equalToId == -1) {
            Cache* cache = nullptr;
            if (funcType == DefaultFunc) {
                Value input = entityValSpace->validate(inputStr);
                DefaultOutput::Function func = DefaultOutput::funcFromString(funcStr);
                Q_ASSERT(func != DefaultOutput::F_Invalid && input.isValid());
                OutputSP newOutput (new DefaultOutput(func, entity, entityValSpace));
                cache = newOutput->addCache({input}, m_trialIds);
            } else {
                OutputSP newOutput (new CustomOutput());
                cache = newOutput->addCache({Value(funcStr)}, m_trialIds);
            }
            m_allCaches.insert({rinfo.id, cache});
        } else {
            OutputSP existingOutput = m_allCaches.at(rinfo.equalToId)->output();
            Value input;
            if (funcType == DefaultFunc) {
                input = entityValSpace->validate(inputStr);
            } else {
                input = Value(funcStr);
            }
            m_allCaches.insert({rinfo.id, existingOutput->addCache({input}, m_trialIds)});
        }
    }

    if (m_allCaches.empty()) {
        reject();
    } else {
        accept();
    }
}

void OutputWidget::slotFuncChanged(int idx)
{
    m_ui->input->clear();
    bool dfFunc = m_ui->func->itemData(idx) == DefaultFunc;
    m_ui->attr->setEnabled(dfFunc);
    m_ui->input->setEnabled(dfFunc);
}

void OutputWidget::slotEntityChanged(bool isAgent)
{
    m_ui->attr->clear();
    m_ui->input->clear();
    if (isAgent) {
        m_currEntity = DefaultOutput::E_Agents;
        m_currEntityStr = m_ui->entityAgent->text();
        for (QString n : m_modelPlugin->agentAttrNames()) {
            m_ui->attr->addItem(n);
        }
    } else {
        m_currEntity = DefaultOutput::E_Edges;
        m_currEntityStr = m_ui->entityEdge->text();
        for (QString n : m_modelPlugin->edgeAttrNames()) {
            m_ui->attr->addItem(n);
        }
    }
    m_ui->input->setEnabled(m_ui->attr->count() > 0);
}

void OutputWidget::slotAdd()
{
    m_hasChanges = true;

    RowInfo rowInfo;
    for (int row = 0; row < m_ui->table->rowCount(); ++row) {
        if (m_ui->table->item(row, 1)->data(Qt::UserRole) == m_ui->func->currentData() &&
            m_ui->table->item(row, 1)->text() == m_ui->func->currentText() &&
            m_ui->table->item(row, 2)->data(Qt::UserRole).toInt() == m_currEntity &&
            m_ui->table->item(row, 3)->text() == m_ui->attr->currentText())
        {
            if (m_ui->table->item(row, 4)->text() == m_ui->input->text()) {
                QMessageBox::warning(this, "Evoplex", "It's already in the table.");
                return;
            }
            rowInfo.equalToId = m_ui->table->item(row, 0)->text().toInt();
        }
    }

    const ValueSpace* entityValSpace;
    if (m_ui->entityAgent->isChecked()) {
        entityValSpace = m_modelPlugin->agentAttrSpace().value(m_ui->attr->currentText());
    } else {
        entityValSpace = m_modelPlugin->edgeAttrSpace().value(m_ui->attr->currentText());
    }

    if (m_ui->func->currentData().toInt() == DefaultFunc) {
        if (!entityValSpace->validate(m_ui->input->text()).isValid()) {
            QMessageBox::warning(this, "Evoplex",
                                 "The 'input' is not valid for the current 'attribute'.\n"
                                 "Expected: " + entityValSpace->space());
            return;
        }
    }

    rowInfo.id = m_ui->table->rowCount();
    insertRow(rowInfo, m_ui->func->currentText(), (FuncType) m_ui->func->currentData().toInt(),
              m_currEntityStr, m_currEntity, m_ui->attr->currentText(), m_ui->input->text());
}

void OutputWidget::insertRow(const RowInfo rowInfo, const QString& funcStr, const FuncType funcType,
                             QString entityStr, DefaultOutput::Entity entity, QString attr, QString input)
{
    m_ui->table->insertRow(rowInfo.id);

    m_ui->table->setItem(rowInfo.id, 0, new QTableWidgetItem(QString::number(rowInfo.id)));

    QTableWidgetItem* itemFunc = new QTableWidgetItem(funcStr);
    itemFunc->setData(Qt::UserRole, funcType);
    m_ui->table->setItem(rowInfo.id, 1, itemFunc);

    QTableWidgetItem* itemEntity = new QTableWidgetItem(entityStr);
    itemEntity->setData(Qt::UserRole, entity);
    m_ui->table->setItem(rowInfo.id, 2, itemEntity);

    m_ui->table->setItem(rowInfo.id, 3, new QTableWidgetItem(attr));

    QTableWidgetItem* itemInput = new QTableWidgetItem(input);
    itemInput->setData(Qt::UserRole, QVariant::fromValue(rowInfo));
    m_ui->table->setItem(rowInfo.id, 4, itemInput);
}

} // evoplex
