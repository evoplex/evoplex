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
      m_iconColor(Qt::white),
      m_kIconFull(":/icons/material/fullscreen_white_18"),
      m_kIconFullExit(":/icons/material/fullscreen_exit_white_18"),
      m_kIconDetach(":/icons/material/detach_white_18"),
      m_kIconAttach(":/icons/material/attach_white_18"),
      m_parent(parent),
      m_bFloat(new QtMaterialIconButton(m_kIconDetach)),
      m_bMaximize(new QtMaterialIconButton(m_kIconFull)),
      m_bClose(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);

    m_bFloat->setColor(m_iconColor);
    m_bFloat->setIconSize(QSize(18,18));
    m_bMaximize->setColor(m_iconColor);
    m_bMaximize->setIconSize(QSize(18,18));
}

BaseTitleBar::~BaseTitleBar()
{
    delete m_bFloat;
    delete m_bMaximize;
    delete m_bClose;
}

void BaseTitleBar::init(QHBoxLayout* layout)
{
    layout->addWidget(m_bFloat);
    m_bFloat->setIcon(m_parent->isFloating()
                      ? m_kIconAttach : m_kIconDetach);
    connect(m_bFloat, &QtMaterialIconButton::clicked,
            [this]() { m_parent->setFloating(!m_parent->isFloating()); });
    connect(m_parent, SIGNAL(topLevelChanged(bool)),
            SLOT(slotFloating(bool)));

    layout->addWidget(m_bMaximize);
    m_bMaximize->setIcon(m_kIconFull);
    connect(m_bMaximize, SIGNAL(pressed()), SLOT(slotFullScreen()));

    if (m_parent->features().testFlag(QDockWidget::DockWidgetClosable)) {
        m_bClose = new QtMaterialIconButton(QIcon(":/icons/material/close_white_18"));
        m_bClose->setColor(Qt::white);
        m_bClose->setIconSize(QSize(18,18));
        layout->addWidget(m_bClose);
        connect(m_bClose, SIGNAL(pressed()), m_parent, SLOT(close()));
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
        m_bFloat->setIcon(m_kIconAttach);
        m_bFloat->setToolTip("attach");
    } else {
        m_bFloat->setIcon(m_kIconDetach);
        m_bFloat->setToolTip("detach");
        m_bMaximize->setIcon(m_kIconFull);
    }
}

void BaseTitleBar::slotFullScreen()
{
    m_parent->setFloating(true);
    auto g = QApplication::desktop()->availableGeometry(m_parent);
    if (m_parent->frameGeometry().size() == g.size()) {
        m_parent->setGeometry(m_parent->parentWidget()->frameGeometry());
        m_bMaximize->setIcon(m_kIconFull);
    } else {
        m_parent->setGeometry(g);
        m_bMaximize->setIcon(m_kIconFullExit);
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
    m_ui->lTitle->setText(parent->windowTitle());
    connect(parent, SIGNAL(windowTitleChanged(QString)), SLOT(setTitle(QString)));

    init(m_ui->top);
}

TitleBar::~TitleBar()
{
    delete m_ui;
}

void TitleBar::addButton(QtMaterialIconButton* btn, QString toolTip)
{
    btn->setIconSize(m_iconSize);
    btn->setMaximumSize(m_iconSize * 1.25);
    btn->setMinimumSize(m_iconSize * 1.25);
    btn->setColor(m_iconColor);
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
