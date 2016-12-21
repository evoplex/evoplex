/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <QVariant>
#include <QVariantHash>
#include <QVector>

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

    // check if the value belongs to the domain
    static bool validateParameter(const QString& space, const QVariant& value) {
        if (space.contains('{') && space.contains('}')) {
            QVariantList values = paramSet(space);
            if (!values.contains(value)) {
                qWarning() << "[Utils]: value (" << value
                           << ") is not in the set" << space;
                return false;
            }
        } else if (space.contains('[') && space.contains(']')) {
            QVariant min, max;
            paramInterval(space, min, max);
            if (!min.isValid() || !max.isValid()) {
                qWarning() << "[Utils]: unable to validate the value"
                           << value << "in the space" << space;
                return false;
            }

            double v = value.toDouble();
            if (min > v || max < v) {
                qWarning() << "[Utils]: value(" << value
                           << ") is out of bounds!" << space;
                return false;
            }
        } else {
            qWarning() << "[Utils]: unable to validate value!\n"
                       << "Value:" << value << "Space:" << space;
            return false;
        }
        // all is fine =D
        return true;
    }

    // assume that space is equal to 'int{ }' or 'double{ }'
    static QVariantList paramSet(QString space) {
        bool ok;
        QVariantList ret;
        space = space.remove("{").remove("}");
        QStringList values = space.split(",");

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
    static void paramInterval(QString space, QVariant& min, QVariant& max) {
        space = space.remove("{").remove("}");
        QStringList values = space.split(",");
        if (values.size() != 2) {
            qWarning() << "[Utils:paramInterval()]: unable to parse" << space;
            return;
        }

        bool ok1, ok2;
        if (space.startsWith("int")) {
            min = QVariant::fromValue(values.at(0).toInt(&ok1));
            max = QVariant::fromValue(values.at(1).toInt(&ok2));
        } else if (space.startsWith("double")) {
            min = QVariant::fromValue(values.at(0).toDouble(&ok1));
            max = QVariant::fromValue(values.at(1).toDouble(&ok2));
        }

        if (!ok1 || !ok2) {
            qWarning() << "[Utils:paramInterval()]: failed to parse" << space;
            min.clear();
            max.clear();
        }
    }

    // return the minimum value for each parameter
    static QVariantHash minParams(const QVariantHash& paramsSpace) {
        if (paramsSpace.isEmpty()) {
            return QVariantHash();
        }

        QVariantHash ret;
        QHashIterator<QString, QVariant> i(paramsSpace);
        while (i.hasNext()) {
            i.next();

            QString space = i.value().toString();
            if (space.isEmpty()) {
                ret.insert(i.key(), 0);
                qWarning() << "[Utils]: unable to parse the parameter space of"
                           << i.key() << i.value();
                continue;
            } else if (space.contains("{") && space.contains("}")) {
                ret.insert(i.key(), paramSet(space).first());
            } else if (space.contains("(") && space.contains(")")) {
                QVariant max, min;
                paramInterval(space, min, max);
                ret.insert(i.key(), min);
            } else {
                qWarning() << "[Utils]: unable to parse the parameter space of"
                           << i.key() << i.value();
            }
        }
    }
};

#endif // UTILS_H
