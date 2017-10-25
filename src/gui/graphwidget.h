/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QDockWidget>

#include "core/experiment.h"

namespace evoplex {

class GraphWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit GraphWidget(Experiment* exp, QWidget* parent = 0);
    ~GraphWidget();

protected:
    void mouseMoveEvent(QMouseEvent* e);
    void mousePressEvent(QMouseEvent* e);

private:
    AbstractGraph* m_graph;
    bool m_isValid;
    float m_radius;
    int m_scale;

    QPoint m_posClicked;

    void paintEvent(QPaintEvent*) override;
};
}

#endif // GRAPHWIDGET_H
