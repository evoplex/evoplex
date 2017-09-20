/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QScopedPointer>
#include <QStyleFactory>

#include "core/mainapp.h"
#include "gui/maingui.h"

QCoreApplication* createApp(int& argc, char* argv[])
{
    for (int i = 1; i < argc; ++i) {
        if (!qstrcmp(argv[i], "-no-gui")) {
            return new QCoreApplication(argc, argv);
        }
    }
    return new QApplication(argc, argv);
}

int main(int argc, char* argv[])
{
    QScopedPointer<QCoreApplication> coreApp(createApp(argc, argv));

    qDebug() <<" *************************************************\n"
             << "* Copyright (C) 2016 - Marcos Cardinot          *\n"
             << "* @author Marcos Cardinot <mcardinot@gmail.com> *\n"
             << "*************************************************\n";

    // init application
    MainApp mainApp;

    int result = -1;
    QApplication* app = qobject_cast<QApplication*>(coreApp.data());
    if (app) {
        // start GUI application
        app->setStyle(QStyleFactory::create("Fusion"));

        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(51,51,51));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(25,25,25));
        darkPalette.setColor(QPalette::AlternateBase, QColor(51,51,51));
        darkPalette.setColor(QPalette::ToolTipBase, QColor(255,255,220));
        darkPalette.setColor(QPalette::ToolTipText, Qt::black);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor(51,51,51));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);
        app->setPalette(darkPalette);

        MainGUI gui(&mainApp);
        gui.show();
        result = app->exec();
    } else {
        // start console application
        // TODO: handle args
    }

    return result;
}
