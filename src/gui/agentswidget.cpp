/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QFileDialog>
#include <QMessageBox>

#include "agentswidget.h"

namespace evoplex
{

AgentsWidget::AgentsWidget(const AttributesSpace& agentAttrsSpace, AgentsGenerator* ag, QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_AgentsWidget)
    , m_agentAttrsSpace(agentAttrsSpace)
{
    setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowModality(Qt::ApplicationModal);
    m_ui->setupUi(this);

    connect(m_ui->saveAs, SIGNAL(pressed()), SLOT(slotSaveAs()));
    connect(m_ui->cancel, SIGNAL(pressed()), SLOT(close()));
    connect(m_ui->ok, &QPushButton::pressed, [this](){
        emit(closed(readCommand()));
        close();
    });

    connect(m_ui->browseFile, &QPushButton::pressed, [this]() {
        QString path = QFileDialog::getOpenFileName(this,
                                                    "Initial Population",
                                                    m_ui->filepath->text(),
                                                    "Text Files (*.csv *.txt)");
        if (!path.isEmpty()) {
            m_ui->filepath->setText(path);
        }
    });

    connect(m_ui->func, &QComboBox::currentTextChanged, [this]() {
        bool notRand = m_ui->func->currentData() != AgentsGenerator::F_Rand;
        m_ui->lseed->setHidden(notRand);
        m_ui->fseed->setHidden(notRand);
    });
    m_ui->func->insertItem(0, AgentsGenerator::enumToString(AgentsGenerator::F_Min), AgentsGenerator::F_Min);
    m_ui->func->insertItem(1, AgentsGenerator::enumToString(AgentsGenerator::F_Max), AgentsGenerator::F_Max);
    m_ui->func->insertItem(2, AgentsGenerator::enumToString(AgentsGenerator::F_Rand), AgentsGenerator::F_Rand);
    m_ui->func->setCurrentIndex(0);

    for (const ValueSpace* vs : m_agentAttrsSpace) {
        const int row = vs->id();
        m_ui->table->insertRow(row);
        m_ui->table->setItem(row, 0, new QTableWidgetItem(vs->attrName()));

        QComboBox* cb = new QComboBox();
        cb->insertItem(0, AgentsGenerator::enumToString(AgentsGenerator::F_Min), AgentsGenerator::F_Min);
        cb->insertItem(1, AgentsGenerator::enumToString(AgentsGenerator::F_Max), AgentsGenerator::F_Max);
        cb->insertItem(2, AgentsGenerator::enumToString(AgentsGenerator::F_Rand), AgentsGenerator::F_Rand);
        cb->insertItem(3, AgentsGenerator::enumToString(AgentsGenerator::F_Value), AgentsGenerator::F_Value);
        m_ui->table->setCellWidget(row, 1, cb);

        QLineEdit* le = new QLineEdit();
        connect(cb, &QComboBox::currentTextChanged, [cb, le, vs](){
            if (cb->currentIndex() == AgentsGenerator::F_Min
                    || cb->currentIndex() == AgentsGenerator::F_Max) {
                le->setHidden(true);
                return;
            } else if (cb->currentIndex() == AgentsGenerator::F_Rand) {
                le->setToolTip("Type the PRG seed (integer).");
            } else if (cb->currentIndex() == AgentsGenerator::F_Value) {
                le->setToolTip("Type a valid value for this attribute.\n"
                               "Expected: " + vs->space());
            } else {
                Q_ASSERT(false);
            }
            le->setHidden(false);
        });
        m_ui->table->setCellWidget(row, 2, le);
    }

    fill(ag);
}

AgentsWidget::~AgentsWidget()
{
    delete m_ui;
}

void AgentsWidget::slotSaveAs()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                "Save Agents",
                                                m_ui->filepath->text(),
                                                "Text Files (*.csv)");
    if (path.isEmpty()) {
        return;
    }

    QString cmd = readCommand();
    if (cmd.isEmpty()) {
        return;
    }

    QString errMsg;
    AgentsGenerator* ag = AgentsGenerator::parse(m_agentAttrsSpace, cmd, errMsg);
    Q_ASSERT(errMsg.isEmpty());

    Agents agents = ag->create();
    Q_ASSERT(agents.size() > 0);
    if (!AgentsGenerator::saveToFile(path, agents)) {
        QMessageBox::warning(this, "Saving Agents",
            "ERROR! Unable to save the set of agents at:\n"
            + path + "\nPlease, make sure this directory is writable.");
    }
}

void AgentsWidget::fill(AgentsGenerator* ag)
{
    if (!ag) {
        return;
    }

    AGFromFile* agff = dynamic_cast<AGFromFile*>(ag);
    if (agff) {
        m_ui->filepath->setText(agff->filePath());
        return;
    }

    AGSameFuncForAll* agsame = dynamic_cast<AGSameFuncForAll*>(ag);
    if (agsame) {
        m_ui->numAgents1->setValue(agsame->numAgents());
        m_ui->func->setCurrentIndex(m_ui->func->findData(agsame->function()));
        m_ui->fseed->setValue(agsame->functionInput().toInt);
        return;
    }

    AGDiffFunctions* agdiff = dynamic_cast<AGDiffFunctions*>(ag);
    if (agdiff) {
        m_ui->numAgents2->setValue(agdiff->numAgents());
        int row = 0;
        for (const AGDiffFunctions::AttrCmd ac : agdiff->attrCmds()) {
            Q_ASSERT(m_ui->table->item(row, 0)->text() == ac.attrName);
            QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->table->cellWidget(row, 1));
            cb->setCurrentIndex(cb->findData(ac.func));
            if (ac.func == AgentsGenerator::F_Rand || ac.func == AgentsGenerator::F_Value) {
                dynamic_cast<QLineEdit*>(m_ui->table->cellWidget(row, 2))->setText(ac.funcInput.toQString());
            }
            ++row;
        }
    }
}

QString AgentsWidget::readCommand()
{
    QString command;
    if (m_ui->bFromFile->isChecked()) {
        if (!QFileInfo::exists(m_ui->filepath->text())) {
            QMessageBox::warning(this, "Agents from csv file...",
                "The file does not exist.\nPlease, check the file path!");
            return QString();
        }
        command = m_ui->filepath->text();
    } else if (m_ui->bSameData->isChecked()) {
        command = QString("*%1;%2").arg(m_ui->numAgents1->text()).arg(m_ui->func->currentText());
        if (m_ui->func->currentData() == AgentsGenerator::F_Rand) {
            command += QString("_%1").arg(m_ui->fseed->value());
        }
    } else if (m_ui->bDiffData->isChecked()) {
        command = QString("#%1").arg(m_ui->numAgents2->text());
        int row = 0;
        for (const ValueSpace* vs : m_agentAttrsSpace) {
            Q_ASSERT(m_ui->table->item(row, 0)->text() == vs->attrName());
            QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->table->cellWidget(row, 1));
            command += QString(";%1_%2").arg(vs->attrName()).arg(cb->currentText());

            QString valStr = dynamic_cast<QLineEdit*>(m_ui->table->cellWidget(row, 2))->text();
            if (cb->currentData() == AgentsGenerator::F_Rand) {
                bool isInt = false;
                valStr.toInt(&isInt);
                if (!isInt) {
                    QMessageBox::warning(this, "Agents Generator",
                        "The PRG seed for '" + vs->attrName() + "' should be an integer!\n");
                    return QString();
                }
                command += "_" + valStr;
            } else if (cb->currentData() == AgentsGenerator::F_Value) {
                if (!vs->validate(valStr).isValid()) {
                    QMessageBox::warning(this, "Agents Generator",
                        "The value of '" + vs->attrName() + "' is invalid!\n"
                         "Expected: " + vs->space());
                    return QString();
                }
                command += "_" + valStr;
            }
            ++row;
        }
    } else {
        qFatal("[AgentsWidget::slotOk()]");
    }

    return command;
}

} // evoplex
