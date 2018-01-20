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
    if (space == "bool") {
        return new ValueSpace(id, attrName, space, Bool, Value(false));
    } else if (space == "string") {
        return new ValueSpace(id, attrName, space, String, Value(QString()));
    } else if (space == "dirpath") {
        return new ValueSpace(id, attrName, space, DirPath, Value(QString()));
    } else if (space == "filepath") {
        return new ValueSpace(id, attrName, space, FilePath, Value(QString()));
    } else if (space.contains('{') && space.endsWith('}')) {
        Values values;
        if (!Utils::paramSet(space, values)) {
            return new ValueSpace();
        } else if (space.startsWith("int")) {
            return new SetSpace(id, attrName, space, Int_Set, values);
        } else if (space.startsWith("double")) {
            return new SetSpace(id, attrName, space, Double_Set, values);
        }
    } else if (space.contains('[') && space.endsWith(']')) {
        Value min, max;
        if (!Utils::paramInterval(space, min, max)) {
            return new ValueSpace();
        } else if (space.startsWith("int")) {
            return new IntervalSpace(id, attrName, space, Int_Interval, min, max);
        } else if (space.startsWith("double")) {
            return new IntervalSpace(id, attrName, space, Double_Interval, min, max);
        }
    }

    qWarning() << "[ValueSpace::parse]: unable to parse " << space;
    return new ValueSpace();
}

ValueSpace::ValueSpace(int id, const QString& attrName, const QString& space, SpaceType type, Value validValue)
    : m_id(id)
    , m_attrName(attrName)
    , m_space(space)
    , m_type(type)
    , m_validValue(validValue)
{
}

ValueSpace::ValueSpace()
    : ValueSpace(-1, "", "", Invalid, Value())
{
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
    case Int_Interval:
    case Double_Interval: {
        const IntervalSpace* ispace = dynamic_cast<const IntervalSpace*>(this);
        Value valSrc = Utils::valueFromString(ispace->min().type, valueStr);
        if (valSrc.isValid() && valSrc >= ispace->min() && valSrc <= ispace->max()) {
            return valSrc;
        }
        break;
    }
    case Int_Set:
    case Double_Set: {
        const SetSpace* sspace = dynamic_cast<const SetSpace*>(this);
        Value valSrc = Utils::valueFromString(sspace->values().front().type, valueStr);
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


} // evoplex
