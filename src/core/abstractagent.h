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
    explicit AbstractAgent() {}
    explicit AbstractAgent(QVariantHash attr)
        : m_attributes(attr), m_x(0), m_y(0) { m_attributes.squeeze(); }

    ~AbstractAgent() {}

    inline AbstractAgent clone() {
        return AbstractAgent(m_attributes);
    }

    inline const QVariantHash& getAttributes() const {
        return m_attributes;
    }

    inline const QVariant getAttribute(const QString& name) const {
        return m_attributes.value(name);
    }
    inline void setAttribute(const QString& name, const QVariant& value) {
        m_attributes.insert(name, value);
    }

    inline const int getId() const { return m_id; }
    inline void setId(int id) { m_id = id; }

    inline const int getX() const { return m_x; }
    inline void setX(int x) { m_x = x; }
    inline const int getY() const { return m_y; }
    inline void setY(int y) { m_y = y; }
    inline void setCoords(int x, int y) { setX(x); setY(y); }

private:
    int m_id;
    int m_x;
    int m_y;
    QVariantHash m_attributes;
};

#endif // ABSTRACT_AGENT_H
