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

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QDebug>

#include "external/qt-material-widgets/qtmaterialtoggle.h"

#include "attrwidget.h"
#include "linebutton.h"

namespace evoplex {
AttrWidget::AttrWidget(AttributeRangePtr attrRange, QWidget* parent, QWidget* customWidget)
    : QWidget(parent),
      m_useCustomWidget(customWidget),
      m_attrRange(attrRange)
{
    m_widget = customWidget ? customWidget : newWidget(attrRange);
    m_widget->setFocusPolicy(Qt::StrongFocus);
    m_widget->setSizePolicy(QSizePolicy::MinimumExpanding,
                            QSizePolicy::Preferred);

    auto l = new QHBoxLayout(this);
    l->addWidget(m_widget);
    l->setMargin(0);
    l->setSpacing(0);
    l->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    setLayout(l);

    // let's ensure that the AttrWidget is not filled with the main palette
    // background. It's particularly important when placing AttrWidgets in
    // a QTreeWidget; whithout this flag, they will use the system background
    // color when expanding/collapsing the tree, which causes a ugly flashing
    // effect.
    setAttribute(Qt::WA_NoSystemBackground);
}

AttrWidget::~AttrWidget()
{
    delete m_widget;
}

void AttrWidget::setReadOnly(bool r)
{
    if (m_isReadOnly != r) {
        emit (readOnlyChanged(r));
    }
    m_isReadOnly = r;
}

Value AttrWidget::validate() const
{
    return m_attrRange->validate(value().toQString());
}

Value AttrWidget::value() const
{
    auto sp = qobject_cast<QSpinBox*>(m_widget);
    if (sp) return sp->value();

    auto dsp = qobject_cast<QDoubleSpinBox*>(m_widget);
    if (dsp) return dsp->value();

    auto chb = qobject_cast<QtMaterialToggle*>(m_widget);
    if (chb) return chb->isChecked();

    auto le = qobject_cast<QLineEdit*>(m_widget);
    if (le) return le->text();

    auto lb = qobject_cast<LineButton*>(m_widget);
    if (lb) return lb->text();

    auto cb = qobject_cast<QComboBox*>(m_widget);
    if (cb) return cb->currentText();

    qFatal("unable to know the widget type.");
}

void AttrWidget::setValue(const Value& value)
{
    auto sp = qobject_cast<QSpinBox*>(m_widget);
    if (sp) { sp->setValue(value.toInt()); return; }

    auto dsp = qobject_cast<QDoubleSpinBox*>(m_widget);
    if (dsp) { dsp->setValue(value.toDouble()); return; }

    auto chb = qobject_cast<QtMaterialToggle*>(m_widget);
    if (chb) { chb->setChecked(value.toBool()); return; }

    auto le = qobject_cast<QLineEdit*>(m_widget);
    if (le) { le->setText(value.toQString()); return; }

    auto lb = qobject_cast<LineButton*>(m_widget);
    if (lb) { lb->setText(value.toQString()); return; }

    auto cb = qobject_cast<QComboBox*>(m_widget);
    if (cb) {
        int idx = cb->findText(value.toQString());
        if (idx >= 0) { cb->setCurrentIndex(idx); return; }
    }

    qFatal("unable to know the widget type.");
}

QWidget* AttrWidget::newWidget(AttributeRangePtr attrRange)
{
    switch (attrRange->type()) {
    case AttributeRange::Double_Range: {
        auto sp = new QDoubleSpinBox(this);
        sp->setMaximum(attrRange->max().toDouble());
        sp->setMinimum(attrRange->min().toDouble());
        sp->setDecimals(8);
        sp->setButtonSymbols(QDoubleSpinBox::NoButtons);
        connect(sp, SIGNAL(valueChanged(double)), SIGNAL(valueChanged()));
        connect(this, &AttrWidget::readOnlyChanged, [sp](bool on) { sp->setReadOnly(on); });
        return sp;
    }
    case AttributeRange::Int_Range: {
        auto sp = new QSpinBox(this);
        sp->setMaximum(attrRange->max().toInt());
        sp->setMinimum(attrRange->min().toInt());
        sp->setButtonSymbols(QSpinBox::NoButtons);
        connect(sp, SIGNAL(valueChanged(int)), SIGNAL(valueChanged()));
        connect(this, &AttrWidget::readOnlyChanged, [sp](bool on) { sp->setReadOnly(on); });
        return sp;
    }
    case AttributeRange::Double_Set:
    case AttributeRange::Int_Set:
    case AttributeRange::String_Set: {
        auto sov = dynamic_cast<SetOfValues*>(attrRange.get());
        auto cb = new QComboBox(this);
        for (const Value& v : sov->values()) {
            cb->addItem(v.toQString());
        }
        connect(cb, SIGNAL(currentIndexChanged(int)), SIGNAL(valueChanged()));
        connect(this, SIGNAL(readOnlyChanged(bool)), cb, SLOT(setDisabled(bool)));
        return cb;
    }
    case AttributeRange::Bool: {
        auto cb = new QtMaterialToggle(this);
        cb->setMaximumSize(QSize(60, 36));
        cb->setActiveColor(palette().color(QPalette::Link));
        connect(cb, SIGNAL(toggled(bool)), SIGNAL(valueChanged()));
        connect(this, SIGNAL(readOnlyChanged(bool)), cb, SLOT(setDisabled(bool)));
        return cb;
    }
    case AttributeRange::FilePath: {
        auto lb = new LineButton(this, LineButton::SelectTextFile);
        connect(lb->line(), SIGNAL(textChanged(QString)), SIGNAL(valueChanged()));
        connect(this, SIGNAL(readOnlyChanged(bool)), lb, SLOT(setReadOnly(bool)));
        return lb;
    }
    case AttributeRange::DirPath: {
        auto lb = new LineButton(this, LineButton::SelectDir);
        connect(lb->line(), SIGNAL(textChanged(QString)), SIGNAL(valueChanged()));
        connect(this, SIGNAL(readOnlyChanged(bool)), lb, SLOT(setReadOnly(bool)));
        return lb;
    }
    default:
        auto le = new QLineEdit(this);
        le->setText(attrRange->min().toQString());
        connect(le, SIGNAL(textChanged(QString)), SIGNAL(valueChanged()));
        connect(this, &AttrWidget::readOnlyChanged, [le](bool on) { le->setReadOnly(on); });
        return le;
    }
}

} // evoplex
