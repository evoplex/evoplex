/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

#include "graphwidget.h"

namespace evoplex
{

class GraphView : public GraphWidget
{
public:
    explicit GraphView(MainGUI* mainGUI, Experiment* exp, ExperimentWidget* parent);

protected:
    void paintEvent(QPaintEvent*) override;
    virtual const Agent* selectAgent(const QPoint& pos) const;

private:
    struct Cache {
        Agent* agent = nullptr;
        QPointF xy;
        std::vector<QLineF> edges;
    };
    std::vector<Cache> m_cache;

    int m_edgeAttr;
    ColorMap* m_edgeCMap;
    float m_edgeSizeRate;

    bool m_showAgents;
    bool m_showEdges;

    virtual int refreshCache();
};

} // evoplex
#endif // GRAPHVIEW_H
