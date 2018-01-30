/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRIDVIEW_H
#define GRIDVIEW_H

#include "graphwidget.h"

namespace evoplex
{

class GridView : public GraphWidget
{
    Q_OBJECT

public:
    explicit GridView(MainGUI* mainGUI, Experiment* exp, QWidget* parent);

protected:
    void paintEvent(QPaintEvent*) override;
    virtual const Agent* selectAgent(const QPoint& pos) const;

private:
    struct Cache {
        Agent* agent = nullptr;
        QRectF rect;
    };
    std::vector<Cache> m_cache;

    virtual int refreshCache();
};

} // evoplex
#endif // GRIDVIEW_H
