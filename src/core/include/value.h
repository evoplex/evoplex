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

    Value();
    Value(const bool value);
    Value(const char value);
    Value(const double value);
    Value(const int value);
    Value(const char* value);
    Value(const QString& value);

    inline bool isValid() const;
    inline Type type() const;

    inline const bool toBool() const;
    inline const char toChar() const;
    inline const double toDouble() const;
    inline const int toInt() const;
    inline const char* toString() const;
    QString toQString() const;

    bool operator==(const Value& v) const;
    bool operator!=(const Value& v) const;
    bool operator<(const Value& v) const;
    bool operator>(const Value& v) const;
    bool operator<=(const Value& v) const;
    bool operator>=(const Value& v) const;

private:
    union { bool b; char c; double d; int i; const char* s; } m_data;
    Type m_type;
};

/************************************************************************
   Value: Inline member functions
 ************************************************************************/

inline bool Value::isValid() const
{ return m_type != INVALID; }

inline Value::Type Value::type() const
{ return m_type; }

inline const bool Value::toBool() const
{ return m_data.b; }

inline const char Value::toChar() const
{ return m_data.c; }

inline const double Value::toDouble() const
{ return m_data.d; }

inline const int Value::toInt() const
{ return m_data.i; }

inline const char* Value::toString() const
{ return m_data.s; }

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
} // std
#endif // VALUE_H
