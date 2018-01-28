/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef GRAPHSETTINGS_H
#define GRAPHSETTINGS_H

#include <QDialog>

#include "maingui.h"
#include "core/experiment.h"

class Ui_GraphSettings;

namespace evoplex
{

class GraphSettings : public QDialog
{
    Q_OBJECT

public:
    explicit GraphSettings(MainGUI* mainGUI, Experiment* exp, QWidget* parent);
    ~GraphSettings();

    int agentAttr() const;
    int edgeAttr() const;
    inline ColorMap* agentCMap() const { return m_agentCMap; }
    inline ColorMap* edgeCMap() const { return m_edgeCMap; }

signals:
    void agentAttrUpdated(int);
    void edgeAttrUpdated(int);
    void agentCMapUpdated(ColorMap* cmap);
    void edgeCMapUpdated(ColorMap* cmap);

private slots:
    void updateAgentCMap();
    void slotAgentCMapName(const QString& name);
    void setAgentAttr(int attrIdx);

    void updateEdgeCMap();
    void slotEdgeCMapName(const QString& name);
    void setEdgeAttr(int attrIdx);

private:
    Ui_GraphSettings* m_ui;
    const ColorMapMgr* m_cmMgr;
    const AttributesSpace m_agentAttrSpace;
    const AttributesSpace m_edgeAttrSpace;
    ColorMap* m_agentCMap;
    ColorMap* m_edgeCMap;
};

} // evoplex
#endif // GRAPHSETTINGS_H
