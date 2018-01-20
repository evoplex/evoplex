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

    inline const std::vector<QString>& graphAttrNames() const { return m_graphAttrRange.min.names(); }

    inline const AttributesSpace& graphAttrSpace() const { return m_graphAttrSpace; }
    inline const AttributesRange& graphAttrRange() const { return m_graphAttrRange; }

private:
    IPluginGraph* m_factory;

    AttributesSpace m_graphAttrSpace;
    AttributesRange m_graphAttrRange;
};
}

#endif // GRAPHPLUGIN_H
