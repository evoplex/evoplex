/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QDialog>

#include "core/output.h"
#include "core/mainapp.h"
#include "ui_outputwidget.h"

namespace evoplex {

class OutputWidget : public QDialog
{
    Q_OBJECT

public:
    explicit OutputWidget(const ModelPlugin* modelPlugin, QWidget* parent = 0);
    ~OutputWidget();

    inline void setTrialIds(const std::vector<int>& trialIds) { m_trialIds = trialIds; }

    void fill(std::vector<Output*> outputs);

protected:
    virtual void hideEvent(QHideEvent* event);

signals:
    void closed(std::vector<Output*> newOutputs);

private slots:
    void slotEntityChanged(bool isAgent);
    void slotFuncChanged(int idx);
    void slotAdd();

private:
    Ui_OutputWidget* m_ui;
    const ModelPlugin* m_modelPlugin;
    std::vector<int> m_trialIds;

    std::vector<Output*> m_allOutputs;
    std::vector<Output*> m_newOutputs;
};
}
#endif // OUTPUTWIDGET_H
