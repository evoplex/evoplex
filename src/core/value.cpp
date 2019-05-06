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

#include <stdexcept>
#include <QString>
#include "value.h"

namespace evoplex {

Value::Value() : m_type(INVALID)
{
}

Value::Value(const Value& value) : m_type(value.type())
{
    if (m_type == INT) m_data.i = value.toInt();
    else if (m_type == DOUBLE) m_data.d = value.toDouble();
    else if (m_type == BOOL) m_data.b = value.toBool();
    else if (m_type == CHAR) m_data.c = value.toChar();
    else if (m_type == STRING) m_data.s = qstrdup(value.toString());
    else if (m_type != INVALID) qFatal("non-existent type");
}

Value::Value(bool value) : m_type(BOOL)
{
    m_data.b = value;
}

Value::Value(std::vector<bool>::reference value) : m_type(BOOL)
{
    m_data.b = value.operator bool();
}

Value::Value(char value) : m_type(CHAR)
{
    m_data.c = value;
}

Value::Value(double value) : m_type(DOUBLE)
{
    m_data.d = value;
}

Value::Value(int value) : m_type(INT)
{
    m_data.i = value;
}

Value::Value(const char* value) : m_type(STRING)
{
    // considering that Value(const QString& value) needs to call a new char[],
    // which must be deleted in this class, for consistency, let's do the same
    // here as well, and we can safely use delete[] whenever it's a STRING
    m_data.s = qstrdup(value);
}

// converts the QString to a char*
Value::Value(const QString& value) : m_type(STRING)
{
    QByteArray text = value.toUtf8();
    m_data.s = qstrdup(text.constData());
}

Value::~Value()
{
    if (m_type == STRING) {
        delete [] m_data.s;
    }
}

QString Value::toQString(char format, int precision) const
{
    switch (m_type) {
    case INT: return QString::number(m_data.i);
    case DOUBLE: return QString::number(m_data.d, format, precision);
    case BOOL: return QString::number(m_data.b);
    case CHAR: return QString(m_data.c);
    case STRING: return QString::fromUtf8(m_data.s);
    case INVALID: return QString();
    }
    throw std::invalid_argument("invalid type of Value");
}

Value& Value::operator=(const Value& v)
{
    if (this != &v) { // check for self-assignment
        if (m_type == STRING) { delete [] m_data.s; }
        m_type = v.m_type;
        switch (m_type) {
        case INT: m_data.i = v.m_data.i; break;
        case DOUBLE: m_data.d = v.m_data.d; break;
        case BOOL: m_data.b = v.m_data.b; break;
        case CHAR: m_data.c = v.m_data.c; break;
        case STRING: m_data.s = qstrdup(v.m_data.s); break;
        case INVALID: /* nothing-to-copy */ break;
        }
    }
    return *this;
}

bool Value::operator==(const Value& v) const
{
    if (m_type != v.m_type) {
        return false;
    }

    switch (m_type) {
    case INT: return m_data.i == v.m_data.i;
    case DOUBLE: return qFuzzyCompare(1.0+m_data.d, 1.0+v.m_data.d);
    case BOOL: return m_data.b == v.m_data.b;
    case CHAR: return m_data.c == v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) == 0;
    case INVALID: return m_type == v.m_type;
    }
    throw std::invalid_argument("invalid type of Value");
}

bool Value::operator!=(const Value& v) const
{
    if (m_type != v.m_type) {
        return true;
    }

    switch (m_type) {
    case INT: return m_data.i != v.m_data.i;
    case DOUBLE: return !qFuzzyCompare(1.0+m_data.d, 1.0+v.m_data.d);
    case BOOL: return m_data.b != v.m_data.b;
    case CHAR: return m_data.c != v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) != 0;
    case INVALID: return m_type != v.m_type;
    }
    throw std::invalid_argument("invalid type of Value");
}

bool Value::operator<(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator<] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i < v.m_data.i;
    case DOUBLE: return !qFuzzyCompare(1.0+m_data.d, 1.0+v.m_data.d) && m_data.d < v.m_data.d;
    case BOOL: return m_data.b < v.m_data.b;
    case CHAR: return m_data.c < v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) < 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator>(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator>] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i > v.m_data.i;
    case DOUBLE: return !qFuzzyCompare(1.0+m_data.d, 1.0+v.m_data.d) && m_data.d > v.m_data.d;
    case BOOL: return m_data.b > v.m_data.b;
    case CHAR: return m_data.c > v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) > 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator<=(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator<=] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i <= v.m_data.i;
    case DOUBLE: return m_data.d <= v.m_data.d || qFuzzyCompare(1.0+m_data.d, 1.0+v.m_data.d);
    case BOOL: return m_data.b <= v.m_data.b;
    case CHAR: return m_data.c <= v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) <= 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator>=(const Value& v) const
{
    if (m_type != v.m_type) {
        throw std::invalid_argument("[operator>=] type should be the same");
    }

    switch (m_type) {
    case INT: return m_data.i >= v.m_data.i;
    case DOUBLE: return m_data.d >= v.m_data.d || qFuzzyCompare(1.0+m_data.d, 1.0+v.m_data.d);
    case BOOL: return m_data.b >= v.m_data.b;
    case CHAR: return m_data.c >= v.m_data.c;
    case STRING: return qstrcmp(m_data.s, v.m_data.s) >= 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

std::logic_error Value::throwError() const
{
    switch (m_type) {
    case INT: return std::logic_error("It is an integer. You must use toInt()");
    case DOUBLE: return std::logic_error("It is a double. You must use toDouble()");
    case BOOL: return std::logic_error("It is a boolean. You must use toBool()");
    case CHAR: return std::logic_error("It is a char. You must use toChar()");
    case STRING: return std::logic_error("It is a string. You must use toString()");
    default: return std::logic_error("Value is invalid. Data cannot be retrieved.");
    }
}

} // evoplex
