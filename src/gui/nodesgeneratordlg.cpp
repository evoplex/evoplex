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

#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>
#include <functional>

#include "nodesgeneratordlg.h"
#include "ui_nodesgeneratordlg.h"
#include "node.h"

namespace evoplex
{

NodesGeneratorDlg::NodesGeneratorDlg(const AttributesScope& nodeAttrsScope, QWidget* parent, NodesGenerator* ag)
    : QDialog(parent)
    , m_ui(new Ui_NodesGeneratorDlg)
    , m_nodeAttrsScope(nodeAttrsScope)
{
    setWindowModality(Qt::ApplicationModal);
    m_ui->setupUi(this);

    connect(m_ui->bFromFile, SIGNAL(toggled(bool)), m_ui->wFromFile, SLOT(setVisible(bool)));
    connect(m_ui->bSameData, SIGNAL(toggled(bool)), m_ui->wSameData, SLOT(setVisible(bool)));
    connect(m_ui->bDiffData, SIGNAL(toggled(bool)), m_ui->wDiffData, SLOT(setVisible(bool)));
    m_ui->wFromFile->setVisible(false);
    m_ui->wSameData->setVisible(false);
    m_ui->wDiffData->setVisible(false);
    m_ui->bSameData->setChecked(true);

    connect(m_ui->saveAs, SIGNAL(pressed()), SLOT(slotSaveAs()));
    connect(m_ui->cancel, SIGNAL(pressed()), SLOT(reject()));
    connect(m_ui->ok, SIGNAL(pressed()), SLOT(accept()));

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
        bool notRand = m_ui->func->currentData() != NodesGenerator::F_Rand;
        m_ui->lseed->setHidden(notRand);
        m_ui->fseed->setHidden(notRand);
    });
    m_ui->func->insertItem(0, NodesGenerator::enumToString(NodesGenerator::F_Min), NodesGenerator::F_Min);
    m_ui->func->insertItem(1, NodesGenerator::enumToString(NodesGenerator::F_Max), NodesGenerator::F_Max);
    m_ui->func->insertItem(2, NodesGenerator::enumToString(NodesGenerator::F_Rand), NodesGenerator::F_Rand);
    m_ui->func->setCurrentIndex(0);

    m_ui->table->setRowCount(m_nodeAttrsScope.size());
    for (const AttributeRange* ar : m_nodeAttrsScope) {
        m_ui->table->setItem(ar->id(), 0, new QTableWidgetItem(ar->attrName()));

        QComboBox* cb = new QComboBox(m_ui->table);
        cb->insertItem(0, NodesGenerator::enumToString(NodesGenerator::F_Min), NodesGenerator::F_Min);
        cb->insertItem(1, NodesGenerator::enumToString(NodesGenerator::F_Max), NodesGenerator::F_Max);
        cb->insertItem(2, NodesGenerator::enumToString(NodesGenerator::F_Rand), NodesGenerator::F_Rand);
        cb->insertItem(3, NodesGenerator::enumToString(NodesGenerator::F_Value), NodesGenerator::F_Value);
        m_ui->table->setCellWidget(ar->id(), 1, cb);

        QLineEdit* le = new QLineEdit();
        connect(cb, &QComboBox::currentTextChanged, [cb, le, ar](){
            if (cb->currentData() == NodesGenerator::F_Min
                    || cb->currentData() == NodesGenerator::F_Max) {
                le->setHidden(true);
                return;
            } else if (cb->currentData() == NodesGenerator::F_Rand) {
                le->setToolTip("Type the PRG seed (integer).");
                le->setFocus();
            } else if (cb->currentData() == NodesGenerator::F_Value) {
                le->setToolTip("Type a valid value for this attribute.\n"
                               "Expected: " + ar->attrRangeStr());
                le->setFocus();
            } else {
                Q_ASSERT(false);
            }
            le->setHidden(false);
        });
        m_ui->table->setCellWidget(ar->id(), 2, le);
        le->setHidden(true);
    }

    resize(width(), 250);
    fill(ag);
}

NodesGeneratorDlg::~NodesGeneratorDlg()
{
    delete m_ui;
}

void NodesGeneratorDlg::slotSaveAs()
{
    QString path = QFileDialog::getSaveFileName(this,
                                                "Save Nodes",
                                                m_ui->filepath->text(),
                                                "Text Files (*.csv)");
    if (path.isEmpty()) {
        return;
    }

    bool saved = false;
    if (m_ui->bFromFile->isChecked()) {
        saved = QFile::copy(m_ui->filepath->text(), path);
    } else {
        QString cmd = readCommand();
        if (cmd.isEmpty()) {
            return;
        }

        int numNodes = 0;
        if (m_ui->bSameData->isChecked()) {
            numNodes = m_ui->numNodes1->value();
        } else {
            numNodes = m_ui->numNodes2->value();
        }

        QProgressDialog progressDlg("Exporting Nodes...", QString(), 0, 2 * numNodes, this);
        progressDlg.setWindowModality(Qt::WindowModal);
        progressDlg.setValue(0);

        QString errMsg;
        NodesGenerator* ag = NodesGenerator::parse(m_nodeAttrsScope, cmd, errMsg);
        Q_ASSERT(errMsg.isEmpty());

        int pValue = 0;
        std::function<void(int)> progress = [&progressDlg, &pValue](int p) { progressDlg.setValue(pValue + p); };
        Nodes nodes = ag->create(progress);
        Q_ASSERT(nodes.size() > 0);

        pValue = numNodes;
        saved = NodesGenerator::saveToFile(path, nodes, progress);

        Utils::deleteAndShrink(nodes);
    }

    if (saved) {
        QMessageBox::information(this, "Exporting Nodes",
                "The set of nodes was saved successfully!\n" + path);
    } else {
        QMessageBox::warning(this, "Exporting Nodes",
                "ERROR! Unable to save the set of nodes at:\n"
                + path + "\nPlease, make sure this directory is writable.");
    }
}

void NodesGeneratorDlg::fill(NodesGenerator* ag)
{
    if (!ag) {
        return;
    }

    AGFromFile* agff = dynamic_cast<AGFromFile*>(ag);
    if (agff) {
        m_ui->bFromFile->setChecked(true);
        m_ui->filepath->setText(agff->filePath());
        return;
    }

    AGSameFuncForAll* agsame = dynamic_cast<AGSameFuncForAll*>(ag);
    if (agsame) {
        m_ui->bSameData->setChecked(true);
        m_ui->numNodes1->setValue(agsame->numNodes());
        m_ui->func->setCurrentIndex(m_ui->func->findData(agsame->function()));
        Value v = agsame->functionInput();
        m_ui->fseed->setValue(v.type() == Value::INT ? v.toInt() : 0);
        return;
    }

    AGDiffFunctions* agdiff = dynamic_cast<AGDiffFunctions*>(ag);
    if (agdiff) {
        m_ui->bDiffData->setChecked(true);
        m_ui->numNodes2->setValue(agdiff->numNodes());
        for (const AGDiffFunctions::AttrCmd ac : agdiff->attrCmds()) {
            Q_ASSERT(m_ui->table->item(ac.attrId, 0)->text() == ac.attrName);
            QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->table->cellWidget(ac.attrId, 1));
            cb->setCurrentIndex(cb->findData(ac.func));
            if (ac.func == NodesGenerator::F_Rand || ac.func == NodesGenerator::F_Value) {
                dynamic_cast<QLineEdit*>(m_ui->table->cellWidget(ac.attrId, 2))->setText(ac.funcInput.toQString());
            }
        }
    }
}

QString NodesGeneratorDlg::readCommand()
{
    QString command;
    if (m_ui->bFromFile->isChecked()) {
        if (!QFileInfo::exists(m_ui->filepath->text())) {
            QMessageBox::warning(this, "Nodes from csv file...",
                "The file does not exist.\nPlease, check the file path!");
            return QString();
        }
        command = m_ui->filepath->text();
    } else if (m_ui->bSameData->isChecked()) {
        command = QString("*%1;%2").arg(m_ui->numNodes1->text()).arg(m_ui->func->currentText());
        if (m_ui->func->currentData() == NodesGenerator::F_Rand) {
            command += QString("_%1").arg(m_ui->fseed->value());
        }
    } else if (m_ui->bDiffData->isChecked()) {
        command = QString("#%1").arg(m_ui->numNodes2->text());
        for (const AttributeRange* ar : m_nodeAttrsScope) {
            Q_ASSERT(m_ui->table->item(ar->id(), 0)->text() == ar->attrName());
            QComboBox* cb = dynamic_cast<QComboBox*>(m_ui->table->cellWidget(ar->id(), 1));
            command += QString(";%1_%2").arg(ar->attrName()).arg(cb->currentText());

            QString valStr = dynamic_cast<QLineEdit*>(m_ui->table->cellWidget(ar->id(), 2))->text();
            if (cb->currentData() == NodesGenerator::F_Rand) {
                bool isInt = false;
                valStr.toInt(&isInt);
                if (!isInt) {
                    QMessageBox::warning(this, "Nodes Generator",
                        "The PRG seed for '" + ar->attrName() + "' should be an integer!\n");
                    return QString();
                }
                command += "_" + valStr;
            } else if (cb->currentData() == NodesGenerator::F_Value) {
                if (!ar->validate(valStr).isValid()) {
                    QMessageBox::warning(this, "Nodes Generator",
                        "The value of '" + ar->attrName() + "' is invalid!\n"
                         "Expected: " + ar->attrRangeStr());
                    return QString();
                }
                command += "_" + valStr;
            }
        }
    } else {
        qFatal("[NodesGeneratorDlg::slotOk()]");
    }

    return command;
}

} // evoplex
