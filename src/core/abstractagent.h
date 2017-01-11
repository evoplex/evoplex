/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef ABSTRACT_AGENT_H
#define ABSTRACT_AGENT_H

#include <QVariantHash>
#include <QString>

class AbstractAgent
{
public:
    // constructors
    AbstractAgent();
    AbstractAgent(QVariantHash p): m_properties(p) {}

    inline AbstractAgent* clone() {
        return new AbstractAgent(m_properties);
    }

    inline const QVariantHash getProperties() {
        return m_properties;
    }

    inline const QVariant getProperty(const QString& name) {
        return m_properties.value(name);
    }

    inline void setProperty(const QString& name, const QVariant& value) {
        m_properties.insert(name, value);
    }

private:
    QVariantHash m_properties;
};

#endif // ABSTRACT_AGENT_H
