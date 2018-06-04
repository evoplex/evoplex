/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2018 - Marcos Cardinot <marcos@cardinot.net>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QDialog>

#include "core/output.h"
#include "core/mainapp.h"

class Ui_OutputWidget;

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
