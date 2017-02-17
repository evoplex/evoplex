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

#include "utils/prg.h"

class Utils
{
public:
    // Fisher–Yates shuffle algorithm
    // https://bost.ocks.org/mike/shuffle
    // http://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle
    static QVector<int> shuffle(QVector<int> list);

    // convert a linear index to row column
    static void ind2sub(const int ind, const int cols, int &row, int &col) {
        row = ind / cols;
        col = ind % cols;
    }

    // return a linear index
    static int getLinearIdx(const int row, const int col, const int cols) {
        return row * cols + col;
    }

    // Check if the value belongs to the parameter space.
    // If true, return a valid QVariant with the correct type
    //
    // Parameter space can be:
    //   - "string"             // a string
    //   - "int[min,max]"       // integers from min to max (including min and max)
    //   - "int{1,2,3}"         // set of integers
    //   - "double[min,max]     // doubles from min to max (including min and max)
    //   - "double{1.1,1.2}     // set of doubles
    static QVariant validateParameter(const QString& space, const QString& valueStr) {
        if (space == "string") {
            // nothing to do
            return valueStr;
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
            max = QVariant(values.at(1).toInt(&ok2));
        } else if (space.startsWith("double")) {
            values[0] = values[0].remove("double");
            min = QVariant(values.at(0).toDouble(&ok1));
            max = QVariant(values.at(1).toDouble(&ok2));
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

    // return a valid random value for each parameter
    static QVariantHash randomParams(const QHash<QString,QString>& attributesSpace, PRG* prg) {
        if (attributesSpace.isEmpty()) {
            return QVariantHash();
        }

        QVariantHash ret;
        QHashIterator<QString, QString> i(attributesSpace);
        while (i.hasNext()) {
            i.next();

            const QString& space = i.value();
            if (space.isEmpty()) {
                ret.insert(i.key(), 0);
                qWarning() << "[Utils]: unable to parse the parameter space of"
                           << i.key() << i.value();
                continue;
            } else if (space.contains("{") && space.endsWith("}")) {
                QVector<QVariant> vals = paramSet(space);
                ret.insert(i.key(), vals.at(prg->randI(vals.size())));
            } else if (space.contains("[") && space.endsWith("]")) {
                QVariant max, min;
                paramInterval(space, min, max);
                if (min.type() == QVariant::Int) {
                    ret.insert(i.key(), prg->randI(min.toInt(), max.toInt()));
                } else {
                    ret.insert(i.key(), prg->randD(min.toDouble(), max.toDouble()));
                }
            } else {
                qWarning() << "[Utils]: unable to parse the parameter space of"
                           << i.key() << space;
            }
        }

        return ret;
    }
};

#endif // UTILS_H
