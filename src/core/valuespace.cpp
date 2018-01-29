/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "valuespace.h"
#include "utils.h"

namespace evoplex
{

ValueSpace* ValueSpace::parse(int id, const QString& attrName, QString space)
{
    ValueSpace* vs;
    if (space == "bool") {
        vs = new DefaultSpace(id, attrName, Bool, Value(false));
    } else if (space == "string") {
        vs = new DefaultSpace(id, attrName, String, Value(QString()));
    } else if (space == "dirpath") {
        vs = new DefaultSpace(id, attrName, DirPath, Value(QString()));
    } else if (space == "filepath") {
        vs = new DefaultSpace(id, attrName, FilePath, Value(QString()));
    } else if (space.contains('{') && space.endsWith('}')) {
        Values values;
        if (!Utils::paramSet(space, values)) {
            vs = new DefaultSpace();
        } else if (space.startsWith("int")) {
            space.replace("max", QString::number(INT32_MAX));
            vs = new SetSpace(id, attrName, Int_Set, values);
        } else if (space.startsWith("double")) {
            space.replace("max", QString::number(DBL_MAX));
            vs = new SetSpace(id, attrName, Double_Set, values);
        }
    } else if (space.contains('[') && space.endsWith(']')) {
        Value min, max;
        if (!Utils::paramInterval(space, min, max)) {
            vs = new DefaultSpace();
        } else if (space.startsWith("int")) {
            space.replace("max", QString::number(INT32_MAX));
            vs = new RangeSpace(id, attrName, Int_Range, min, max);
        } else if (space.startsWith("double")) {
            space.replace("max", QString::number(DBL_MAX));
            vs = new RangeSpace(id, attrName, Double_Range, min, max);
        }
    }

    if (!vs) {
        qWarning() << "[ValueSpace::parse]: unable to parse " << space;
        return new DefaultSpace();
    }
    Q_ASSERT(space == vs->space());
    return vs;
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
    case Int_Range:
    case Double_Range: {
        const RangeSpace* ispace = dynamic_cast<const RangeSpace*>(this);
        Value valSrc = Utils::valueFromString(ispace->min().type(), valueStr);
        if (valSrc.isValid() && valSrc >= ispace->min() && valSrc <= ispace->max()) {
            return valSrc;
        }
        break;
    }
    case Int_Set:
    case Double_Set: {
        const SetSpace* sspace = dynamic_cast<const SetSpace*>(this);
        Value valSrc = Utils::valueFromString(sspace->values().front().type(), valueStr);
        foreach (Value val, sspace->values()) {
            if (val == valSrc) return val;
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
