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
#include <QDebug>
#include <QDesktopWidget>
#include <QPainter>
#include <QStyleOption>

#include "fontstyles.h"
#include "titlebar.h"
#include "ui_titlebar.h"

namespace evoplex {

BaseTitleBar::BaseTitleBar(QDockWidget* parent)
    : QWidget(parent),
      m_parent(parent),
      m_floatingBtn(nullptr),
      m_kIconFull(":/icons/material/fullscreen_white_18"),
      m_kIconFullExit(":/icons/material/fullscreen_exit_white_18"),
      m_kIconDetach(":/icons/material/detach_white_18"),
      m_kIconAttach(":/icons/material/attach_white_18")
{
    setFocusPolicy(Qt::StrongFocus);
}

void BaseTitleBar::init(QPushButton* floating, QPushButton* maximize, QPushButton* close)
{
    m_floatingBtn = floating;
    m_maximizeBtn = maximize;
    m_closeBtn = close;

    if (m_floatingBtn) {
        m_floatingBtn->setIcon(m_parent->isFloating()
                               ? m_kIconAttach : m_kIconDetach);

        connect(m_floatingBtn, &QPushButton::clicked,
            [this]() { m_parent->setFloating(!m_parent->isFloating()); });

        connect(m_parent, SIGNAL(topLevelChanged(bool)),
                SLOT(slotFloating(bool)));
    }

    if (m_maximizeBtn) {
        m_maximizeBtn->setIcon(m_kIconFull);
        connect(m_maximizeBtn, SIGNAL(clicked(bool)), SLOT(slotFullScreen()));
    }

    if (m_closeBtn) {
        if (m_parent->features().testFlag(QDockWidget::DockWidgetClosable)) {
            connect(m_closeBtn, SIGNAL(clicked(bool)), m_parent, SLOT(close()));
            m_closeBtn->setHidden(false);
        } else {
            m_closeBtn->setDisabled(true);
            m_closeBtn->setHidden(true);
        }
    }
}

void BaseTitleBar::paintEvent(QPaintEvent* pe)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    QWidget::paintEvent(pe);
}

void BaseTitleBar::slotFloating(bool floating)
{
    if (floating) {
        m_floatingBtn->setIcon(m_kIconAttach);
        m_floatingBtn->setToolTip("attach");
    } else {
        m_floatingBtn->setIcon(m_kIconDetach);
        m_floatingBtn->setToolTip("detach");
        m_maximizeBtn->setIcon(m_kIconFull);
    }
}

void BaseTitleBar::slotFullScreen()
{
    m_parent->setFloating(true);
    auto g = QApplication::desktop()->availableGeometry(m_parent);
    if (m_parent->frameGeometry().size() == g.size()) {
        m_parent->setGeometry(m_parent->parentWidget()->frameGeometry());
        m_maximizeBtn->setIcon(m_kIconFull);
    } else {
        m_parent->setGeometry(g);
        m_maximizeBtn->setIcon(m_kIconFullExit);
    }
}

/***********************************/

TitleBar::TitleBar(QDockWidget* parent)
    : BaseTitleBar(parent),
      m_ui(new Ui_TitleBar),
      m_iconSize(24, 24)
{
    m_ui->setupUi(this);

    QFont f = FontStyles::subtitle2();
    f.setPixelSize(12);
    m_ui->lSubtitle->setFont(FontStyles::caption());
    m_ui->lTitle->setFont(FontStyles::subtitle1());
    m_ui->lTitle->setText(parent->objectName());

    init(m_ui->bFloat, m_ui->bMaximize, m_ui->bClose);
}

TitleBar::~TitleBar()
{
    delete m_ui;
}

void TitleBar::addButton(QPushButton* btn, const QString& iconPath, QString toolTip)
{
    btn->setFlat(true);
    btn->setIconSize(m_iconSize);
    btn->setMaximumSize(m_iconSize);
    btn->setMinimumSize(m_iconSize);
    btn->setIcon(QIcon(iconPath));
    btn->setToolTip(toolTip);
    m_ui->btnLayout->addWidget(btn);
}

void TitleBar::setTitle(const QString& t)
{
    m_ui->lTitle->setText(t);
}

void TitleBar::setSubtitle(const QString& t)
{
    m_ui->lSubtitle->setText(t);
}

} // evoplex
