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
    explicit AbstractAgent() {}
    explicit AbstractAgent(QVariantHash attr): m_attributes(attr) {}

    inline AbstractAgent* clone() {
        return new AbstractAgent(m_attributes);
    }

    inline const QVariantHash getAttributes() {
        return m_attributes;
    }

    inline const QVariant getAttribute(const QString& name) {
        return m_attributes.value(name);
    }

    inline void setAttribute(const QString& name, const QVariant& value) {
        m_attributes.insert(name, value);
    }

private:
    QVariantHash m_attributes;
};

#endif // ABSTRACT_AGENT_H
