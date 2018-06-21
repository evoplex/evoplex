/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
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

#include <QString>
#include "value.h"

namespace evoplex {

Value::Value() : m_type(INVALID)
{
}

Value::Value(const bool value) : m_type(BOOL)
{
    m_data.b = value;
}

Value::Value(const char value) : m_type(CHAR)
{
    m_data.c = value;
}

Value::Value(const double value) : m_type(DOUBLE)
{
    m_data.d = value;
}

Value::Value(const int value) : m_type(INT)
{
    m_data.i = value;
}

Value::Value(const char* value) : m_type(STRING)
{
    m_data.s = value;
}

// converts the QString to a char*
// qPrintable will return a temporary char*, so we use qstrdup (see ref)
Value::Value(const QString& value)
    : Value(qstrdup(qPrintable(value)))
{
}

QString Value::toQString() const
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

bool Value::operator==(const Value& v) const
{
    if (m_type != v.m_type) {
        return false;
    }

    switch (m_type) {
    case INT: return m_data.i == v.m_data.i;
    case DOUBLE: return m_data.d == v.m_data.d;
    case BOOL: return m_data.b == v.m_data.b;
    case CHAR: return m_data.c == v.m_data.c;
    case STRING: return strcmp (m_data.s, v.m_data.s) == 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator!=(const Value& v) const
{
    if (m_type != v.m_type) {
        return true;
    }

    switch (m_type) {
    case INT: return m_data.i != v.m_data.i;
    case DOUBLE: return m_data.d != v.m_data.d;
    case BOOL: return m_data.b != v.m_data.b;
    case CHAR: return m_data.c != v.m_data.c;
    case STRING: return strcmp (m_data.s, v.m_data.s) != 0;
    default: throw std::invalid_argument("invalid type of Value");
    }
}

bool Value::operator<(const Value& v) const
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

bool Value::operator>(const Value& v) const
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

bool Value::operator<=(const Value& v) const
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

bool Value::operator>=(const Value& v) const
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

} // evoplex
