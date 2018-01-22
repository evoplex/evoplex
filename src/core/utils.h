/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <QFileInfo>
#include <QVariant>
#include <QVector>
#include <QtDebug>
#include <float.h>

#include "agent.h"
#include "attributes.h"
#include "prg.h"
#include "valuespace.h"

namespace evoplex {

class Utils
{
public:

    // convert a linear index to row and column
    static void ind2sub(const int ind, const int cols, int &row, int &col) {
        row = ind / cols;
        col = ind % cols;
    }

    // return the linear index of an element in a matrix.
    static int linearIdx(const int row, const int col, const int cols) {
        return row * cols + col;
    }

    static int linearIdx(QPair<int,int> rowCol, const int cols) {
        return linearIdx(rowCol.first, rowCol.second, cols);
    }

    static Value valueFromString(Value::Type type, const QString& str) {
        Value value;
        bool ok;
        if (type == Value::DOUBLE)
            value = str.toDouble(&ok);
        else if (type == Value::INT)
            value = str.toInt(&ok);

        if (!ok || !value.isValid())
            return Value();
        return value;
    }

    static bool isASet(const QString& space) {
        return space.contains('{') && space.endsWith('}');
    }

    static bool isAnInterval(const QString& space) {
        return space.contains('[') && space.endsWith(']');
    }

    // Check if the value belongs to the parameter space.
    // If true, return a valid Value with the correct type
    //
    // Parameter space can be:
    //   - "bool"               // a boolean
    //   - "string"             // a string
    //   - "dirpath"            // a string containing a valid dirpath
    //   - "filepath"           // a string containing a valid filepath
    //   - "int[min,max]"       // integers from min to max (including min and max)
    //   - "int{1,2,3}"         // set of integers
    //   - "double[min,max]     // doubles from min to max (including min and max)
    //   - "double{1.1,1.2}     // set of doubles
    //   * you can use 'max' to take the maximum value for the type
    //
    static Value validateParameter(const QString& space, const QString& valueStr) {
        if (space == "string") {
            return Value(valueStr);
        } else if (valueStr.isEmpty()) {
            qWarning() << "[Utils]: unable to validate parameter! It should not be empty."
                       << "Space:" << space;
            return Value();
        } else if (space == "dirpath") {
            QFileInfo dir(valueStr);
            if (dir.exists() && dir.isDir()) {
                return Value(valueStr);
            }
        } else if (space == "filepath") {
            QFileInfo file(valueStr);
            if (file.exists() && file.isFile()) {
                return Value(valueStr);
            }
        } else if (space == "bool") {
            return QVariant(valueStr).toBool();
        } else if (isASet(space)) {
            Values values;
            if (paramSet(space, values)) {
                Value valSrc = valueFromString(values.front().type, valueStr);
                foreach (Value val, values) {
                    if (val == valSrc) return val;
                }
            }
        } else if (isAnInterval(space)) {
            Value min, max;
            if (paramInterval(space, min, max)) {
                Value valSrc = valueFromString(min.type, valueStr);
                if (valSrc.isValid() && valSrc >= min && valSrc <= max) {
                    return valSrc;
                }
            }
        }

        qWarning() << "[Utils]: unable to validate parameter!\n"
                   << "Value:" << valueStr << "Space:" << space;
        return Value();
    }

    // assume that space is equal to 'int{ }' or 'double{ }'
    // return a vector with all elements with the proper type
    static bool paramSet(const QString& space, Values& values) {
        QString spc = space;
        QStringList valuesStr = spc.remove("{").remove("}").split(",");
        bool ok = false;

        if (space.startsWith("int")) {
            valuesStr[0] = valuesStr[0].remove("int");
            foreach (QString vStr, valuesStr) {
                values.push_back(vStr.toInt(&ok));
                if (!ok) break;
            }
        } else if (space.startsWith("double")) {
            valuesStr[0] = valuesStr[0].remove("double");
            foreach (QString vStr, valuesStr) {
                values.push_back(vStr.toDouble(&ok));
                if (!ok) break;
            }
        }

        if (!ok) {
            qWarning() << "[Utils:paramSet()]: unable to recognize the parameter space" << space;
        }
        return ok;
    }

    // assume that space is equal to 'int[min,max]' or 'double[min,max]'
    static bool paramInterval(const QString& space, Value& min, Value& max) {
        QString spc = space;
        QStringList values = spc.remove("[").remove("]").split(",");
        if (values.size() != 2) {
            qWarning() << "[Utils:paramInterval()]: unable to parse" << space;
            return false;
        }

        bool ok1 = false;
        bool ok2 = false;
        if (space.startsWith("int")) {
            values[0] = values[0].remove("int");
            min = Value(values.at(0).toInt(&ok1));
            if (values.at(1) == "max") {
                max = Value(INT32_MAX);
                ok2 = true;
            } else {
                max = Value(values.at(1).toInt(&ok2));
            }
        } else if (space.startsWith("double")) {
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
            qWarning() << "[Utils:paramInterval()]: failed to parse" << space;
            return false;
        }
        return true;
    }
};
}
#endif // UTILS_H
