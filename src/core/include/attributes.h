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

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QPair>
#include <QString>
#include <algorithm>
#include <stdexcept>
#include <vector>

#include "value.h"

namespace evoplex {

class Attributes
{
public:
    Attributes() {}
    Attributes(int size) { resize(size); }
    ~Attributes() {}

    inline int size() const;
    inline void resize(int size);
    inline void reserve(int size);

    inline int indexOf(const char* name) const;
    inline int indexOf(const QString& name) const;

    inline bool contains(const char* name) const;
    inline bool contains(const QString& name) const;

    inline void replace(int id, QString newName, Value newValue);
    inline void push_back(QString name, Value value);

    inline const std::vector<QString>& names() const;
    inline const QString& name(int id) const;

    inline const std::vector<Value>& values() const;

    inline void setValue(int id, Value value);

    inline const Value& value(int id) const;
    inline const Value& value(const char* name) const;
    inline const Value& value(const QString& name) const;
    inline const Value& value(const char* name, const Value& defaultValue) const;
    inline const Value& value(const QString& name, const Value& defaultValue) const;

private:
    std::vector<QString> m_names;
    std::vector<Value> m_values;
};


/************************************************************************
   Attributes: Inline member functions
 ************************************************************************/

inline int Attributes::size() const
{ return static_cast<int>(m_values.size()); }

inline void Attributes::resize(int size)
{ m_names.resize(size); m_values.resize(size); }

inline void Attributes::reserve(int size)
{ m_names.reserve(size); m_values.reserve(size); }

inline int Attributes::indexOf(const char* name) const
{ return indexOf(QString::fromLocal8Bit(name)); }

inline int Attributes::indexOf(const QString& name) const {
    int idx = std::find(m_names.begin(), m_names.end(), name) - m_names.begin();
    return idx == static_cast<int>(m_names.size()) ? -1 : idx;
}

inline bool Attributes::contains(const char* name) const
{ return indexOf(name) > -1; }

inline bool Attributes::contains(const QString& name) const
{ return indexOf(name) > -1; }

inline void Attributes::replace(int id, QString newName, Value newValue)
{ m_names.at(id) = newName; m_values.at(id) = newValue; }

inline void Attributes::push_back(QString name, Value value)
{ m_names.emplace_back(name); m_values.emplace_back(value); }

inline const std::vector<QString>& Attributes::names() const
{ return m_names; }

inline const QString& Attributes::name(int id) const
{ return m_names.at(id); }

inline const std::vector<Value>& Attributes::values() const
{ return m_values; }

inline void Attributes::setValue(int id, Value value)
{ m_values.at(id) = value; }

inline const Value& Attributes::value(int id) const
{ return m_values.at(id); }

inline const Value& Attributes::value(const char* name) const
{ return value(indexOf(name)); }

inline const Value& Attributes::value(const QString& name) const
{ return value(indexOf(name)); }

inline const Value& Attributes::value(const char* name, const Value& defaultValue) const {
    const int idx = indexOf(name);
    return idx < 0 ? defaultValue : value(idx);
}

inline const Value& Attributes::value(const QString& name, const Value& defaultValue) const {
    const int idx = indexOf(name);
    return idx < 0 ? defaultValue : value(idx);
}


} // evoplex
#endif // ATTRIBUTES_H
