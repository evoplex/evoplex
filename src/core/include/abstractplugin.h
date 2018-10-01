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

/**
 * @brief Base class for plugins.
 * @see AbstractGraph and AbstractModel
 */
class AbstractPlugin
{
    friend class Trial;
    friend class AbstractGraph;

public:
    /**
     * @brief Initializes the plugin.
     * This method is called when the plugin is created and
     * is mainly used to validate inputs and set the environment.
     * The default implementation does nothing.
     * @return true if successful
     */
    virtual bool init();

    //! @returns pseudo-random generator pointer
    PRG* prg() const;
    //! prg() alias
    inline PRG* rand() const;

    //! @returns the plugin's attributes
    inline const Attributes* attrs() const;

    //! @returns the attribute's name for \p attrId
    //! @throw  std::out_of_range if no such data is present.
    inline const QString& attrName(int attrId) const;
    //! @returns the attribute's value for \p attrId
    //! @throw  std::out_of_range if no such data is present.
    inline const Value& attr(int attrId) const;

    /**
     * @param name the attribute name
     * @param defaultValue a Value to be returned if \p name is not present.
     * @return the attribute's value
     */
    inline Value attr(const QString& name, Value defaultValue=Value()) const;

    /**
     * @brief Checks if the plugin's attribute \p name exists.
     * @param name The name of the plugin's attribute.
     * @returns true \p name exists
     */
    inline bool attrExists(const char* name) const;
    //! @copydoc attrExists(const char*) const
    inline bool attrExists(const QString& name) const;

protected:
    Trial* m_trial;

    AbstractPlugin() = default;
    ~AbstractPlugin() = default;

private:
    const Attributes* m_attrs;

    bool setup(Trial& trial, const Attributes& attrs);
};

/************************************************************************
   AbstractPlugin: Inline member functions
 ************************************************************************/

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
