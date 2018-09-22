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

#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#include <QPair>
#include <QString>
#include <algorithm>
#include <stdexcept>
#include <vector>

#include "value.h"
#include "utils.h"

namespace evoplex {

class Attributes;
typedef std::vector<Attributes> SetOfAttributes;

/**
 *  @brief A container of Values which offers fixed time access to
 *  individual elements in any order by id and linear time by name.
 */
class Attributes
{
public:
    Attributes() {}
    Attributes(int size) { resize(size); }
    ~Attributes() {}

    /** @brief  Resizes the container to the specified number of elements.
     *  @param  size Number of elements the container should contain.
     *  This function will resize the container to the specified
     *  number of elements. If the number is smaller than the
     *  container's current size the container is truncated, otherwise
     *  default constructed elements, i.e., empty name and Value() are
     *  appended. */
    inline void resize(int size);

    /** @brief Attempt to preallocate enough memory for specified number
     *         of elements.
     *  @param size  Number of elements required.
     *  @throw std::length_error  If size n exceeds maximum size.
     *  This function attempts to reserve enough memory for the
     *  container to hold the specified number of elements.  If the
     *  number requested is more than max_size(), length_error is
     *  thrown.
     */
    inline void reserve(int size);

    inline int size() const;
    inline bool isEmpty() const;
    inline bool empty() const;

    inline int indexOf(const QString& name) const;
    inline bool contains(const QString& name) const;

    inline void replace(int id, QString newName, Value newValue);
    inline void push_back(QString name, Value value);

    inline const std::vector<QString>& names() const;
    inline const QString& name(int id) const;

    inline const std::vector<Value>& values() const;
    inline const Value& value(int id) const;
    inline Value value(const QString& name, Value defaultValue=Value()) const;

    inline void setValue(int id, const Value& value);

private:
    std::vector<QString> m_names;
    std::vector<Value> m_values;
};


/************************************************************************
   Attributes: Inline member functions
 ************************************************************************/

inline void Attributes::resize(int size) {
    size_t s = size < 0 ? 0 : static_cast<size_t>(size);
    m_names.resize(s);
    m_values.resize(s);
}

inline void Attributes::reserve(int size) {
    size_t s = size < 0 ? 0 : static_cast<size_t>(size);
    m_names.reserve(s);
    m_values.reserve(s);
}

inline int Attributes::size() const
{ return static_cast<int>(m_values.size()); }

inline bool Attributes::isEmpty() const
{ return m_names.empty(); }

inline bool Attributes::empty() const
{ return m_names.empty(); }

inline int Attributes::indexOf(const QString& name) const
{ return Utils::indexOf(m_names, name); }

inline bool Attributes::contains(const QString& name) const
{ return indexOf(name) > -1; }

inline void Attributes::replace(int id, QString newName, Value newValue) {
    if (id < 0) throw std::out_of_range("id must be positive!");
    size_t _id = static_cast<size_t>(id);
    m_names.at(_id) = newName;
    m_values.at(_id) = newValue;
}

inline void Attributes::push_back(QString name, Value value) {
    m_names.emplace_back(name);
    m_values.emplace_back(value);
    if (m_names.size() >= INT32_MAX)
        throw std::length_error("too many attributes");
}

inline const std::vector<QString>& Attributes::names() const
{ return m_names; }

inline const QString& Attributes::name(int id) const
{ return m_names.at(id); }

inline const std::vector<Value>& Attributes::values() const
{ return m_values; }

inline const Value& Attributes::value(int id) const
{ return m_values.at(id); }

inline Value Attributes::value(const QString& name, Value defaultValue) const {
    const int idx = indexOf(name);
    return idx < 0 ? defaultValue : m_values.at(static_cast<size_t>(idx));
}

inline void Attributes::setValue(int id, const Value& value) {
    if (id < 0) throw std::out_of_range("id must be positive!");
    m_values.at(static_cast<size_t>(id)) = value;
}

} // evoplex
#endif // ATTRIBUTES_H
