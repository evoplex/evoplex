/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef MOORE_GRID_H
#define MOORE_GRID_H

#include <QPair>
#include <vector>

#include <evoplex/plugininterfaces.h>

namespace evoplex {
class CustomGraph: public AbstractGraph
{
public:
    CustomGraph(const QString &name);
    bool init();
    void reset();

private:
    // graph parameters
    enum GraphAttr { FilePath };
    QString m_filePath;
};
}

#endif // MOORE_GRID_H
