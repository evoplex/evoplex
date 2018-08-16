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

#ifndef LINEBUTTON_H
#define LINEBUTTON_H

#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class LineButton : public QWidget
{
    Q_OBJECT
public:
    enum Mode {
        None,
        SelectDir,
        SelectTextFile
    };

    explicit LineButton(QWidget* parent, Mode mode);

    inline QLineEdit* line() const { return m_line; }
    inline QPushButton* button() const { return m_button; }

    inline QString text() const { return m_line->text(); }
    inline void setText(const QString& t) const { return m_line->setText(t); }

public slots:
    void setReadOnly(bool r);
    void slotSelectDir();
    void slotSelectFile();

private:
    Mode m_mode;
    QLineEdit* m_line;
    QPushButton* m_button;
    QString m_fileType;
};

#endif // LINEBUTTON_H
