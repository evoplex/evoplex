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

#include <QVBoxLayout>

#include "external/qt-material-widgets/qtmaterialiconbutton.h"

#include "core/logger.h"
#include "consolewidget.h"
#include "fontstyles.h"
#include "ui_consoletitlebar.h"

namespace evoplex {

ConsoleWidget::ConsoleWidget(QWidget *parent)
    : QDockWidget(parent),
      m_ui(new Ui_ConsoleTitleBar),
      m_warnings(0)
{
    setObjectName("ConsoleWidget");
    setWindowTitle("Messages");
    setFeatures(DockWidgetClosable);

    auto titlebar = new QWidget(this);
    titlebar->setFont(FontStyles::caption());
    setTitleBarWidget(titlebar);
    m_ui->setupUi(titlebar);
    m_ui->title->setText(windowTitle());

    auto bClear = new QtMaterialIconButton(QIcon(":/icons/material/clear_white_18"), this);
    bClear->setColor(Qt::white);
    bClear->setToolTip("clear messages");
    titlebar->layout()->addWidget(bClear);
    connect(bClear, &QtMaterialIconButton::pressed, [this]() {
        m_console->clear();
        m_warnings = 0;
        emit (warningsCountChanged(m_warnings));
    });

    auto bClose = new QtMaterialIconButton(QIcon(":/icons/material/close_white_18"), this);
    bClose->setToolTip("hide console");
    bClose->setColor(Qt::white);
    titlebar->layout()->addWidget(bClose);
    connect(bClose, SIGNAL(pressed()), SLOT(hide()));

    auto w = new QWidget(this);
    auto l = new QVBoxLayout;
    l->setContentsMargins(0,0,0,0);
    m_console = new QPlainTextEdit;
    m_console->setFrameShape(QFrame::NoFrame);
    m_console->setObjectName("Console");
    m_console->setReadOnly(true);
    l->addWidget(m_console);
    w->setLayout(l);
    setWidget(w);

    connect(Logger::instance(), &Logger::warning, this, [this](const QString& s) {
        m_console->appendPlainText(s);
        emit (warningsCountChanged(++m_warnings));
    }, Qt::QueuedConnection);
}

ConsoleWidget::~ConsoleWidget()
{
    delete m_ui;
}

} // evoplex
