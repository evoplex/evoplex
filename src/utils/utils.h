/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef UTILS_H
#define UTILS_H

#include <QVariant>
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
    static bool validateProperty(const QString& domain, const QVariant& value) {
        QString d = domain;
        QString v = value.toString();
        if (d.startsWith('{') && d.endsWith('}')) {
            d = d.remove('{').remove('}');
            QStringList dValues = d.split(',');
            if (!dValues.contains(v)) {
                qWarning() << "[Utils]: value (" << v
                           << ") is not in the set" << d;
                return false;
            }
        } else if (d.startsWith('[') && d.endsWith(']')) {
            d = d.remove('[').remove(']');
            QStringList dValues = d.split(',');
            if (dValues.size() != 2) {
                qWarning() << "[Utils]: unable to validate the value" << v;
                return false;
            }
            double a = dValues.at(0).toDouble();
            double b = dValues.at(1).toDouble();
            double v = value.toDouble();
            if (v < a || v > b) {
                qWarning() << "[Utils]: value(" << v
                           << ") is out of bounds!" << domain;
                return false;
            }
        } else {
            qWarning() << "[Utils]: unable to validate value!\n"
                       << "Value:" << value << "Domain:" << domain;
            return false;
        }
        // all is fine =D
        return true;
    }
};

#endif // UTILS_H
