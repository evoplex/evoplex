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
    GraphPlugin(const QObject* instance, const QJsonObject* metaData, const QString& libPath);
    virtual ~GraphPlugin();

    inline AbstractGraph* create() const { return m_factory->create(); }

private:
    IPluginGraph* m_factory;
    std::vector<AbstractGraph::GraphType> m_validGraphTypes;
};
}

#endif // GRAPHPLUGIN_H
