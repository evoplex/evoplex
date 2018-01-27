/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PLUGINSPAGE_H
#define PLUGINSPAGE_H

#include <QWidget>

#include "maingui.h"
#include "core/mainapp.h"

class Ui_PluginsPage;

namespace evoplex {
class PluginsPage : public QWidget
{
    Q_OBJECT

public:
    explicit PluginsPage(MainGUI* mainGUI);
    ~PluginsPage();

private slots:
    void importPlugin();
    void rowSelectionChanged();

private:
    enum TableCols {
        TYPE = 0,
        UID = 1,
        NAME = 2
    };

    Ui_PluginsPage* m_ui;
    MainApp* m_mainApp;

    void insertRow(const AbstractPlugin *plugin);

    void loadHtml(const GraphPlugin* plugin);
    void loadHtml(const ModelPlugin* plugin);
};
}
#endif // PLUGINSPAGE_H
