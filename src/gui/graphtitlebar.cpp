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

#include "core/nodes_p.h"
#include "core/project.h"
#include "core/trial.h"

#include "graphtitlebar.h"
#include "ui_graphtitlebar.h"

namespace evoplex {

GraphTitleBar::GraphTitleBar(const Experiment* exp, QDockWidget* parent)
    : BaseTitleBar(parent),
      m_ui(new Ui_GraphTitleBar),
      m_exp(exp),
      m_bExportNodes(new QtMaterialIconButton(QIcon(":/icons/material/table_white_18"), this)),
      m_bSettings(new QtMaterialIconButton(QIcon(":/icons/material/settings_white_18"), this))
{
    m_ui->setupUi(this);

    m_bExportNodes->setToolTip("export nodes to file");
    m_bExportNodes->setIconSize(QSize(22,18));
    m_bExportNodes->setColor(m_iconColor);
    m_ui->btns->addWidget(m_bExportNodes);
    connect(m_bExportNodes, SIGNAL(clicked(bool)), SLOT(slotExporNodes()));

    m_bSettings->setToolTip("graph settings");
    m_bSettings->setIconSize(QSize(22,18));
    m_bSettings->setColor(m_iconColor);
    m_ui->btns->addWidget(m_bSettings);
    connect(m_bSettings, SIGNAL(clicked(bool)), SIGNAL(openSettingsDlg()));

    connect(m_ui->cbTrial, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [this](int t) {
            Q_ASSERT(t >= 0 && t < UINT16_MAX);
            emit(trialSelected(static_cast<quint16>(t)));
        });

    connect(m_exp, SIGNAL(restarted()), SLOT(slotRestarted()));
    slotRestarted(); // init

    init(qobject_cast<QHBoxLayout*>(layout()));
}

GraphTitleBar::~GraphTitleBar()
{
    delete m_ui;
}

void GraphTitleBar::slotExporNodes()
{
    if (m_exp->expStatus() == Status::Disabled ||
            m_exp->expStatus() == Status::Invalid) {
        QMessageBox::warning(this, "Exporting nodes",
                "This experiment is invalid or has not been initialized yet.\n"
                "Please, initialize the experiment and try again.");
        return;
    } else if (m_exp->expStatus() == Status::Running ||
                    m_exp->expStatus() == Status::Queued) {
        QMessageBox::warning(this, "Exporting nodes",
                "Please, pause the experiment and try again.");
        return;
    }

    const quint16 currTrial = m_ui->cbTrial->currentText().toUShort();
    auto trial = m_exp->trial(currTrial);
    if (!trial || trial->graph()->nodes().empty()) {
        QMessageBox::warning(this, "Exporting nodes",
                "Could not export the set of nodes.\n"
                "Please, make sure this experiment is valid and that "
                "there are nodes to be exported!");
        return;
    }

    QString path = m_exp->project()->filepath();
    path = QFileDialog::getSaveFileName(this, "Export Nodes",
            path.isEmpty() ? QDir::homePath() : path, "Text Files (*.csv)");
    if (path.isEmpty()) {
        return;
    }

    QProgressDialog progressDlg("Exporting nodes", QString(), 0, trial->graph()->numNodes(), this);
    progressDlg.setWindowModality(Qt::WindowModal);
    progressDlg.setValue(0);
    std::function<void(int)> progress = [&progressDlg](int p) { progressDlg.setValue(p); };

    if (NodesPrivate::saveToFile(trial->graph()->nodes(), path, progress)) {
        QMessageBox::information(this, "Exporting nodes",
                "The set of nodes was saved successfully!\n" + path);
    } else {
        QMessageBox::warning(this, "Exporting nodes",
                "ERROR! Unable to save the set of nodes at:\n"
                + path + "\nPlease, make sure this directory is writable.");
    }
}

void GraphTitleBar::slotRestarted()
{
    const quint16 currTrial = m_ui->cbTrial->currentText().toUShort();
    m_ui->cbTrial->blockSignals(true);
    m_ui->cbTrial->clear();
    for (quint16 trialId = 0; trialId < m_exp->numTrials(); ++trialId) {
        m_ui->cbTrial->insertItem(trialId, QString::number(trialId));
    }
    m_ui->cbTrial->setCurrentText(QString::number(currTrial)); // try to keep the same id
    m_ui->cbTrial->blockSignals(false);

    const quint16 _currTrial = m_ui->cbTrial->currentText().toUShort();
    if (currTrial != _currTrial) {
        emit(trialSelected(_currTrial));
    }
}

} // evoplex
