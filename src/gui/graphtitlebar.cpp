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

#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QPainter>

#include "graphtitlebar.h"
#include "ui_graphtitlebar.h"

namespace evoplex {

GraphTitleBar::GraphTitleBar(const Experiment* exp, QDockWidget* parent)
    : QWidget(parent),
      m_parent(parent),
      m_ui(new Ui_GraphTitleBar),
      m_exp(exp),
      m_kIconFull(":/icons/material/fullscreen_white_18"),
      m_kIconFullExit(":/icons/material/fullscreen_exit_white_18"),
      m_kIconDetach(":/icons/material/detach_white_18"),
      m_kIconAttach(":/icons/material/attach_white_18")
{
    m_ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);

    m_ui->bFloat->setIcon(parent->isFloating() ? m_kIconAttach : m_kIconDetach);
    auto g = QApplication::desktop()->availableGeometry(parent);
    m_ui->bMaximize->setIcon(g.size() == parent->geometry().size()
                             ? m_kIconFull : m_kIconFullExit);

    connect(m_ui->bClose, SIGNAL(clicked(bool)), parent, SLOT(close()));
    connect(m_ui->bMaximize, SIGNAL(clicked(bool)), SLOT(slotFullScreen()));
    connect(m_ui->bFloat, &QPushButton::clicked,
        [parent]() { parent->setFloating(!parent->isFloating()); });
    connect(parent, SIGNAL(topLevelChanged(bool)), SLOT(slotFloating(bool)));

    connect(m_ui->bSettings, SIGNAL(clicked(bool)), SIGNAL(openSettingsDlg()));
    connect(m_ui->cbTrial, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [this](int t) {
            Q_ASSERT(t >= 0 && t < UINT16_MAX);
            emit(trialSelected(static_cast<quint16>(t)));
        });

    connect(m_exp, SIGNAL(restarted()), SLOT(slotRestarted()));
    slotRestarted(); // init
}

GraphTitleBar::~GraphTitleBar()
{
    delete m_ui;
}

void GraphTitleBar::paintEvent(QPaintEvent* pe)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(pe);
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

void GraphTitleBar::slotFloating(bool floating)
{
    if (floating) {
        m_ui->bFloat->setIcon(m_kIconAttach);
        m_ui->bFloat->setToolTip("attach");
    } else {
        m_ui->bFloat->setIcon(m_kIconDetach);
        m_ui->bFloat->setToolTip("detach");
    }
}

void GraphTitleBar::slotFullScreen()
{
    m_parent->setFloating(true);
    auto g = QApplication::desktop()->availableGeometry(m_parent);
    if (g.size() == m_parent->geometry().size()) {
        m_parent->setGeometry(m_parent->parentWidget()->geometry());
        m_ui->bMaximize->setIcon(m_kIconFull);
    } else {
        m_parent->setGeometry(g);
        m_ui->bMaximize->setIcon(m_kIconFullExit);
    }
}

} // evoplex
