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
#include <QFontDatabase>
#include <QScopedPointer>
#include <QStringBuilder>
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
    if (!qstrcmp(argv[1], "-version")) {
        printf("%s-%s\n", EVOPLEX_VERSION, EVOPLEX_RELEASE);
        return 0;
    }

    QScopedPointer<QCoreApplication> coreApp(createApp(argc, argv));

    QCoreApplication::setOrganizationName("Evoplex");
    QCoreApplication::setOrganizationDomain("https://evoplex.org");
    QCoreApplication::setApplicationName("Evoplex");
    QCoreApplication::setApplicationVersion(EVOPLEX_VERSION "-" EVOPLEX_RELEASE);

    // Enables high-DPI scaling in Qt on supported platforms
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling, true);

    evoplex::Logger::init();

    const QString copyright = "Copyright (C) 2016-" % QDate::currentDate().toString("yyyy");
    const QString authors = "Marcos Cardinot et al.";

    const QString copyrightLine = copyright % " - " % authors;
    const QString versionLine = QCoreApplication::applicationName() % " " %
                                QCoreApplication::applicationVersion() % " - " %
                                QCoreApplication::organizationDomain();

    const int maxLength = qMax(versionLine.size(), copyrightLine.size());
    qInfo() << qPrintable(" " % QString().fill('-', maxLength+2));
    qInfo() << qPrintable(QString("[ %1 ]").arg(versionLine.leftJustified(maxLength, ' ')));
    qInfo() << qPrintable(QString("[ %1 ]").arg(copyrightLine.leftJustified(maxLength, ' ')));
    qInfo() << qPrintable(" " % QString().fill('-', maxLength+2));
    qInfo() << "Writing log file to:" << QDir::toNativeSeparators(evoplex::Logger::logFileName());

    // init application
    evoplex::MainApp mainApp;

    int result = -1;
    auto app = qobject_cast<QApplication*>(coreApp.data());
    if (app) {
        QStringList roboto = { "Regular", "Bold", "Italic", "Light", "Medium" };
        for (const QString& f : roboto) {
            int r = QFontDatabase::addApplicationFont(":/fonts/fonts/Roboto/Roboto-"+f+".ttf");
            if (r < 0) { qWarning() << "unable to add font: Roboto-" + f; }
        };
        QFont font("Roboto Regular");
        font.setPixelSize(14); // body 2
        font.setLetterSpacing(QFont::AbsoluteSpacing, 0.25);
        font.setStyleHint(QFont::SansSerif);
        app->setFont(font);

        QPixmap pixmap(":icons/splash.svg");
        QSplashScreen splash(pixmap, Qt::WindowStaysOnTopHint);

        splash.show();
        app->processEvents();

        splash.showMessage(copyright % "\n" % authors,
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

        // load dark style sheet
        QFile styleSheet(":/stylesheets/stylesheets/dark.qss");
        if (styleSheet.open(QIODevice::Text | QIODevice::Unbuffered | QIODevice::ReadOnly)) {
            app->setStyleSheet(styleSheet.readAll());
            styleSheet.close();
        }

        evoplex::MainGUI gui(&mainApp);
        splash.finish(&gui);
        gui.show();
        splash.close();
        result = app->exec();
    } else {
        // start console application
        // TODO: handle args
    }

    evoplex::Logger::deinit();

    return result;
}
