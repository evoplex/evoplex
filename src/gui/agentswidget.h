/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef AGENTSWIDGET_H
#define AGENTSWIDGET_H

#include <QDialog>

#include "ui_agentswidget.h"
#include "core/agentsgenerator.h"
#include "core/valuespace.h"

namespace evoplex
{

class AgentsWidget : public QDialog
{
    Q_OBJECT

public:
    explicit AgentsWidget(const AttributesSpace& agentAttrsSpace, AgentsGenerator* ag, QWidget *parent = 0);
    ~AgentsWidget();

private slots:
    void slotSaveAs();

signals:
    void closed(const QString& cmd);

private:
    Ui_AgentsWidget* m_ui;
    const AttributesSpace& m_agentAttrsSpace;

    void fill(AgentsGenerator* ag);
    QString readCommand();

};

} // evoplex
#endif // AGENTSWIDGET_H
