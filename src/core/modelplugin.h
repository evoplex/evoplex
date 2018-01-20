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

    inline const std::vector<QString>& agentAttrNames() const { return m_agentAttrNames; }
    inline const AttributesSpace& agentAttrSpace() const { return m_agentAttrSpace; }

    inline const std::vector<QString>& edgeAttrNames() const { return m_edgeAttrNames; }
    inline const AttributesSpace& edgeAttrSpace() const { return m_edgeAttrSpace; }

    inline const std::vector<QString>& modelAttrNames() const { return m_modelAttrNames; }
    inline const AttributesSpace& modelAttrSpace() const { return m_modelAttrSpace; }

private:
    IPluginModel* m_factory;

    QVector<QString> m_supportedGraphs;
    QVector<QString> m_customOutputs;

    AttributesSpace m_agentAttrSpace;
    std::vector<QString> m_agentAttrNames;

    AttributesSpace m_edgeAttrSpace;
    std::vector<QString> m_edgeAttrNames;

    AttributesSpace m_modelAttrSpace;
    std::vector<QString> m_modelAttrNames;
};
}

#endif // MODELPLUGIN_H
