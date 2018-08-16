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

#include <functional>

#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>

#include "core/include/abstractgraph.h"
#include "core/include/enum.h"
#include "core/include/nodes.h"
#include "core/nodes_p.h"

#include "attrsgendlg.h"
#include "ui_attrsgendlg.h"
#include "maingui.h"

namespace evoplex {

AttrsGenDlg::AttrsGenDlg(QWidget* parent, Mode mode,
                         const AttributesScope& attrsScope, const QString& cmd)
    : QDialog(parent, MainGUI::kDefaultDlgFlags),
      m_ui(new Ui_AttrsGenDlg),
      m_mode(mode),
      m_attrsScope(attrsScope)
{
    setWindowModality(Qt::ApplicationModal);
    m_ui->setupUi(this);

    m_ui->wFromFile->setVisible(false);
    m_ui->wSameData->setVisible(false);
    m_ui->wDiffData->setVisible(false);
    m_ui->wNumNodes->setVisible(false);

    if (m_mode == Mode::Edges) {
        setupForEdges();
    } else {
        setupForNodes();
    }

    connect(m_ui->cancel, SIGNAL(pressed()), SLOT(reject()));
    connect(m_ui->ok, SIGNAL(pressed()), SLOT(accept()));

    const auto MIN = static_cast<unsigned char>(Function::Min);
    const auto MAX = static_cast<unsigned char>(Function::Max);
    const auto RAND = static_cast<unsigned char>(Function::Rand);
    const auto VALUE = static_cast<unsigned char>(Function::Value);

    auto slotHideRand = [this]() {
        bool notRand = m_ui->func->currentData() != static_cast<unsigned char>(Function::Rand);
        m_ui->lseed->setHidden(notRand);
        m_ui->fseed->setHidden(notRand);
    };
    connect(m_ui->func, &QComboBox::currentTextChanged, slotHideRand);
    m_ui->func->insertItem(0, _enumToString<Function>(Function::Min), MIN);
    m_ui->func->insertItem(1, _enumToString<Function>(Function::Max), MAX);
    m_ui->func->insertItem(2, _enumToString<Function>(Function::Rand), RAND);
    m_ui->func->setCurrentIndex(0);
    slotHideRand();

    m_ui->table->setRowCount(m_attrsScope.size());
    for (auto const& ar : m_attrsScope) {
        m_ui->table->setItem(ar->id(), 0, new QTableWidgetItem(ar->attrName()));

        QComboBox* cb = new QComboBox(m_ui->table);
        cb->insertItem(0, _enumToString<Function>(Function::Min), MIN);
        cb->insertItem(1, _enumToString<Function>(Function::Max), MAX);
        cb->insertItem(2, _enumToString<Function>(Function::Rand), RAND);
        cb->insertItem(3, _enumToString<Function>(Function::Value), VALUE);
        m_ui->table->setCellWidget(ar->id(), 1, cb);

        QLineEdit* le = new QLineEdit(m_ui->table);
        le->setAlignment(Qt::AlignHCenter);
        auto slotInput = [cb, le, ar]() {
            Function f = static_cast<Function>(cb->currentData().toInt());
            QString tt;
            if (f == Function::Rand) {
                le->setFocus();
                tt = "Type the PRG seed (integer).";
            } else if (f == Function::Value) {
                le->setFocus();
                tt = "Type a valid value for this attribute.\n"
                     "Expected: " + ar->attrRangeStr();
            }
            le->setToolTip(tt);
            le->setDisabled(tt.isEmpty());
        };
        connect(cb, &QComboBox::currentTextChanged, slotInput);
        m_ui->table->setCellWidget(ar->id(), 2, le);
        cb->setCurrentIndex(0);
        slotInput();
    }

    resize(width(), 250);
    fill(cmd);
}

void AttrsGenDlg::setupForEdges()
{
    Q_ASSERT_X(!m_attrsScope.empty(), "AttrsGenDlg",
        "cannot open the AttrsGenDlg for edges with an empty attrsScope");

    setWindowTitle("Edges Generator");

    connect(m_ui->bSameData, SIGNAL(toggled(bool)), m_ui->wSameData, SLOT(setVisible(bool)));
    connect(m_ui->bDiffData, SIGNAL(toggled(bool)), m_ui->wDiffData, SLOT(setVisible(bool)));

    m_ui->saveAs->setVisible(false);
    m_ui->bFromFile->setVisible(false);
    m_ui->bCreateNodes->setVisible(false);
    m_ui->bSameData->setChecked(true);
}

void AttrsGenDlg::setupForNodes()
{
    setWindowTitle("Nodes Generator");

    connect(m_ui->saveAs, SIGNAL(pressed()), SLOT(slotSaveAs()));

    connect(m_ui->bFromFile, SIGNAL(toggled(bool)), m_ui->wFromFile, SLOT(setVisible(bool)));
    connect(m_ui->browseFile, &QPushButton::pressed, [this]() {
        QString path = QFileDialog::getOpenFileName(this, "Initial Population",
                m_ui->filepath->text(), "Text Files (*.csv *.txt)");
        if (!path.isEmpty()) {
            m_ui->filepath->setText(path);
        }
    });

    if (m_attrsScope.empty()) {
        connect(m_ui->bCreateNodes, SIGNAL(toggled(bool)), m_ui->wNumNodes, SLOT(setVisible(bool)));
        m_ui->bSameData->setVisible(false);
        m_ui->bDiffData->setVisible(false);
        m_ui->bCreateNodes->setChecked(true);
    } else {
        connect(m_ui->bSameData, SIGNAL(toggled(bool)), m_ui->wSameData, SLOT(setVisible(bool)));
        connect(m_ui->bSameData, SIGNAL(toggled(bool)), m_ui->wNumNodes, SLOT(setVisible(bool)));
        connect(m_ui->bDiffData, SIGNAL(toggled(bool)), m_ui->wDiffData, SLOT(setVisible(bool)));
        connect(m_ui->bDiffData, SIGNAL(toggled(bool)), m_ui->wNumNodes, SLOT(setVisible(bool)));
        m_ui->bCreateNodes->setVisible(false);
        m_ui->bSameData->setChecked(true);
    }
}

AttrsGenDlg::~AttrsGenDlg()
{
    delete m_ui;
}

void AttrsGenDlg::slotSaveAs()
{
    if (m_mode == Mode::Edges) {
        return;
    }

    QString path = QFileDialog::getSaveFileName(this,
            "Save Nodes", m_ui->filepath->text(), "Text Files (*.csv)");

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

        const int numNodes = m_ui->numNodes->value();
        QProgressDialog progressDlg("Saving file", QString(), 0, 2 * numNodes, this);
        progressDlg.setWindowModality(Qt::WindowModal);
        progressDlg.setValue(0);

        int pValue = 0;
        std::function<void(int)> progress = [&progressDlg, &pValue](int p) { progressDlg.setValue(pValue + p); };

        QString errMsg;
        Nodes nodes = NodesPrivate::fromCmd(cmd, m_attrsScope, GraphType::Undirected, errMsg, progress);
        Q_ASSERT_X(errMsg.isEmpty(), "AttrsGenDlg", "the command should be free of erros here");
        Q_ASSERT_X(!nodes.empty(), "AttrsGenDlg", "nodes size must be >0");

        pValue = numNodes;
        saved = NodesPrivate::saveToFile(nodes, path, progress);
    }

    if (saved) {
        QMessageBox::information(this, "Saving file",
                "The set of nodes was saved successfully!\n" + path);
    } else {
        QMessageBox::warning(this, "Saving file",
                "ERROR! Unable to save the set of nodes at:\n"
                + path + "\nPlease, make sure this directory is writable.");
    }
}

void AttrsGenDlg::fill(const QString& cmd)
{
    if (cmd.isEmpty()) {
        return;
    }

    if (m_mode == Mode::Nodes && QFileInfo::exists(cmd)) {
        m_ui->bFromFile->setChecked(true);
        m_ui->filepath->setText(cmd);
        return;
    }

    QString errMsg;
    auto ag = AttrsGenerator::parse(m_attrsScope, cmd, errMsg);
    if (!errMsg.isEmpty() || !ag) {
        QMessageBox::warning(parentWidget(), "Attributes Generator", errMsg);
        return;
    }

    AGSameFuncForAll* agsame = dynamic_cast<AGSameFuncForAll*>(ag.get());
    if (agsame) {
        m_ui->bSameData->setChecked(!m_attrsScope.empty());
        m_ui->bCreateNodes->setChecked(m_attrsScope.empty());
        m_ui->numNodes->setValue(agsame->size());
        m_ui->func->setCurrentIndex(m_ui->func->findData(static_cast<int>(agsame->function())));
        const Value& v = agsame->functionInput();
        m_ui->fseed->setValue(v.type() == Value::INT ? v.toInt() : 0);
        return;
    }

    AGDiffFunctions* agdiff = dynamic_cast<AGDiffFunctions*>(ag.get());
    if (agdiff) {
        m_ui->bDiffData->setChecked(true);
        m_ui->numNodes->setValue(agdiff->size());
        for (const AGDiffFunctions::AttrCmd& ac : agdiff->attrCmds()) {
            Q_ASSERT_X(m_ui->table->item(ac.attrId, 0)->text() == ac.attrName,
                       "AttrsGenDlg::fill", "attribute name mismatch. It should never happen!");
            QComboBox* cb = qobject_cast<QComboBox*>(m_ui->table->cellWidget(ac.attrId, 1));
            QLineEdit* le = qobject_cast<QLineEdit*>(m_ui->table->cellWidget(ac.attrId, 2));
            cb->setCurrentIndex(cb->findData(static_cast<int>(ac.func)));
            if (ac.func == Function::Rand || ac.func == Function::Value) {
                le->setText(ac.funcInput.toQString());
                le->setHidden(false);
            } else {
                le->setText("");
                le->setHidden(true);
            }
        }
    }
}

QString AttrsGenDlg::readCommand()
{
    QString command;
    if (m_ui->bFromFile->isChecked()) {
        if (!QFileInfo::exists(m_ui->filepath->text())) {
            QMessageBox::warning(this, "Attributes from csv file...",
                "The file does not exist.\nPlease, check the file path!");
            return QString();
        }
        command = m_ui->filepath->text();
    } else if (m_ui->bCreateNodes->isChecked()) {
        command = m_ui->numNodes->text();
    } else if (m_ui->bSameData->isChecked()) {
        command = readCommand_sameData();
    } else if (m_ui->bDiffData->isChecked()) {
        command = readCommand_diffData();
    } else {
        qFatal("invalid command!");
    }

    return command;
}

QString AttrsGenDlg::readCommand_diffData() const
{
    QString cmd("#");
    if (m_mode == Mode::Nodes) {
        cmd += m_ui->numNodes->text();
    }

    for (auto const& ar : m_attrsScope) {
        Q_ASSERT_X(m_ui->table->item(ar->id(), 0)->text() == ar->attrName(),
                   "AttrsGenDlg", "attribute name mismatch. It should never happen!");

        QComboBox* cb = qobject_cast<QComboBox*>(m_ui->table->cellWidget(ar->id(), 1));
        cmd += QString(";%1_%2").arg(ar->attrName(), cb->currentText());

        QString valStr = qobject_cast<QLineEdit*>(m_ui->table->cellWidget(ar->id(), 2))->text();
        Function f = static_cast<Function>(cb->currentData().toInt());
        if (f == Function::Rand) {
            Value seed = AttrsGenerator::parseRandSeed("rand_" + valStr);
            if (!seed.isValid()) {
                QMessageBox::warning(parentWidget(), "Attributes Generator",
                        "The PRG seed for '" + ar->attrName() +
                        "' should be a positive integer!\n");
                return QString();
            }
            cmd += "_" + valStr;
        } else if (f == Function::Value) {
            if (!ar->validate(valStr).isValid()) {
                QMessageBox::warning(parentWidget(), "Attributes Generator",
                        "The value of '" + ar->attrName() + "' is invalid!\n"
                        "Expected: " + ar->attrRangeStr());
                return QString();
            }
            cmd += "_" + valStr;
        }
    }

    return cmd.replace("#;", "#");
}

QString AttrsGenDlg::readCommand_sameData() const
{
    QString cmd("*");
    if (m_mode == Mode::Nodes) {
        cmd += m_ui->numNodes->text() + ";";
    }
    cmd += m_ui->func->currentText();

    if (m_ui->func->currentData() == static_cast<int>(Function::Rand)) {
        cmd += QString("_%1").arg(m_ui->fseed->value());
    }

    return cmd;
}


} // evoplex
