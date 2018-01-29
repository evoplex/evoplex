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

    inline const QVector<QString>& customOutputs() const { return m_customOutputs; }
    inline const QVector<QString>& supportedGraphs() const { return m_supportedGraphs; }
    inline bool graphIsSupported(const QString& graphId) const { return m_supportedGraphs.empty() || m_supportedGraphs.contains(graphId); }

    inline const std::vector<QString>& agentAttrNames() const { return m_agentAttrNames; }
    inline const AttributesSpace& agentAttrSpace() const { return m_agentAttrSpace; }

    inline const std::vector<QString>& edgeAttrNames() const { return m_edgeAttrNames; }
    inline const AttributesSpace& edgeAttrSpace() const { return m_edgeAttrSpace; }

private:
    IPluginModel* m_factory;

    QVector<QString> m_supportedGraphs;
    QVector<QString> m_customOutputs;

    AttributesSpace m_agentAttrSpace;
    std::vector<QString> m_agentAttrNames;

    AttributesSpace m_edgeAttrSpace;
    std::vector<QString> m_edgeAttrNames;
};
}

#endif // MODELPLUGIN_H
