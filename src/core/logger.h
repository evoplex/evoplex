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

namespace evoplex
{
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

private:
    static QFile m_logFile;
    static QString m_log;
    static QMutex m_fileMutex;
    static QDir m_logDir;
};
} // evoplex
#endif // LOGGER_HPP
