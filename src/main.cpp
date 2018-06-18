/**
 *  This file is part of Evoplex.
 *
 *  Evoplex is a multi-agent system for networks.
 *  Copyright (C) 2016 - Marcos Cardinot <marcos@cardinot.net>
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

#include <QApplication>
#include <QCoreApplication>
#include <QDate>
#include <QDebug>
#include <QDir>
#include <QScopedPointer>
#include <QSplashScreen>
#include <QStyleFactory>

#include "config.h"
#include "core/logger.h"
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

    QCoreApplication::setOrganizationName("Evoplex");
    QCoreApplication::setOrganizationDomain("https://evoplex.github.io");
    QCoreApplication::setApplicationName("Evoplex");
    QCoreApplication::setApplicationVersion(EVOPLEX_VERSION "-" EVOPLEX_RELEASE);

    evoplex::Logger::init();

    const QString copyright = "Copyright (C) 2016-" + QDate::currentDate().toString("yyyy");
    const QString authors = "Marcos Cardinot et al.";
    const QString copyrightLine = copyright + " - " + authors;
    const QString versionLine = QString("%1 %2 - %3").arg(QCoreApplication::applicationName())
                                                     .arg(QCoreApplication::applicationVersion())
                                                     .arg(QCoreApplication::organizationDomain());

    int maxLength = qMax(versionLine.size(), copyrightLine.size());
    qDebug() << qPrintable(QString(" %1").arg(QString().fill('-', maxLength+2)));
    qDebug() << qPrintable(QString("[ %1 ]").arg(versionLine.leftJustified(maxLength, ' ')));
    qDebug() << qPrintable(QString("[ %1 ]").arg(copyrightLine.leftJustified(maxLength, ' ')));
    qDebug() << qPrintable(QString(" %1").arg(QString().fill('-', maxLength+2)));
    qDebug() << "Writing log file to:" << QDir::toNativeSeparators(evoplex::Logger::logFileName());

    // init application
    evoplex::MainApp mainApp;

    int result = -1;
    QApplication* app = qobject_cast<QApplication*>(coreApp.data());
    if (app) {
        QPixmap pixmap(":icons/splash.svg");
        QSplashScreen* splash = new QSplashScreen(pixmap, Qt::WindowStaysOnTopHint);
        QFont splashFont = qApp->font();
        splashFont.setPixelSize(12);
        splash->setFont(splashFont);

        splash->show();
        app->processEvents();

        splash->showMessage(QString("%1\n%2").arg(copyright).arg(authors),
                            Qt::AlignHCenter | Qt::AlignBottom, QColor(66,133,244));

        // start GUI application
        app->setStyle(QStyleFactory::create("Fusion"));

        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor(51,51,51));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor(51,51,51));
        darkPalette.setColor(QPalette::AlternateBase, QColor(25,25,25));
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

        evoplex::MainGUI gui(&mainApp);
        splash->finish(&gui);
        gui.show();
        result = app->exec();
    } else {
        // start console application
        // TODO: handle args
    }

    evoplex::Logger::deinit();

    return result;
}
