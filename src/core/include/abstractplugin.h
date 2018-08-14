/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
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

#ifndef ABSTRACT_PLUGIN_H
#define ABSTRACT_PLUGIN_H

#include <QString>

#include "attributes.h"
#include "prg.h"
#include "value.h"

namespace evoplex {

class Trial;

class AbstractPluginInterface
{
public:
    // Provide the destructor to keep compilers happy.
    virtual ~AbstractPluginInterface() = default;

    // Initializes the plugin.
    // This method is called when the plugin is created and
    // is mainly used to validate inputs and set the environment.
    // Return true if successful
    virtual bool init() = 0;
};

class AbstractPlugin : public AbstractPluginInterface
{
public:
    inline bool init() override;

    PRG* prg() const;
    inline const Attributes* attrs() const;

    inline const QString& attrName(int attrId) const;

    inline const Value& attr(int attrId) const;
    inline Value attr(const QString& name, Value defaultValue=Value()) const;

    inline bool attrExists(const char* name) const;
    inline bool attrExists(const QString& name) const;

protected:
    Trial* m_trial;

    AbstractPlugin() = default;
    ~AbstractPlugin() override = default;

    bool setup(Trial& trial, const Attributes& attrs);

private:
    const Attributes* m_attrs;
};

/************************************************************************
   AbstractPlugin: Inline member functions
 ************************************************************************/

inline bool AbstractPlugin::init()
{ return false; }

inline const Attributes* AbstractPlugin::attrs() const
{ return m_attrs; }

inline const QString& AbstractPlugin::attrName(int attrId) const
{ return m_attrs->name(attrId); }

inline const Value& AbstractPlugin::attr(int attrId) const
{ return m_attrs->value(attrId);  }

inline Value AbstractPlugin::attr(const QString& name, Value defaultValue) const
{ return m_attrs->value(name, defaultValue); }

inline bool AbstractPlugin::attrExists(const char* name) const
{ return m_attrs->contains(name); }

inline bool AbstractPlugin::attrExists(const QString& name) const
{ return m_attrs->contains(name); }

} // evoplex
#endif // ABSTRACT_PLUGIN_H
