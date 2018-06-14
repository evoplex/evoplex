/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2017 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    enum Type { BOOL, CHAR, DOUBLE, INT, STRING, INVALID };

    Value() : m_type(INVALID) {}
    Value(const bool value) : m_type(BOOL) { m_data.b = value; }
    Value(const char value) : m_type(CHAR) { m_data.c = value; }
    Value(const double value) : m_type(DOUBLE) { m_data.d = value; }
    Value(const int value) : m_type(INT) { m_data.i = value; }
    Value(const char* value) : m_type(STRING) { m_data.s = value; }
    Value(const QString& value) : Value(value.toUtf8().constData()) {}

    inline bool isValid() const { return m_type != INVALID; }
    inline Type type() const { return m_type; }
    inline const bool toBool() const { return m_data.b; }
    inline const char toChar() const { return m_data.c; }
    inline const double toDouble() const { return m_data.d; }
    inline const int toInt() const { return m_data.i; }
    inline const char* toString() const { return m_data.s; }

    inline QString toQString() const
    {
        switch (m_type) {
        case INT: return QString::number(m_data.i);
        case DOUBLE: return QString::number(m_data.d);
        case BOOL: return QString::number(m_data.b);
        case CHAR: return QString(m_data.c);
        case STRING: return QString::fromUtf8(m_data.s);
        default: throw std::invalid_argument("invalid type of Value");
        }
    }

    bool operator==(const Value& v) const
    {
        if (m_type != v.m_type)
            return false;

        switch (m_type) {
        case INT: return m_data.i == v.m_data.i;
        case DOUBLE: return m_data.d == v.m_data.d;
        case BOOL: return m_data.b == v.m_data.b;
        case CHAR: return m_data.c == v.m_data.c;
        case STRING: return strcmp (m_data.s, v.m_data.s) == 0;
        default: throw std::invalid_argument("invalid type of Value");
        }
    }

    bool operator!=(const Value& v) const
    {
        if (m_type != v.m_type)
            return true;

        switch (m_type) {
        case INT: return m_data.i != v.m_data.i;
        case DOUBLE: return m_data.d != v.m_data.d;
        case BOOL: return m_data.b != v.m_data.b;
        case CHAR: return m_data.c != v.m_data.c;
        case STRING: return strcmp (m_data.s, v.m_data.s) != 0;
        default: throw std::invalid_argument("invalid type of Value");
        }
    }

    bool operator<(const Value& v) const
    {
        if (m_type != v.m_type) {
            throw std::invalid_argument("[operator<] type should be the same");
        }

        switch (m_type) {
        case INT: return m_data.i < v.m_data.i;
        case DOUBLE: return m_data.d < v.m_data.d;
        case BOOL: return m_data.b < v.m_data.b;
        case CHAR: return m_data.c < v.m_data.c;
        case STRING: throw std::invalid_argument("[operator<] cannot do that with strings");
        default: throw std::invalid_argument("invalid type of Value");
        }
    }

    bool operator>(const Value& v) const
    {
        if (m_type != v.m_type) {
            throw std::invalid_argument("[operator>] type should be the same");
        }

        switch (m_type) {
        case INT: return m_data.i > v.m_data.i;
        case DOUBLE: return m_data.d > v.m_data.d;
        case BOOL: return m_data.b > v.m_data.b;
        case CHAR: return m_data.c > v.m_data.c;
        case STRING: throw std::invalid_argument("[operator>] cannot do that with strings");
        default: throw std::invalid_argument("invalid type of Value");
        }
    }

    bool operator<=(const Value& v) const
    {
        if (m_type != v.m_type) {
            throw std::invalid_argument("[operator<=] type should be the same");
        }

        switch (m_type) {
        case INT: return m_data.i <= v.m_data.i;
        case DOUBLE: return m_data.d <= v.m_data.d;
        case BOOL: return m_data.b <= v.m_data.b;
        case CHAR: return m_data.c <= v.m_data.c;
        case STRING: throw std::invalid_argument("[operator<=] cannot do that with strings");
        default: throw std::invalid_argument("invalid type of Value");
        }
    }

    bool operator>=(const Value& v) const
    {
        if (m_type != v.m_type) {
            throw std::invalid_argument("[operator>=] type should be the same");
        }

        switch (m_type) {
        case INT: return m_data.i >= v.m_data.i;
        case DOUBLE: return m_data.d >= v.m_data.d;
        case BOOL: return m_data.b >= v.m_data.b;
        case CHAR: return m_data.c >= v.m_data.c;
        case STRING: throw std::invalid_argument("[operator>=] cannot do that with strings");
        default: throw std::invalid_argument("invalid type of Value");
        }
    }

private:
    union { bool b; char c; double d; int i; const char* s; } m_data;
    Type m_type;
};
} // evoplex

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
        switch (v.type()) {
        case evoplex::Value::INT: return hash<int>()(v.toInt());
        case evoplex::Value::DOUBLE: return hash<double>()(v.toDouble());
        case evoplex::Value::BOOL: return hash<bool>()(v.toBool());
        case evoplex::Value::CHAR: return hash<char>()(v.toChar());
        case evoplex::Value::STRING: return hash<char*>()(v.toString());
        default: throw std::invalid_argument("invalid type of Value");
        }
    }
};
}

#endif // VALUE_H
