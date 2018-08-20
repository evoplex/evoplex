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

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QDockWidget>
#include <QIcon>
#include <QPushButton>
#include <QWidget>

class Ui_TitleBar;

namespace evoplex {

class BaseTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit BaseTitleBar(QDockWidget *parent);

    void init(QPushButton* floating=nullptr,
              QPushButton* maximize=nullptr,
              QPushButton* close=nullptr);

protected:
    QDockWidget* m_parent;
    QPushButton* m_floatingBtn;
    QPushButton* m_maximizeBtn;
    QPushButton* m_closeBtn;

    const QIcon m_kIconFull;
    const QIcon m_kIconFullExit;
    const QIcon m_kIconDetach;
    const QIcon m_kIconAttach;

    // it makes the qt stylesheet work for this titlebar
    virtual void paintEvent(QPaintEvent* pe);

private slots:
    void slotFloating(bool floating);
    void slotFullScreen();
};

class TitleBar : public BaseTitleBar
{
    Q_OBJECT

public:
    explicit TitleBar(QDockWidget* parent);
    ~TitleBar();

    void addButton(QPushButton* btn, const QString& iconPath, QString toolTip="");

public slots:
    void setTitle(const QString& label);
    void setSubtitle(const QString& label);

private:
    Ui_TitleBar* m_ui;
    QSize m_iconSize;
};

} // evoplex
#endif // TITLEBAR_H
