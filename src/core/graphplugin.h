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

#ifndef GRAPHPLUGIN_H
#define GRAPHPLUGIN_H

#include <vector>

#include "enum.h"
#include "plugin.h"

namespace evoplex {
class GraphPlugin : public Plugin
{
    friend class Plugin;
    friend class MainApp;

public:
    using GraphTypes = std::vector<GraphType>;

    virtual ~GraphPlugin() = default;

    inline const GraphTypes& validGraphTypes() const;
    inline bool supportsEdgeAttrsGen() const;

protected:
    explicit GraphPlugin(QPluginLoader* loader, const QString& libPath);

private:
    bool m_supportsEdgeAttrsGen;
    std::vector<GraphType> m_validGraphTypes;
};

/************************************************************************
   GraphPlugin: Inline member functions
 ************************************************************************/

inline const GraphPlugin::GraphTypes& GraphPlugin::validGraphTypes() const
{ return m_validGraphTypes; }

inline bool GraphPlugin::supportsEdgeAttrsGen() const
{ return m_supportsEdgeAttrsGen; }

} //evoplex
#endif // GRAPHPLUGIN_H
