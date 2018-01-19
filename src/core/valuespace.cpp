/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "valuespace.h"
#include "utils.h"

namespace evoplex
{

ValueSpace ValueSpace::parse(int id, const QString& attrName, QString space)
{
    if (space == "bool") {
        return ValueSpace(id, attrName, Bool, Value(false));
    } else if (space == "string") {
        return ValueSpace(id, attrName, String, Value(QString()));
    } else if (space == "dirpath") {
        return ValueSpace(id, attrName, DirPath, Value(QString()));
    } else if (space == "filepath") {
        return ValueSpace(id, attrName, FilePath, Value(QString()));
    } else if (space.contains('{') && space.endsWith('}')) {
        Values values;
        if (!Utils::paramSet(space, values)) {
            return ValueSpace();
        } else if (space.startsWith("int")) {
            return SetSpace(id, attrName, Int_Set, values);
        } else if (space.startsWith("double")) {
            return SetSpace(id, attrName, Double_Set, values);
        } else {
            return ValueSpace();
        }
    } else if (space.contains('[') && space.endsWith(']')) {
        Value min, max;
        if (!Utils::paramInterval(space, min, max)) {
            return ValueSpace();
        } else if (space.startsWith("int")) {
            return IntervalSpace(id, attrName, Int_Interval, min, max);
        } else if (space.startsWith("double")) {
            return IntervalSpace(id, attrName, Double_Interval, min, max);
        } else {
            return ValueSpace();
        }
    }

    return ValueSpace();
}

ValueSpace::ValueSpace(int id, const QString& attrName, SpaceType type, Value validValue)
    : m_id(id),
      m_name(attrName),
      m_type(type),
      m_validValue(validValue)
{
}

ValueSpace::ValueSpace()
    : ValueSpace(-1, "", Invalid, Value())
{
}

Value ValueSpace::validate(const QString& valueStr)
{
    if (valueStr.isEmpty()) {
        qWarning() << "[ValueSpace::validate]: unable to validate parameter! It should not be empty."
                   << "Space:" << m_space;
        return Value();
    }

    switch (m_type) {
    case Bool: {
        if (valueStr == "true") {
            return Value(true);
        } else if (valueStr == "false") {
            return Value(false);
        }
        break;
    }
    case String: {
        return Value(valueStr);
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
        IntervalSpace* ispace = dynamic_cast<IntervalSpace*>(this);
        Value valSrc = Utils::valueFromString(ispace->min().type, valueStr);
        if (valSrc.isValid() && valSrc >= ispace->min() && valSrc <= ispace->max()) {
            return valSrc;
        }
        break;
    }
    case Int_Set:
    case Double_Set: {
        SetSpace* sspace = dynamic_cast<SetSpace*>(this);
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
