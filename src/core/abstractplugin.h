/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACTPLUGIN_H
#define ABSTRACTPLUGIN_H

#include <QJsonObject>
#include <QString>

#include "attributes.h"

namespace evoplex {
class AbstractPlugin
{
public:
    AbstractPlugin(const QJsonObject* metaData);

    inline const bool isValid() const { return m_isValid; }
    inline const QString& id() const { return m_id; }
    inline const QString& author() const { return m_author; }
    inline const QString& name() const { return m_name; }
    inline const QString& description() const { return m_descr; }

protected:
    bool m_isValid;
    AttributesSpace attrsSpace(const QJsonObject* metaData, const QString& name) const;

private:
    QString m_id;
    QString m_author;
    QString m_name;
    QString m_descr;
};
}
#endif // ABSTRACTPLUGIN_H
