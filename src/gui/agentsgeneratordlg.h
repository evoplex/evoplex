/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef AGENTSGENERATORDLG_H
#define AGENTSGENERATORDLG_H

#include <QDialog>

#include "ui_agentsgeneratordlg.h"
#include "core/agentsgenerator.h"
#include "core/valuespace.h"

namespace evoplex
{

class AgentsGeneratorDlg : public QDialog
{
    Q_OBJECT

public:
    explicit AgentsGeneratorDlg(const AttributesSpace& agentAttrsSpace, AgentsGenerator* ag, QWidget *parent);
    ~AgentsGeneratorDlg();

    QString readCommand();

private slots:
    void slotSaveAs();

private:
    Ui_AgentsGeneratorDlg* m_ui;
    const AttributesSpace& m_agentAttrsSpace;

    void fill(AgentsGenerator* ag);
};

} // evoplex
#endif // AGENTSGENERATORDLG_H
