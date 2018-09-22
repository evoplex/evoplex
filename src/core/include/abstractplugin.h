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
    inline PRG* rand() const; // prg() alias
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

inline PRG* AbstractPlugin::rand() const
{ return prg(); }

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
