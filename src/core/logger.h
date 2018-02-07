/**
 * Copyright (C) 2018 - Marcos Cardinot
 * @author Marcos Cardinot <mcardinot@gmail.com>
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <QString>
#include <QFile>
#include <QMutex>

namespace evoplex
{
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

    static const QString& log() { return log; }

    static QString logFileName() { return logFile.fileName(); }

private:
    static QFile logFile;
    static QString log;
    static QMutex fileMutex;

    static QString getMsvcVersionString(int ver);
};
} // evoplex
#endif // LOGGER_HPP
