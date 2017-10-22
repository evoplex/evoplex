/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QMap>
#include <QString>
#include <QVector>

namespace evoplex {

// QHash<attributeName, <id, space>>
typedef QHash<QString, QPair<int, QString>> AttributesSpace;

struct Value
{
    enum Type { BOOL, CHAR, DOUBLE, INT, STRING, INVALID } type;
    union { bool toBool; char toChar; double toDouble; int toInt; const char* toString; };

    Value() : type(INVALID) {}
    Value(const bool value) : type(BOOL), toBool(value) {}
    Value(const char value) : type(CHAR), toChar(value) {}
    Value(const double value) : type(DOUBLE), toDouble(value) {}
    Value(const int value) : type(INT), toInt(value) {}
    Value(const char* value) : type(STRING), toString(value) {}
    Value(const QString& value) : Value(strdup(value.toUtf8().constData())) {}

    inline bool isValid() const { return type != INVALID; }
    inline QString toQString() const {
        switch (type) {
            case BOOL: return QString::number(toBool);
            case CHAR: return QString(toChar);
            case DOUBLE: return QString::number(toDouble);
            case INT: return QString::number(toInt);
            case STRING: return QString::fromUtf8(toString);
            default: return QString();
        }
    }

    bool operator ==(Value v) {
        Q_ASSERT(type == v.type);
        if (type == BOOL) return toBool == v.toBool;
        if (type == CHAR) return toChar == v.toChar;
        if (type == DOUBLE) return toDouble == v.toDouble;
        if (type == INT) return toInt == v.toInt;
        Q_ASSERT(false);
    }

    bool operator <(Value v) {
        Q_ASSERT(type == v.type);
        if (type == CHAR) return toChar < v.toChar;
        if (type == DOUBLE) return toDouble < v.toDouble;
        if (type == INT) return toInt < v.toInt;
        Q_ASSERT(false);
    }

    bool operator >(Value v) {
        Q_ASSERT(type == v.type);
        if (type == CHAR) return toChar > v.toChar;
        if (type == DOUBLE) return toDouble > v.toDouble;
        if (type == INT) return toInt > v.toInt;
        Q_ASSERT(false);
    }

    bool operator <=(Value v) {
        Q_ASSERT(type == v.type);
        if (type <= CHAR) return toChar <= v.toChar;
        if (type <= DOUBLE) return toDouble <= v.toDouble;
        if (type <= INT) return toInt <= v.toInt;
        Q_ASSERT(false);
    }

    bool operator >=(Value v) {
        Q_ASSERT(type == v.type);
        if (type <= CHAR) return toChar >= v.toChar;
        if (type <= DOUBLE) return toDouble >= v.toDouble;
        if (type <= INT) return toInt >= v.toInt;
        Q_ASSERT(false);
    }
};

class Attributes
{
public:
    Attributes() {}
    Attributes(int size) { resize(size); }

    inline int size() const { return m_values.size(); }
    inline void resize(int size) { m_names.resize(size); m_values.resize(size); }

    inline int indexOf(const char* name, int from=0) const { return m_names.indexOf(QString::fromLocal8Bit(name), from); }
    inline int indexOf(const QString& name, int from=0) const { return m_names.indexOf(name, from); }

    inline void replace(int id, const QString& name, const Value& value) {
        m_names.replace(id, name);
        m_values.replace(id, value);
    }
    inline void append(const QString& name, const Value& value) {
        m_names.push_back(name);
        m_values.push_back(value);
    }

    inline const QString& name(int id) const { return m_names.at(id); }
    inline const QVector<QString>& names() const { return m_names; }

    inline const Value& value(int id) const { return m_values.at(id); }
    inline void setValue(int id, const Value& value) { m_values.replace(id, value); }

    inline const Value& value(const char* name) const { return value(indexOf(name)); }
    inline const Value& value(const QString& name) const { return value(indexOf(name)); }
    inline const QVector<Value>& values() const { return m_values; }

private:
    QVector<QString> m_names;
    QVector<Value> m_values;
};
}
#endif // ATTRIBUTES_H
