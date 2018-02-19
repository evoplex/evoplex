/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "valuespace.h"
#include "utils.h"

namespace evoplex
{

ValueSpace* ValueSpace::parse(int id, const QString& attrName, const QString& space)
{
    ValueSpace* vs = nullptr;
    if (space == "bool") {
        vs = new DefaultSpace(id, attrName, Bool, Value(false));
    } else if (space == "string") {
        vs = new DefaultSpace(id, attrName, String, Value(QString()));
    } else if (space == "dirpath") {
        vs = new DefaultSpace(id, attrName, DirPath, Value(QString()));
    } else if (space == "filepath") {
        vs = new DefaultSpace(id, attrName, FilePath, Value(QString()));
    } else if (space.contains('{') && space.endsWith('}')) {
        vs = setSpace(space, id, attrName);
    } else if (space.contains('[') && space.endsWith(']')) {
        vs = rangeSpace(space, id, attrName);
    }

    if (!vs) {
        qWarning() << "[ValueSpace::parse]: unable to parse " << space;
        return new DefaultSpace();
    }
    return vs;
}

ValueSpace* ValueSpace::setSpace(QString space, const int id, const QString& attrName)
{
    QStringList valuesStr = space.remove("{").remove("}").split(",");
    ValueSpace::SpaceType type;
    Values values;
    bool ok = false;
    if (space.startsWith("int")) {
        type = ValueSpace::Int_Set;
        valuesStr[0] = valuesStr[0].remove("int");
        foreach (QString vStr, valuesStr) {
            values.push_back(vStr.toInt(&ok));
            if (!ok) break;
        }
    } else if (space.startsWith("double")) {
        type = ValueSpace::Double_Set;
        valuesStr[0] = valuesStr[0].remove("double");
        foreach (QString vStr, valuesStr) {
            values.push_back(vStr.toDouble(&ok));
            if (!ok) break;
        }
    }

    if (!ok) {
        return new DefaultSpace();
    }
    return new SetSpace(id, attrName, type, values);
}

ValueSpace* ValueSpace::rangeSpace(QString space, const int id, const QString& attrName)
{
    QStringList values = space.remove("[").remove("]").split(",");
    if (values.size() != 2) {
        return new DefaultSpace();
    }

    ValueSpace::SpaceType type;
    Value min;
    Value max;
    bool ok1 = false;
    bool ok2 = false;

    if (space.startsWith("int")) {
        type = ValueSpace::Int_Range;
        values[0] = values[0].remove("int");
        min = Value(values.at(0).toInt(&ok1));
        if (values.at(1) == "max") {
            max = Value(INT32_MAX);
            ok2 = true;
        } else {
            max = Value(values.at(1).toInt(&ok2));
        }
    } else if (space.startsWith("double")) {
        type = ValueSpace::Double_Range;
        values[0] = values[0].remove("double");
        min = Value(values.at(0).toDouble(&ok1));
        if (values.at(1) == "max") {
            max = Value(DBL_MAX);
            ok2 = true;
        } else {
            max = Value(values.at(1).toDouble(&ok2));
        }
    }

    if (!ok1 || !ok2) {
        return new DefaultSpace();
    }
    return new RangeSpace(id, attrName, type, min, max);
}

Value ValueSpace::validate(const QString& valueStr) const
{
    if (m_type == String) {
        return Value(valueStr);
    } else if (valueStr.isEmpty()) {
        qWarning() << "[ValueSpace::validate]: unable to validate parameter! It should not be empty."
                   << "Space:" << m_space;
        return Value();
    }

    switch (m_type) {
    case Bool: {
        if (valueStr == "true" || valueStr == "1") {
            return Value(true);
        } else if (valueStr == "false" || valueStr == "0") {
            return Value(false);
        }
        break;
    }
    case DirPath: {
        QFileInfo dir(valueStr);
        if (dir.exists() && dir.isDir()) {
            return Value(valueStr);
        }
        break;
    }
    case FilePath: {
        QFileInfo file(valueStr);
        if (file.exists() && file.isFile()) {
            return Value(valueStr);
        }
        break;
    }
    case Int_Range: {
        const RangeSpace* space = dynamic_cast<const RangeSpace*>(this);
        bool ok = false;
        Value value(valueStr.toInt(&ok));
        if (ok && value.isValid() && value >= space->min() && value <= space->max()) {
            return value;
        }
        break;
    }
    case Double_Range: {
        const RangeSpace* space = dynamic_cast<const RangeSpace*>(this);
        bool ok = false;
        Value value(valueStr.toDouble(&ok));
        if (ok && value.isValid() && value >= space->min() && value <= space->max()) {
            return value;
        }
        break;
    }
    case Int_Set: {
        const SetSpace* sspace = dynamic_cast<const SetSpace*>(this);
        bool ok = false;
        Value value(valueStr.toInt(&ok));
        if (ok && value.isValid()) {
            for (Value validValue : sspace->values()) {
                if (value == validValue) {
                    return value;
                }
            }
        }
        break;
    }
    case Double_Set: {
        const SetSpace* sspace = dynamic_cast<const SetSpace*>(this);
        bool ok = false;
        Value value(valueStr.toDouble(&ok));
        if (ok && value.isValid()) {
            for (Value validValue : sspace->values()) {
                if (value == validValue) {
                    return value;
                }
            }
        }
        break;
    }
    default:
        break;
    }

    qWarning() << "[ValueSpace]: unable to validate parameter!\n"
               << "Value:" << valueStr << "Space:" << m_space;
    return Value();
}

ValueSpace::ValueSpace(int id, const QString& attrName, SpaceType type)
    : m_id(id)
    , m_attrName(attrName)
    , m_type(type)
{
}

DefaultSpace::DefaultSpace(int id, const QString& attrName, SpaceType type, Value validValue)
    : ValueSpace(id, attrName, type)
    , m_validValue(validValue)
{
    switch (m_type) {
    case Bool:
        m_space = "bool";
        break;
    case String:
        m_space = "string";
        break;
    case DirPath:
        m_space = "dirpath";
        break;
    case FilePath:
        m_space = "filepath";
        break;
    default:
        m_space.clear();
    }
}

DefaultSpace::DefaultSpace()
    : DefaultSpace(-1, "", Invalid, Value())
{
}

RangeSpace::RangeSpace(int id, const QString& attrName, SpaceType type, Value min, Value max)
    : DefaultSpace(id, attrName, type, min)
    , m_min(min)
    , m_max(max)
{
    switch (m_type) {
    case Double_Range:
        m_space = QString("double[%1,%2]").arg(min.toDouble()).arg(max.toDouble());
        f_rand = &evoplex::RangeSpace::randD;
        break;
    case Int_Range:
        m_space = QString("int[%1,%2]").arg(min.toInt()).arg(max.toInt());
        f_rand = &evoplex::RangeSpace::randI;
        break;
    default:
        m_space.clear();
        qFatal("[RangeSpace]: invalid type!");
    }
}

SetSpace::SetSpace(int id, const QString& attrName, SpaceType type, Values values)
    : DefaultSpace(id, attrName, type, values.front())
    , m_values(values)
{
    m_space.clear();
    switch (m_type) {
    case Double_Set:
        m_space = "double{";
        break;
    case Int_Set:
        m_space = "int{";
        break;
    default:
        m_space.clear();
        qFatal("[SetSpace]: invalid type!");
    }

    m_space += values.front().toQString();
    for (int i = 1; i < values.size(); ++i) {
        m_space += "," + values.at(i).toQString();
    }
    m_space += "}";
}

} // evoplex
