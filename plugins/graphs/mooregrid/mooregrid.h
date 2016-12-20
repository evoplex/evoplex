/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MOORE_GRID_H
#define MOORE_GRID_H

#include <QObject>
#include <QVariantHash>
#include <QVector>

#include "../src/core/abstractagent.h"
#include "../src/core/abstractgraph.h"

class MooreGrid: public AbstractGraph
{
    Q_OBJECT
    Q_INTERFACES(AbstractGraph)

public:
    MooreGrid(const QString &name);

    bool init(QVector<AbstractAgent*> agents, QVariantHash graphParams);

    void resetLinks();

    bool buildCoordinates();

    QVariantHash getGraphParams() const;

private:
    // graph parameters
    int m_width;
    int m_height;

    QVector<AbstractAgent*> calcMooreNeighborhood(const int id) const;
};

// This class will be loaded as a plugin.
class PGMooreGrid: public QObject, public IPluginGraph
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.evoplex.IPluginGraph")
    Q_INTERFACES(IPluginGraph)

public:
    IModel* create() { return qobject_cast<AbstractGraph*>(new MooreGrid(name()));  }
    const QString& name() { return "mooreGrid"; }
    const QString description() { return "Regular lattice grid with moore neighbourhood. That is, each agent has eight neighbours."; }
    QString author() { return "Marcos Cardinot"; }
};

#endif // MOORE_GRID_H
