/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
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

    inline int size() const { return m_values.size(); }
    inline void resize(int size) { m_names.resize(size); m_values.resize(size); }

    inline int indexOf(const char* name) const { return indexOf(QString::fromLocal8Bit(name)); }
    inline int indexOf(const QString& name) const {
        int idx = std::find(m_names.begin(), m_names.end(), name) - m_names.begin();
        return idx == m_names.size() ? -1 : idx;
    }

    inline bool contains(const char* name) const { return indexOf(name) > -1; }
    inline bool contains(const QString& name) const { return indexOf(name) > -1; }

    inline void replace(int id, QString newName, Value newValue) { m_names[id] = newName; m_values[id] = newValue; }
    inline void push_back(QString name, Value value) { m_names.emplace_back(name); m_values.emplace_back(value); }

    inline const std::vector<QString>& names() const { return m_names; }
    inline const QString& name(int id) const {
        try {
            return m_names.at(id);
        } catch (const std::out_of_range&) {
            qFatal("[Attributes::name()]: Out of range error!");
        }
    }

    inline const Value& value(int id) const {
        try {
            return m_values.at(id);
        } catch (const std::out_of_range&) {
            qFatal("[Attributes::value()]: Out of range error!");
        }
    }

    inline void setValue(int id, Value value) {
        try {
            m_values.at(id) = value;
        } catch (const std::out_of_range&) {
            qFatal("[Attributes::setValue()]: Out of range error!");
        }
    }

    inline const Value& value(const char* name) const { return value(indexOf(name)); }
    inline const Value& value(const QString& name) const { return value(indexOf(name)); }
    inline const std::vector<Value>& values() const { return m_values; }

private:
    std::vector<QString> m_names;
    std::vector<Value> m_values;
};

}
#endif // ATTRIBUTES_H
