/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

// std stuff, used in PRNG
#include <functional>
#include <iostream>
#include <random>
#include <time.h>

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QScopedPointer>
#include <QStyleFactory>

#include "core/mainapp.h"
#include "gui/maingui.h"

static std::mt19937 _mt_eng_; //  Mersenne Twister engine

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

    // seed PRG
    // http://stackoverflow.com/a/15509942
    std::array<int, 624> seed_data;
    std::random_device r;
    std::generate_n(seed_data.data(), seed_data.size(), std::ref(r));
    std::seed_seq seq(std::begin(seed_data), std::end(seed_data));
    _mt_eng_ = std::mt19937(seq);
    qsrand(time(NULL)); // just in case... =D

    // init application
    MainApp mainApp;

    int result = -1;
    QApplication* app = qobject_cast<QApplication*>(coreApp.data());
    if (app) {
        // start GUI application
        app->setStyle(QStyleFactory::create("Fusion"));
        MainGUI gui(&mainApp);
        gui.show();
        result = app->exec();
    } else {
        // start console application
        // TODO: handle args
    }

    return result;
}
