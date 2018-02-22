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
    struct RowInfo {
        int id = -1;
        int equalToId = -1; // when Output* is the same, but with different inputs
    };

    // It creates a new Cache* for each row (which can take only one input).
    // We don't touch the initial Caches*.
    // If the user does not make changes, nothing is created and reject() is returned
    explicit OutputWidget(const ModelPlugin* modelPlugin, const std::vector<int>& trialIds, QWidget* parent,
                          const std::vector<Cache*> &init = std::vector<Cache*>());
    ~OutputWidget();

    inline std::map<int, Cache*>& caches() { return m_allCaches; }

protected:
    virtual void closeEvent(QCloseEvent*);

private slots:
    void slotEntityChanged(bool isAgent);
    void slotFuncChanged(int idx);
    void slotAdd();
    void slotClose(bool canceled);

private:
    enum FuncType {
        DefaultFunc,
        CustomFunc
    };

    Ui_OutputWidget* m_ui;
    const ModelPlugin* m_modelPlugin;
    std::vector<int> m_trialIds;
    bool m_hasChanges;

    DefaultOutput::Entity m_currEntity;
    QString m_currEntityStr;

    // we cannot rely on the row numbers because they might change.
    // that's why we use Ids in the RowInfo.
    // also, we need a map to keep the Caches in order
    std::map<int, Cache*> m_allCaches;

    void insertRow(const RowInfo rowInfo, const QString& funcStr, const FuncType funcType,
                   QString entityStr="", DefaultOutput::Entity entity=DefaultOutput::E_Agents,
                   QString attr="", QString input="");
};
} // evoplex

Q_DECLARE_METATYPE(evoplex::OutputWidget::RowInfo)

#endif // OUTPUTWIDGET_H
