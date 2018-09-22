/* Evoplex <https://evoplex.org>
 * Copyright (C) 2016-present - Marcos Cardinot <marcos@cardinot.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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
class Logger : public QObject
{
    Q_OBJECT

public:
    static Logger* instance() { return m_instance; }

    // Handler for qDebug() and friends. Writes message to log file at $USERDIR/log.txt and echoes to stderr.
    static void debugLogHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& str);

    void init();

    void destroy();

    // Write the message plus a newline to the log file at $USERDIR/log.txt.
    // @param msg message to write.
    // If you call this function the message will be only in the log file,
    // not on the console like with qDebug().
    void writeLog(QString msg);

    const QString& log() { return m_log; }

    QString logFileName() { return m_logFile.fileName(); }

    QString logDir() { return m_logDir.absolutePath(); }

signals:
    void warning(const QString& msg);

private:
    static Logger* m_instance;
    QFile m_logFile;
    QString m_log;
    QMutex m_fileMutex;
    QDir m_logDir;
    QString m_logDateFormat;
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
