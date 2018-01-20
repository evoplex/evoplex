/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MODELPLUGIN_H
#define MODELPLUGIN_H

#include <QVector>

#include "abstractplugin.h"
#include "attributes.h"
#include "plugininterfaces.h"

namespace evoplex {
class ModelPlugin : public AbstractPlugin
{
public:
    ModelPlugin(const QObject* instance, const QJsonObject* metaData);
    virtual ~ModelPlugin();

    AbstractModel* create() const { return m_factory->create(); }

    inline const QVector<QString>& supportedGraphs() const { return m_supportedGraphs; }
    inline const QVector<QString>& customOutputs() const { return m_customOutputs; }

    inline const std::vector<QString>& agentAttrNames() const { return m_agentAttrRange.min.names(); }
    inline const AttributesSpace& agentAttrSpace() const { return m_agentAttrSpace; }
    inline const AttributesRange& agentAttrRange() const { return m_agentAttrRange; }

    inline const std::vector<QString>& edgeAttrNames() const { return m_edgeAttrRange.min.names(); }
    inline const AttributesSpace& edgeAttrSpace() const { return m_edgeAttrSpace; }
    inline const AttributesRange& edgeAttrRange() const { return m_edgeAttrRange; }

    inline const std::vector<QString>& modelAttrNames() const { return m_modelAttrRange.min.names(); }
    inline const AttributesSpace& modelAttrSpace() const { return m_modelAttrSpace; }
    inline const AttributesRange& modelAttrRange() const { return m_modelAttrRange; }

private:
    IPluginModel* m_factory;

    QVector<QString> m_supportedGraphs;
    QVector<QString> m_customOutputs;

    AttributesSpace m_agentAttrSpace;
    AttributesRange m_agentAttrRange;

    AttributesSpace m_edgeAttrSpace;
    AttributesRange m_edgeAttrRange;

    AttributesSpace m_modelAttrSpace;
    AttributesRange m_modelAttrRange;
};
}

#endif // MODELPLUGIN_H
