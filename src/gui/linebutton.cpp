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

#include <QHBoxLayout>
#include <QFileDialog>

#include "linebutton.h"

LineButton::LineButton(QWidget* parent, Mode mode)
    : QWidget(parent),
      m_mode(mode),
      m_line(new QLineEdit(this)),
      m_button(new QPushButton(this))
{
    m_fileType = "Text Files (*.csv *.txt)";

    m_button->setText("...");
    m_button->setMaximumWidth(20);

    QHBoxLayout* hlayout = new QHBoxLayout();
    hlayout->setMargin(0);
    hlayout->insertWidget(0, m_line);
    hlayout->insertWidget(1, m_button);

    setLayout(hlayout);

    if (m_mode == SelectDir) {
        connect(m_button, SIGNAL(pressed()), SLOT(slotSelectDir()));
    } else if (m_mode == SelectTextFile) {
        connect(m_button, SIGNAL(pressed()), SLOT(slotSelectFile()));
    }
}

void LineButton::setReadOnly(bool r)
{
    m_line->setReadOnly(r);
    m_button->setDisabled(r);
}

void LineButton::slotSelectDir()
{
    QString path = QFileDialog::getExistingDirectory(this,
            "Select a Directory", m_line->text());
    if (!path.isEmpty()) {
        m_line->setText(path);
    }
}

void LineButton::slotSelectFile()
{
    QString path = QFileDialog::getOpenFileName(this,
            "Select a File", m_line->text(), m_fileType);
    if (!path.isEmpty()) {
        m_line->setText(path);
    }
}
