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

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QDir>
#include <QString>
#include <QFile>
#include <QMutex>

namespace evoplex {

// This class was mostly based on the StelLogger.hpp from Stellarium
// https://github.com/Stellarium/stellarium/blob/master/src/StelLogger.hpp
class Logger
{
public:
    static void init();

    static void deinit();

    // Handler for qDebug() and friends. Writes message to log file at $USERDIR/log.txt and echoes to stderr.
    static void debugLogHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& str);

    // Write the message plus a newline to the log file at $USERDIR/log.txt.
    // @param msg message to write.
    // If you call this function the message will be only in the log file,
    // not on the console like with qDebug().
    static void writeLog(QString msg);

    static const QString& log() { return m_log; }

    static QString logFileName() { return m_logFile.fileName(); }

    static QString logDir() { return m_logDir.absolutePath(); }

private:
    static QFile m_logFile;
    static QString m_log;
    static QMutex m_fileMutex;
    static QDir m_logDir;
    static QString m_logDateFormat;
};
} // evoplex


/******************************************************************************
    Compiler version and architecture
******************************************************************************/
// stringfy intergers
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
// version
#if defined __clang__
    #define COMPILER_VERSION "Clang " STR(__clang_major__) "." STR(__clang_minor__) "." STR(__clang_patchlevel__)
# elif defined __GNUC__
    #ifdef __MINGW32__
        #define COMPILER_NAME "MinGW GCC "
    #else
        #define COMPILER_NAME "GCC "
    #endif
    #define COMPILER_VERSION COMPILER_NAME STR(__GNUC__) "." STR(__GNUC_MINOR__) "." STR(__GNUC_PATCHLEVEL__)
#elif defined _MSC_VER
    #if _MSC_VER == 1600
        #define COMPILER_VERSION "MSVC++ 10.0 (Visual Studio 2010)"
    #elif _MSC_VER == 1700
        #define COMPILER_VERSION "MSVC++ 11.0 (Visual Studio 2012)"
    #elif _MSC_VER == 1800
        #define COMPILER_VERSION "MSVC++ 12.0 (Visual Studio 2013)"
    #elif _MSC_VER == 1900
        #define COMPILER_VERSION "MSVC++ 14.0 (Visual Studio 2015)"
    #elif _MSC_VER == 1910
        #define COMPILER_VERSION "MSVC++ 14.1 (Visual Studio 2017 v15.0)"
    #elif _MSC_VER == 1911
        #define COMPILER_VERSION "MSVC++ 14.11 (Visual Studio 2017 v15.3)"
    #elif _MSC_VER == 1912
        #define COMPILER_VERSION "MSVC++ 14.12 (Visual Studio 2017 v15.5)"
    #elif _MSC_VER == 1913
        #define COMPILER_VERSION "MSVC++ 14.13 (Visual Studio 2017 v15.6)"
    #elif _MSC_VER == 1914
        #define COMPILER_VERSION "MSVC++ 14.14 (Visual Studio 2017 v15.7)"
    #else
        #define COMPILER_VERSION "Unknown MSVC++ version " STR(_MSC_VER)
    #endif
#else
    #define COMPILER_VERSION "Unknown"
#endif
// architecture
#if defined(__LP64__) || defined(_WIN64)
    #define COMPILER_ARCHITECTURE "64-bit"
#else
    #define COMPILER_ARCHITECTURE "32-bit"
#endif


#endif // LOGGER_HPP
