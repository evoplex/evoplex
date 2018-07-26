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

#include <QMessageBox>
#include <QPainter>

#include "titlebar.h"
#include "ui_titlebar.h"

namespace evoplex {

TitleBar::TitleBar(const Experiment* exp, QDockWidget* parent)
    : QWidget(parent),
      m_ui(new Ui_TitleBar),
      m_exp(exp)
{
    m_ui->setupUi(this);
    setFocusPolicy(Qt::StrongFocus);
    setStyleSheet("background-color:rgb(40,40,40);");

    connect(m_exp, SIGNAL(restarted()), SLOT(slotRestarted()));
    slotRestarted(); // init

    QStyle* style = QApplication::style();
    m_ui->bClose->setIcon(style->standardIcon(QStyle::SP_TitleBarCloseButton));
    m_ui->bFloat->setIcon(style->standardIcon(QStyle::SP_TitleBarNormalButton));
    connect(m_ui->bClose, SIGNAL(clicked(bool)), parent, SLOT(close()));
    connect(m_ui->bFloat, &QPushButton::clicked,
            [parent]() { parent->setFloating(!parent->isFloating()); });

    connect(m_ui->bSettings, SIGNAL(clicked(bool)), SIGNAL(openSettingsDlg()));

    connect(m_ui->cbTrial, QOverload<int>::of(&QComboBox::currentIndexChanged),
        [this](int t) {
            Q_ASSERT(t >= 0 && t < UINT16_MAX);
            emit(trialSelected(static_cast<quint16>(t)));
        }
    );
}

TitleBar::~TitleBar()
{
    delete m_ui;
}

void TitleBar::paintEvent(QPaintEvent* pe)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(pe);
}

void TitleBar::slotRestarted()
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
