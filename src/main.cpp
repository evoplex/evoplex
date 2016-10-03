/**
 * Copyright (C) 2016 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#include "core/mainapp.h"
#include "gui/maingui.h"

#include <QApplication>
#include <QCoreApplication>
#include <QScopedPointer>
#include <QDebug>

// std stuff, used in PRNG
#include <functional>
#include <iostream>
#include <random>
#include <time.h>
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
    QScopedPointer<QCoreApplication> app(createApp(argc, argv));

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
    MainApp a;

    int result = -1;
    if (qobject_cast<QApplication*>(app.data())) {
       // start GUI version
       MainGUI gui;
       gui.show();
       result = app->exec();
    } else {
       // start non-GUI version
    }

    return result;
}
