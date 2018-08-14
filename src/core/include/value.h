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

#include <functional>
#include <vector>
#include <QString>

namespace evoplex {

class Value;
typedef std::vector<Value> Values;

class Value
{
    friend struct std::hash<Value>;

public:
    enum Type { BOOL, CHAR, DOUBLE, INT, STRING, INVALID };

    Value();
    Value(const Value& value);
    Value(bool value);
    Value(char value);
    Value(double value);
    Value(int value);
    Value(const char* value);
    Value(const QString& value);

    // let's accept the specialized vector<bool>
    // http://www.cplusplus.com/reference/vector/vector-bool
    Value(std::vector<bool>::reference value);

    ~Value();

    inline Type type() const;
    inline bool isValid() const;
    inline bool isBool() const;
    inline bool isChar() const;
    inline bool isDouble() const;
    inline bool isInt() const;
    inline bool isString() const;

    inline bool toBool() const;
    inline char toChar() const;
    inline double toDouble() const;
    inline int toInt() const;
    inline quint32 toUInt() const;
    inline const char* toString() const;
    QString toQString(char format = 'g', int precision = 8) const;

    Value& operator=(const Value& v);
    bool operator==(const Value& v) const;
    bool operator!=(const Value& v) const;
    bool operator<(const Value& v) const;
    bool operator>(const Value& v) const;
    bool operator<=(const Value& v) const;
    bool operator>=(const Value& v) const;

private:
    union { bool b; char c; double d; int i; const char* s; } m_data;
    Type m_type;

    std::logic_error throwError() const;
};

/************************************************************************
   Value: Inline member functions
 ************************************************************************/

inline Value::Type Value::type() const
{ return m_type; }

inline bool Value::isValid() const
{ return m_type != INVALID; }

inline bool Value::isBool() const
{ return m_type == BOOL; }

inline bool Value::isChar() const
{ return m_type == CHAR; }

inline bool Value::isDouble() const
{ return m_type == DOUBLE; }

inline bool Value::isInt() const
{ return m_type == INT; }

inline bool Value::isString() const
{ return m_type == STRING; }

inline bool Value::toBool() const
{ if (m_type == BOOL) { return m_data.b; } throw throwError(); }

inline char Value::toChar() const
{ if (m_type == CHAR) { return m_data.c; } throw throwError(); }

inline double Value::toDouble() const
{ if (m_type == DOUBLE) { return m_data.d; } throw throwError(); }

inline int Value::toInt() const
{ if (m_type == INT) { return m_data.i; } throw throwError(); }

inline const char* Value::toString() const
{ if (m_type == STRING) { return m_data.s; } throw throwError(); }

inline quint32 Value::toUInt() const {
    if (m_type == INT && m_data.i >= 0) { return static_cast<quint32>(m_data.i); }
    throw throwError();
}

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
        while ((c = *s++)) {
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
        case evoplex::Value::INT: return std::hash<int>()(v.m_data.i);
        case evoplex::Value::DOUBLE: return std::hash<double>()(v.m_data.d);
        case evoplex::Value::BOOL: return std::hash<bool>()(v.m_data.b);
        case evoplex::Value::CHAR: return std::hash<char>()(v.m_data.c);
        case evoplex::Value::STRING: return std::hash<char*>()(v.m_data.s);
        default: throw std::invalid_argument("invalid type of Value");
        }
    }
};
} // std
#endif // VALUE_H
