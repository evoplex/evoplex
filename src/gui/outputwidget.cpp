/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QMessageBox>

#include "outputwidget.h"
#include "core/utils.h"

namespace evoplex {

OutputWidget::OutputWidget(const ModelPlugin* modelPlugin, QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_OutputWidget)
    , m_modelPlugin(modelPlugin)
{
    m_ui->setupUi(this);

    for (QString funcName : DefaultOutput::availableFunctions()) {
        m_ui->func->addItem(funcName, 0); // default function
    }
    for (QString funcName : m_modelPlugin->customOutputs()) {
        m_ui->func->addItem(funcName, 1); // custom function
    }
    connect(m_ui->add, SIGNAL(clicked(bool)), SLOT(slotAdd()));
    connect(m_ui->func, SIGNAL(currentIndexChanged(int)), SLOT(slotFuncChanged(int)));
    connect(m_ui->entityAgent, SIGNAL(toggled(bool)), SLOT(slotEntityChanged(bool)));
    m_ui->entityAgent->setChecked(true);
}

OutputWidget::~OutputWidget()
{
    delete m_ui;
}

void OutputWidget::hideEvent(QHideEvent* event)
{
    if (!m_newOutputs.empty()) {
        emit (closed(m_newOutputs));
        m_newOutputs.clear();
    }
    QWidget::hideEvent(event);
}

void OutputWidget::slotFuncChanged(int idx)
{
    m_ui->input->clear();
    m_ui->attr->setEnabled(m_ui->func->itemData(idx) == 0);
    m_ui->input->setEnabled(m_ui->func->itemData(idx) == 0);
}

void OutputWidget::slotEntityChanged(bool isAgent)
{
    m_ui->attr->clear();
    m_ui->input->clear();
    if (isAgent) {
        for (QString n : m_modelPlugin->agentAttrRange().min.names()) {
            m_ui->attr->addItem(n);
        }
    } else {
        for (QString n : m_modelPlugin->edgeAttrRange().min.names()) {
            m_ui->attr->addItem(n);
        }
    }
    m_ui->input->setEnabled(m_ui->attr->count() > 0);
}

void OutputWidget::slotAdd()
{
    int row = m_ui->table->rowCount();
    QString attrName = m_ui->attr->currentText();

    Output* output;
    Value input;
    if (m_ui->func->currentData().toInt() == 0) { // default output
        DefaultOutput::Entity entity;
        if (m_ui->entityAgent->isChecked()) {
            entity = DefaultOutput::E_Agents;
            input = m_modelPlugin->agentAttrSpace().value(attrName)->validate(m_ui->input->text());
        } else {
            entity = DefaultOutput::E_Edges;
            input = m_modelPlugin->edgeAttrSpace().value(attrName)->validate(m_ui->input->text());
        }

        if (!input.isValid()) {
            QMessageBox::warning(this, "Wrong Input", "The 'input' is not valid for the current 'attribute'.");
            return;
        }

        DefaultOutput::Function func = DefaultOutput::funcFromString(m_ui->func->currentText());
        Q_ASSERT(func != DefaultOutput::F_Invalid);

        output = new DefaultOutput(func, entity, attrName, m_ui->attr->currentIndex(), {input}, m_trialIds);

        m_ui->table->insertRow(row);
        m_ui->table->setItem(row, 0, new QTableWidgetItem("Default"));
        m_ui->table->setItem(row, 1, new QTableWidgetItem(m_ui->func->currentText()));
        m_ui->table->setItem(row, 2, new QTableWidgetItem(entity == DefaultOutput::E_Agents ? "Agent" : "Edge"));
        m_ui->table->setItem(row, 3, new QTableWidgetItem(m_ui->attr->currentText()));
        m_ui->table->setItem(row, 4, new QTableWidgetItem(m_ui->input->text()));

    } else { // custom output
        input = Value(m_ui->func->currentText());
        output = new CustomOutput({input}, m_trialIds);

        m_ui->table->insertRow(row);
        m_ui->table->setItem(row, 0, new QTableWidgetItem("Custom"));
        m_ui->table->setItem(row, 1, new QTableWidgetItem(m_ui->func->currentText()));
    }

    // make sure it is unique
    for (Output* existing : m_allOutputs) {
        if (existing && existing->operator ==(output)
                && existing->allInputs().front().operator ==(output->allInputs().front())) {
            m_ui->table->removeRow(row);
            delete output;
            output = nullptr;
            return;
        }
    }

    m_newOutputs.emplace_back(output);
    m_allOutputs.emplace_back(output);
}

void OutputWidget::fill(std::vector<Output*> outputs)
{
    m_ui->table->clearContents();
    qDeleteAll(m_allOutputs);
    m_allOutputs.clear();
    m_newOutputs.clear();

    for (Output* output : outputs) {
        int row = m_ui->table->rowCount();
        if (dynamic_cast<DefaultOutput*>(output)) {
            DefaultOutput* df = dynamic_cast<DefaultOutput*>(output);
            std::vector<int> trialIds(df->trialIds().begin(), df->trialIds().end());

            for (Value input : df->allInputs()) {
                // we create one Output object for each input
                m_newOutputs.emplace_back(new DefaultOutput(
                        df->function(), df->entity(), df->attrName(),
                        df->attrIdx(), {input}, trialIds));

                m_ui->table->insertRow(row);
                m_ui->table->setItem(row, 0, new QTableWidgetItem("Default"));
                m_ui->table->setItem(row, 1, new QTableWidgetItem(df->functionStr()));
                m_ui->table->setItem(row, 2, new QTableWidgetItem(df->entity() == DefaultOutput::E_Agents ? "Agent" : "Edge"));
                m_ui->table->setItem(row, 3, new QTableWidgetItem(df->attrName()));
                m_ui->table->setItem(row, 4, new QTableWidgetItem(input.toQString()));
                ++row;
            }
        } else if (dynamic_cast<CustomOutput*>(output)) {
            CustomOutput* cs = dynamic_cast<CustomOutput*>(output);
            std::vector<int> trialIds(cs->trialIds().begin(), cs->trialIds().end());

            for (Value func : cs->allInputs()) {
                // we create one Output object for each input
                m_newOutputs.emplace_back(new CustomOutput({func}, trialIds));

                m_ui->table->insertRow(row);
                m_ui->table->setItem(row, 0, new QTableWidgetItem("Custom"));
                m_ui->table->setItem(row, 1, new QTableWidgetItem(func.toQString()));
                ++row;
            }
        } else {
            qFatal("[OutputWidget] : error! invalid Output object.");
        }
        m_allOutputs.emplace_back(m_newOutputs.back());
    }
}

}
