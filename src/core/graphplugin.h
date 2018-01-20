/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPHPLUGIN_H
#define GRAPHPLUGIN_H

#include "abstractplugin.h"
#include "attributes.h"
#include "plugininterfaces.h"

namespace evoplex {
class GraphPlugin : public AbstractPlugin
{
public:
    GraphPlugin(const QObject* instance, const QJsonObject* metaData);
    virtual ~GraphPlugin();

    inline AbstractGraph* create() const { return m_factory->create(); }

    inline const std::vector<QString>& graphAttrNames() const { return m_graphAttrNames; }

    inline const AttributesSpace& graphAttrSpace() const { return m_graphAttrSpace; }

private:
    IPluginGraph* m_factory;

    std::vector<AbstractGraph::GraphType> m_validGraphTypes;
    AttributesSpace m_graphAttrSpace;
    std::vector<QString> m_graphAttrNames;
};
}

#endif // GRAPHPLUGIN_H
