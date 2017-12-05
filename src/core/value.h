/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef VALUE_H
#define VALUE_H

#include <QString>
#include <vector>

namespace evoplex
{
class Value;
typedef std::vector<Value> Values;

class Value
{
public:
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

    inline QString toQString() const
    {
        switch (type) {
        case INT: return QString::number(toInt);
        case DOUBLE: return QString::number(toDouble);
        case BOOL: return QString::number(toBool);
        case CHAR: return QString(toChar);
        case STRING: return QString::fromUtf8(toString);
        default: throw std::invalid_argument("invalid type");
        }
    }

    bool operator==(const Value& v) const
    {
        if (type != v.type)
            return false;

        switch (type) {
        case INT: return toInt == v.toInt;
        case DOUBLE: return toDouble == v.toDouble;
        case BOOL: return toBool == v.toBool;
        case CHAR: return toChar == v.toChar;
        case STRING: return strcmp (toString, v.toString) == 0;
        default: throw std::invalid_argument("invalid type");
        }
    }

    bool operator!=(const Value& v) const
    {
        if (type != v.type)
            return true;

        switch (type) {
        case INT: return toInt != v.toInt;
        case DOUBLE: return toDouble != v.toDouble;
        case BOOL: return toBool != v.toBool;
        case CHAR: return toChar != v.toChar;
        case STRING: return strcmp (toString, v.toString) != 0;
        default: throw std::invalid_argument("invalid type");
        }
    }

    bool operator<(const Value& v) const
    {
        if (type != v.type) {
            throw std::invalid_argument("[operator<] type should be the same");
        }

        switch (type) {
        case INT: return toInt < v.toInt;
        case DOUBLE: return toDouble < v.toDouble;
        case BOOL: return toBool < v.toBool;
        case CHAR: return toChar < v.toChar;
        case STRING: throw std::invalid_argument("[operator<] cannot do that with strings");
        default: throw std::invalid_argument("invalid type");
        }
    }

    bool operator>(const Value& v) const
    {
        if (type != v.type) {
            throw std::invalid_argument("[operator>] type should be the same");
        }

        switch (type) {
        case INT: return toInt > v.toInt;
        case DOUBLE: return toDouble > v.toDouble;
        case BOOL: return toBool > v.toBool;
        case CHAR: return toChar > v.toChar;
        case STRING: throw std::invalid_argument("[operator>] cannot do that with strings");
        default: throw std::invalid_argument("invalid type");
        }
    }

    bool operator<=(const Value& v) const
    {
        if (type != v.type) {
            throw std::invalid_argument("[operator<=] type should be the same");
        }

        switch (type) {
        case INT: return toInt <= v.toInt;
        case DOUBLE: return toDouble <= v.toDouble;
        case BOOL: return toBool <= v.toBool;
        case CHAR: return toChar <= v.toChar;
        case STRING: throw std::invalid_argument("[operator<=] cannot do that with strings");
        default: throw std::invalid_argument("invalid type");
        }
    }

    bool operator>=(const Value& v) const
    {
        if (type != v.type) {
            throw std::invalid_argument("[operator>=] type should be the same");
        }

        switch (type) {
        case INT: return toInt >= v.toInt;
        case DOUBLE: return toDouble >= v.toDouble;
        case BOOL: return toBool >= v.toBool;
        case CHAR: return toChar >= v.toChar;
        case STRING: throw std::invalid_argument("[operator>=] cannot do that with strings");
        default: throw std::invalid_argument("invalid type");
        }
    }
};
}

namespace std
{
// http://www.cse.yorku.ca/~oz/hash.html
template <>
struct hash<char*>
{
    size_t operator()(const char *s) const {
        size_t h = 5381;
        int c;
        while (c = *s++) {
            h = ((h << 5) + h) + c;
        }
        return h;
    }
};

template <>
struct hash<evoplex::Value>
{
    size_t operator()(const evoplex::Value& v) const {
        switch (v.type) {
        case evoplex::Value::INT: return hash<int>()(v.toInt);
        case evoplex::Value::DOUBLE: return hash<double>()(v.toDouble);
        case evoplex::Value::BOOL: return hash<bool>()(v.toBool);
        case evoplex::Value::CHAR: return hash<char>()(v.toChar);
        case evoplex::Value::STRING: return hash<char*>()(v.toString);
        default: Q_ASSERT(false);
        }
    }
};
}

#endif // VALUE_H
