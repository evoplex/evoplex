/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef VALUE_H
#define VALUE_H

#include <functional>
#include <vector>
#include <QString>

namespace evoplex {

class Value;
using Values = std::vector<Value>;

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
