/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <QVariant>
#include <QVariantHash>
#include <QVector>
#include <QtDebug>
#include <float.h>

#include "utils/prg.h"

class Utils
{
public:
    // Fisher–Yates shuffle algorithm
    // https://bost.ocks.org/mike/shuffle
    // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    static QVector<int> shuffle(QVector<int> list);

    // convert a linear index to row and column
    static void ind2sub(const int ind, const int cols, int &row, int &col) {
        row = ind / cols;
        col = ind % cols;
    }

    // return the linear index of an element in a matrix.
    static int linearIdx(const int row, const int col, const int cols) {
        return row * cols + col;
    }

    // Check if the value belongs to the parameter space.
    // If true, return a valid QVariant with the correct type
    //
    // Parameter space can be:
    //   - "bool"             // a boolean
    //   - "string"             // a string
    //   - "int[min,max]"       // integers from min to max (including min and max)
    //   - "int{1,2,3}"         // set of integers
    //   - "double[min,max]     // doubles from min to max (including min and max)
    //   - "double{1.1,1.2}     // set of doubles
    //   * you can use 'max' to take the maximum value for the type
    static QVariant validateParameter(const QString& space, const QString& valueStr) {
        if (space == "string") {
            return valueStr; // nothing to do
        } else if (space == "bool") {
            return QVariant(valueStr).toBool();
        } else if (space.contains('{') && space.endsWith('}')) {
            QVector<QVariant> values = paramSet(space);
            int idx = values.indexOf(valueStr);
            if (idx < 0) {
                qWarning() << "[Utils]: value (" << valueStr
                           << ") is not in the set" << space;
                return QVariant();
            }
            return values.at(idx);
        } else if (space.contains('[') && space.endsWith(']')) {
            QVariant min, max;
            paramInterval(space, min, max);
            if (!min.isValid() || !max.isValid()) {
                qWarning() << "[Utils]: unable to validate the value"
                           << valueStr << "in the space" << space;
                return QVariant();
            }

            QVariant v = min.type() == QVariant::Int ? valueStr.toInt() : valueStr.toDouble();
            if (min > v || max < v) {
                qWarning() << "[Utils]: value(" << valueStr
                           << ") is out of bounds!" << space;
                return QVariant();
            }
            return v;
        }

        qWarning() << "[Utils]: unable to validate value!\n"
                   << "Value:" << valueStr << "Space:" << space;
        return QVariant();
    }

    // assume that space is equal to 'int{ }' or 'double{ }'
    // return a vector with all elements with the proper type
    static QVector<QVariant> paramSet(const QString& space) {
        bool ok;
        QVector<QVariant> ret;
        QString spc = space;
        QStringList values = spc.remove("{").remove("}").split(",");

        if (space.startsWith("int")) {
            values[0] = values[0].remove("int");
            foreach (QString v, values) {
                ret.append(v.toInt(&ok));
                if (!ok) qWarning() << "[Utils:paramSet()]: unable to convert" << v;
            }
        } else if (space.startsWith("double")) {
            values[0] = values[0].remove("double");
            foreach (QString v, values) {
                ret.append(v.toDouble(&ok));
                if (!ok) qWarning() << "[Utils:paramSet()]: unable to convert" << v;
            }
        } else {
            qWarning() << "[Utils:paramSet()]: unable to recognize the parameter space" << space;
        }
        return ret;
    }

    // assume that space is equal to 'int[min,max]' or 'double[min,max]'
    static void paramInterval(const QString& space, QVariant& min, QVariant& max) {
        QString spc = space;
        QStringList values = spc.remove("[").remove("]").split(",");
        if (values.size() != 2) {
            qWarning() << "[Utils:paramInterval()]: unable to parse" << space;
            return;
        }

        bool ok1, ok2;
        if (space.startsWith("int")) {
            values[0] = values[0].remove("int");
            min = QVariant(values.at(0).toInt(&ok1));
            if (values.at(1) == "max") {
                max = INT32_MAX;
                ok2 = true;
            } else {
                max = QVariant(values.at(1).toInt(&ok2));
            }
        } else if (space.startsWith("double")) {
            values[0] = values[0].remove("double");
            min = QVariant(values.at(0).toDouble(&ok1));
            if (values.at(1) == "max") {
                max = DBL_MAX;
                ok2 = true;
            } else {
                max = QVariant(values.at(1).toDouble(&ok2));
            }
        }

        if (!ok1 || !ok2) {
            qWarning() << "[Utils:paramInterval()]: failed to parse" << space;
            min.clear();
            max.clear();
        }
    }

    // return the boundary value for each parameter (min and max)
    static void boundaryValues(const QHash<QString, QString>& attributesSpace,
                               QVariantHash& minValues, QVariantHash& maxValues) {
        if (attributesSpace.isEmpty()) {
            return;
        }

        QHashIterator<QString, QString> i(attributesSpace);
        while (i.hasNext()) {
            i.next();
            const QString& space = i.value();
            if (space.isEmpty()) {
                minValues.insert(i.key(), 0);
                maxValues.insert(i.key(), 0);
                qWarning() << "[Utils]: unable to parse the parameter space of"
                           << i.key() << i.value();
                continue;
            } else if (space.contains("{") && space.endsWith("}")) {
                minValues.insert(i.key(), paramSet(space).first());
                maxValues.insert(i.key(), paramSet(space).last());
            } else if (space.contains("[") && space.endsWith("]")) {
                QVariant max, min;
                paramInterval(space, min, max);
                minValues.insert(i.key(), min);
                maxValues.insert(i.key(), max);
            } else {
                qWarning() << "[Utils]: unable to parse the parameter space of"
                           << i.key() << space;
            }
        }
    }

    // return a vector of valid random values for each parameter
    static QVector<QVariantHash> randomParams(const QHash<QString,QString>& attributesSpace, PRG* prg, const int size=1)
    {
        if (attributesSpace.isEmpty() || size < 1) {
            return QVector<QVariantHash>();
        }

        QVector<QVariantHash> ret;
        ret.reserve(size);
        for (int j = 0; j < size; ++j) {
            ret.push_back(QVariantHash());
        }

        QVector<QVariant> retVals;
        retVals.reserve(size);

        QHash<QString, QString>::const_iterator it = attributesSpace.begin();
        while (it != attributesSpace.end()) {
            const QString& space = it.value();

            if (space.contains("{") && space.endsWith("}")) {
                QVector<QVariant> vals = paramSet(space);
                for (int j = 0; j < size; ++j)
                    retVals.push_back(vals.at(prg->randI(vals.size())));
            } else if (space.contains("[") && space.endsWith("]")) {
                QVariant max, min;
                paramInterval(space, min, max);
                if (min.type() == QVariant::Int) {
                    for (int j = 0; j < size; ++j)
                        retVals.push_back(prg->randI(min.toInt(), max.toInt()));
                } else {
                    for (int j = 0; j < size; ++j)
                        retVals.push_back(prg->randD(min.toDouble(), max.toDouble()));
                }
            } else {
                for (int j = 0; j < size; ++j)
                    retVals.push_back(0);
                qWarning() << "[Utils]: unable to parse the parameter space of"
                           << it.key() << space;
            }

            for (int j = 0; j < size; ++j) {
                ret[j].insert(it.key(), retVals.at(j));
            }

            retVals.clear();
            retVals.squeeze();
            ++it;
        }
        return ret;
    }

    // return a valid random value for each parameter
    static QVariantHash randomParams(const QHash<QString,QString>& attributesSpace, PRG* prg)
    {
        QVector<QVariantHash> r = randomParams(attributesSpace, prg, 1);
        if (r.isEmpty())
            return QVariantHash();
        else
            return r.first();
    }
};

#endif // UTILS_H
