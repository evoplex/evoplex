/**
 * Copyright (C) 2017 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef PLUGINSWIDGET_H
#define PLUGINSWIDGET_H

#include <QWidget>

#include "core/mainapp.h"

class Ui_PluginsWidget;

namespace evoplex {
class PluginsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PluginsWidget(MainApp* mainApp, QWidget *parent = 0);
    ~PluginsWidget();

private slots:
    void importPlugin();
    void rowSelectionChanged();

private:
    enum TableCols {
        TYPE = 0,
        UID = 1,
        NAME = 2
    };

    enum PluginType {
        Graph,
        Model
    };

    Ui_PluginsWidget* m_ui;
    MainApp* m_mainApp;

    void insertRow(const QString& uid, const QString& name, PluginType type);

    void loadHtml(const GraphPlugin* plugin);
    void loadHtml(const ModelPlugin* plugin);
};
}
#endif // PLUGINSWIDGET_H
