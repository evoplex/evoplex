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

#include <cfloat>
#include <QtDebug>
#include <QFileInfo>

#include "attributerange.h"

namespace evoplex
{

AttributeRangePtr AttributeRange::parse(int attrId, const QString& attrName,
                                        const QString& attrRangeStr)
{
    AttributeRangePtr vs;
    if (attrRangeStr == "bool") {
        vs = std::unique_ptr<SingleValue>(
                new SingleValue(attrId, attrName, Bool));
    } else if (attrRangeStr == "string") {
        vs = std::unique_ptr<SingleValue>(
                new SingleValue(attrId, attrName, String));
    } else if (attrRangeStr == "non-empty-string") {
        vs = std::unique_ptr<SingleValue>(
                new SingleValue(attrId, attrName, NonEmptyString));
    } else if (attrRangeStr == "dirpath") {
        vs = std::unique_ptr<SingleValue>(
                new SingleValue(attrId, attrName, DirPath));
    } else if (attrRangeStr == "filepath") {
        vs = std::unique_ptr<SingleValue>(
                new SingleValue(attrId, attrName, FilePath));
    } else if (attrRangeStr.contains('{') && attrRangeStr.endsWith('}')) {
        vs = setOfValues(attrRangeStr, attrId, attrName);
    } else if (attrRangeStr.contains('[') && attrRangeStr.endsWith(']')) {
        vs = intervalOfValues(attrRangeStr, attrId, attrName);
    }

    if (!vs || !vs->isValid()) {
        qWarning() << "unable to parse" << attrRangeStr;
        vs = std::unique_ptr<SingleValue>(new SingleValue());
    }
    return vs;
}

AttributeRangePtr AttributeRange::setOfValues(QString attrRangeStr, const int id,
                                              const QString& attrName)
{
    QStringList valuesStr = attrRangeStr.remove("{").remove("}").split(",");
    Type type;
    Values values;
    values.reserve(static_cast<size_t>(valuesStr.size()));
    bool ok = false;
    if (attrRangeStr.startsWith("int")) {
        type = AttributeRange::Int_Set;
        valuesStr[0] = valuesStr[0].remove("int");
        for (const QString& vStr : valuesStr) {
            values.push_back(vStr.toInt(&ok));
            if (!ok) break;
        }
    } else if (attrRangeStr.startsWith("double")) {
        type = AttributeRange::Double_Set;
        valuesStr[0] = valuesStr[0].remove("double");
        for (const QString& vStr : valuesStr) {
            values.push_back(vStr.toDouble(&ok));
            if (!ok) break;
        }
    } else if (attrRangeStr.startsWith("string")) {
        type = AttributeRange::String_Set;
        valuesStr[0] = valuesStr[0].remove("string");
        ok = true;
        for (const QString& vStr : valuesStr) {
            values.push_back(vStr);
        }
    } else {
        return std::unique_ptr<SingleValue>(new SingleValue());
    }

    if (!ok) {
        return std::unique_ptr<SingleValue>(new SingleValue());
    }
    return std::unique_ptr<SetOfValues>(new SetOfValues(id, attrName, type, values));
}

AttributeRangePtr AttributeRange::intervalOfValues(QString attrRangeStr, const int id,
                                                   const QString& attrName)
{
    QStringList values = attrRangeStr.remove("[").remove("]").split(",");
    if (values.size() != 2) {
        return std::unique_ptr<SingleValue>(new SingleValue());
    }

    Type type;
    Value min;
    Value max;
    bool ok1 = false;
    bool ok2 = false;

    if (attrRangeStr.startsWith("int")) {
        type = AttributeRange::Int_Range;
        values[0] = values[0].remove("int");
        min = Value(values.at(0).toInt(&ok1));
        if (values.at(1) == "max") {
            max = Value(INT32_MAX);
            ok2 = true;
        } else {
            max = Value(values.at(1).toInt(&ok2));
        }
    } else if (attrRangeStr.startsWith("double")) {
        type = AttributeRange::Double_Range;
        values[0] = values[0].remove("double");
        min = Value(values.at(0).toDouble(&ok1));
        if (values.at(1) == "max") {
            max = Value(DBL_MAX);
            ok2 = true;
        } else {
            max = Value(values.at(1).toDouble(&ok2));
        }
    } else {
        return std::unique_ptr<SingleValue>(new SingleValue());
    }

    if (!ok1 || !ok2) {
        return std::unique_ptr<SingleValue>(new SingleValue());
    }
    return std::unique_ptr<IntervalOfValues>(
            new IntervalOfValues(id, attrName, type, min, max));
}

Value AttributeRange::validate(const QString& valueStr) const
{
    if (m_type == String) {
        return Value(valueStr);
    }

    if (valueStr.isEmpty()) {
        qWarning() << "unable to validate value! It should not be empty."
                   << "Expected:" << m_attrRangeStr;
        return Value();
    }

    switch (m_type) {
    case NonEmptyString: {
        return Value(valueStr);
    }
    case Bool: {
        const QString v = valueStr.toLower();
        if (v == "true" || v == "1") return Value(true);
        if (v == "false" || v == "0") return Value(false);
        break;
    }
    case DirPath: {
        QFileInfo dir(valueStr);
        if (dir.exists() && dir.isDir()) return Value(valueStr);
        break;
    }
    case FilePath: {
        QFileInfo file(valueStr);
        if (file.exists() && file.isFile()) return Value(valueStr);
        break;
    }
    case Int_Range: {
        auto attrRange = dynamic_cast<const IntervalOfValues*>(this);
        bool ok = false;
        Value value(valueStr.toInt(&ok));
        if (ok && value.isValid() && value >= attrRange->min() &&
                value <= attrRange->max()) {
            return value;
        }
        break;
    }
    case Double_Range: {
        auto iov = dynamic_cast<const IntervalOfValues*>(this);
        bool ok = false;
        Value value(valueStr.toDouble(&ok));
        if (ok && value.isValid() && value >= iov->min() &&
                value <= iov->max()) {
            return value;
        }
        break;
    }
    case Int_Set: {
        auto sov = dynamic_cast<const SetOfValues*>(this);
        bool ok = false;
        Value value(valueStr.toInt(&ok));
        if (ok && value.isValid()) {
            for (const Value& validValue : sov->values()) {
                if (value == validValue) return value;
            }
        }
        break;
    }
    case Double_Set: {
        auto sov = dynamic_cast<const SetOfValues*>(this);
        bool ok = false;
        Value value(valueStr.toDouble(&ok));
        if (ok && value.isValid()) {
            for (const Value& validValue : sov->values()) {
                if (value == validValue) return value;
            }
        }
        break;
    }
    case String_Set: {
        auto sov = dynamic_cast<const SetOfValues*>(this);
        Value value(valueStr);
        if (value.isValid() && valueStr == value.toQString()) {
            for (const Value& validValue : sov->values()) {
                if (value == validValue) return value;
            }
        }
        break;
    }
    default:
        break;
    }

    qDebug() << "unable to validate value!\n"
             << "Input:" << valueStr << "\nExpected:" << m_attrRangeStr;
    return Value();
}

/**********************************/

AttributeRange::AttributeRange(int id, const QString& attrName, Type type)
    : m_id(id)
    , m_attrName(attrName)
    , m_type(type)
{
}

/**********************************/

SingleValue::SingleValue(int id, const QString& attrName, Type type)
    : AttributeRange(id, attrName, type)
{
    m_min = Value("");
    m_max = Value("");

    switch (m_type) {
    case Bool:
        m_attrRangeStr = "bool";
        m_min = Value(false);
        m_max = Value(true);
        break;
    case String:
        m_attrRangeStr = "string";
        break;
    case NonEmptyString:
        m_attrRangeStr = "non-empty-string";
        break;
    case DirPath:
        m_attrRangeStr = "dirpath";
        break;
    case FilePath:
        m_attrRangeStr = "filepath";
        break;
    default:
        m_attrRangeStr.clear();
    }
}

SingleValue::SingleValue()
    : SingleValue(-1, "", Invalid)
{
}

/**********************************/

IntervalOfValues::IntervalOfValues(int id, const QString& attrName, Type type,
                                   const Value& min, const Value& max)
    : AttributeRange(id, attrName, type)
{
    m_min = min;
    m_max = max;

    switch (m_type) {
    case Double_Range:
        m_attrRangeStr = QString("double[%1,%2]").arg(min.toDouble()).arg(max.toDouble());
        f_rand = &evoplex::IntervalOfValues::randD;
        break;
    case Int_Range:
        m_attrRangeStr = QString("int[%1,%2]").arg(min.toInt()).arg(max.toInt());
        f_rand = &evoplex::IntervalOfValues::randI;
        break;
    default:
        m_attrRangeStr.clear();
        qFatal("invalid type!");
    }
}

/**********************************/

SetOfValues::SetOfValues(int id, const QString& attrName, Type type, Values values)
    : AttributeRange(id, attrName, type)
    , m_values(values)
{
    m_min = (*std::min_element(m_values.cbegin(), m_values.cend()));
    m_max = (*std::max_element(m_values.cbegin(), m_values.cend()));

    m_attrRangeStr.clear();
    switch (m_type) {
    case Double_Set:
        m_attrRangeStr = "double{";
        break;
    case Int_Set:
        m_attrRangeStr = "int{";
        break;
    case String_Set:
        m_attrRangeStr = "string{";
        break;
    default:
        m_attrRangeStr.clear();
        qFatal("invalid type!");
    }

    m_attrRangeStr += values.front().toQString();
    for (size_t i = 1; i < values.size(); ++i) {
        m_attrRangeStr += "," + values.at(i).toQString();
    }
    m_attrRangeStr += "}";
}

} // evoplex
