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

#ifndef ATTRWIDGET_H
#define ATTRWIDGET_H

#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QWidget>

#include "core/include/attributerange.h"

namespace evoplex {
class AttrWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AttrWidget(AttributeRangePtr attrRange, QWidget* parent, QWidget* customWidget=nullptr);

    // set the current content of the field
    // it does not check if the value matches the attrRange
    void setValue(const Value& value);

    // get the current value
    // it does not check if the value matches the attrRange
    Value value() const;

    // Return the current value if it is within the attrRange
    // Otherwise, it returns a Value()
    Value validate() const;

    inline int id() const { return m_attrRange->id(); }
    inline const QString& attrName() const { return m_attrRange->attrName(); }
    inline QWidget* widget() const { return m_widget; }

signals:
    void editingFinished();

private:
    bool m_useCustomWidget;
    QWidget* m_widget;
    AttributeRangePtr m_attrRange;

    QWidget* newWidget(AttributeRangePtr attrRange);
};
} // evoplex
#endif // ATTRWIDGET_H
