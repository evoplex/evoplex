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

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QProcess>
#include <QSslSocket>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QSysInfo>
#ifdef Q_OS_WIN
  #include <Windows.h>
#endif

#include "logger.h"
#include "constants.h"
#include "../config.h"

namespace evoplex
{

Logger* Logger::m_instance = new Logger;

void Logger::init()
{
    m_logDir = QDir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!m_logDir.exists()) {
        m_logDir.mkpath(".");
    }

    m_logDateFormat = "yyyy-MM-dd_HHmmss";
    const QString currDate = QDateTime::currentDateTime().toString(m_logDateFormat);
    const QString logFilename = m_logDir.absoluteFilePath("log_" % currDate % ".txt");
    m_logFile.setFileName(logFilename);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text | QIODevice::Unbuffered)) {
        qWarning() << "ERROR! Unable to write the log file." << logFilename;
        return;
    }

    qInstallMessageHandler(Logger::debugLogHandler);

    writeLog(currDate);
    writeLog(QString("Evoplex version: %1").arg(qApp->applicationVersion()));
    writeLog(QString("Built on %1 (branch: %2 commit: %3)")
             .arg(EVOPLEX_BUILDDATE, EVOPLEX_GIT_BRANCH, EVOPLEX_GIT_COMMIT_HASH));

    writeLog(QString("Operating System: %1").arg(QSysInfo::prettyProductName()));
    writeLog(QString("Built with %1").arg(COMPILER_VERSION));
    writeLog(QString("Qt runtime version: %1").arg(qVersion()));
    writeLog(QString("Qt compilation version: %1").arg(QT_VERSION_STR));
    writeLog(QString("Architecture: %1").arg(COMPILER_ARCHITECTURE));

    writeLog(QString("SslSupport: %1").arg(QSslSocket::supportsSsl()));
    writeLog(QString("SslLibraryBuildVersion: %1").arg(QSslSocket::sslLibraryBuildVersionString()));
    writeLog(QString("SslLibraryRuntimeVersion: %1").arg(QSslSocket::sslLibraryVersionString()));

    // write memory and CPU info
#ifdef Q_OS_LINUX
    QFile infoFile("/proc/meminfo");
    if (infoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!infoFile.peek(1).isEmpty()) {
            QString line = infoFile.readLine();
            line.chop(1);
            if (line.startsWith("Mem") || line.startsWith("SwapTotal")) {
                writeLog(line);
            }
        }
        infoFile.close();
    } else {
        writeLog("Could not get memory info.");
    }

    infoFile.setFileName("/proc/cpuinfo");
    if (infoFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!infoFile.peek(1).isEmpty()) {
            QString line = infoFile.readLine();
            line.chop(1);
            if (line.startsWith("model name") || line.startsWith("cpu MHz")) {
                writeLog(line);
            }
        }
        infoFile.close();
    } else {
        writeLog("Could not get CPU info.");
    }

    QProcess lspci;
    lspci.start("lspci -v", QIODevice::ReadOnly);
    lspci.waitForFinished(300);
    const QString pciData(lspci.readAll());
    QStringList pciLines = pciData.split('\n', QString::SkipEmptyParts);
    for (int i = 0; i < pciLines.size(); ++i) {
        if (pciLines.at(i).contains("VGA compatible controller")) {
            writeLog(pciLines.at(i));
            i++;
            while(i < pciLines.size() && pciLines.at(i).startsWith('\t')) {
                if (pciLines.at(i).contains("Kernel driver in use")) {
                    writeLog(pciLines.at(i).trimmed());
                } else if(pciLines.at(i).contains("Kernel modules")) {
                    writeLog(pciLines.at(i).trimmed());
                }
                i++;
            }
        }
    }

#elif defined Q_OS_WIN
    if (QSysInfo::WindowsVersion >= QSysInfo::WV_XP) {
        MEMORYSTATUSEX statex;
        statex.dwLength = sizeof (statex);
        GlobalMemoryStatusEx(&statex);
        writeLog(QString("Total physical memory: %1 MB").arg(statex.ullTotalPhys/(1024<<10)));
        writeLog(QString("Available physical memory: %1 MB").arg(statex.ullAvailPhys/(1024<<10)));
        writeLog(QString("Physical memory in use: %1%").arg(statex.dwMemoryLoad));
        #ifndef _WIN64
        // This always reports about 8TB on Win64, not really useful to show.
        writeLog(QString("Total virtual memory: %1 MB").arg(statex.ullTotalVirtual/(1024<<10)));
        writeLog(QString("Available virtual memory: %1 MB").arg(statex.ullAvailVirtual/(1024<<10)));
        #endif
    } else {
        writeLog("Windows version too old to get memory info.");
    }

    HKEY hKey = nullptr;
    DWORD dwType = REG_DWORD;
    DWORD numVal = 0;
    DWORD dwSize = sizeof(numVal);

    // iterate over the processors listed in the registry
    QString procKey = "Hardware\\Description\\System\\CentralProcessor";
    LONG lRet = ERROR_SUCCESS;
    int i;
    for(i = 0; lRet == ERROR_SUCCESS; ++i) {
        lRet = RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                    qPrintable(QString("%1\\%2").arg(procKey).arg(i)),
                    0, KEY_QUERY_VALUE, &hKey);

        if(lRet == ERROR_SUCCESS) {
            if(RegQueryValueExA(hKey, "~MHz", nullptr, &dwType, (LPBYTE)&numVal, &dwSize) == ERROR_SUCCESS) {
                writeLog(QString("Processor speed: %1 MHz").arg(numVal));
            } else {
                writeLog("Could not get processor speed.");
            }
        }

        // can you believe this trash?
        dwType = REG_SZ;
        char nameStr[512];
        DWORD nameSize = sizeof(nameStr);

        if (lRet == ERROR_SUCCESS) {
            if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, &dwType, (LPBYTE)&nameStr, &nameSize) == ERROR_SUCCESS) {
                writeLog(QString("Processor name: %1").arg(nameStr));
            } else {
                writeLog("Could not get processor name.");
            }
        }

        RegCloseKey(hKey);
    }
    if(i == 0) {
        writeLog("Could not get processor info.");
    }

#elif defined Q_OS_MACOS
    QProcess systemProfiler;
    systemProfiler.start("/usr/sbin/system_profiler -detailLevel mini SPHardwareDataType SPDisplaysDataType");
    systemProfiler.waitForStarted();
    systemProfiler.waitForFinished();
    const QString systemData(systemProfiler.readAllStandardOutput());
    QStringList systemLines = systemData.split('\n', QString::SkipEmptyParts);
    for (int i = 0; i<systemLines.size(); ++i) {
        if(systemLines.at(i).contains("Model")) {
            writeLog(systemLines.at(i).trimmed());
        }
        if(systemLines.at(i).contains("Processor")) {
            writeLog(systemLines.at(i).trimmed());
        }
        if(systemLines.at(i).contains("Memory")) {
            writeLog(systemLines.at(i).trimmed());
        }
        if(systemLines.at(i).contains("VRAM")) {
            writeLog(systemLines.at(i).trimmed());
        }
    }

#elif defined Q_OS_BSD4
    QProcess dmesg;
    dmesg.start("/sbin/dmesg", QIODevice::ReadOnly);
    dmesg.waitForStarted();
    dmesg.waitForFinished();
    const QString dmesgData(dmesg.readAll());
    QStringList dmesgLines = dmesgData.split('\n', QString::SkipEmptyParts);
    for (int i = 0; i<dmesgLines.size(); ++i) {
        if (dmesgLines.at(i).contains("memory")) {
            writeLog(dmesgLines.at(i).trimmed());
        }
        if (dmesgLines.at(i).contains("CPU")) {
            writeLog(dmesgLines.at(i).trimmed());
        }
        if (dmesgLines.at(i).contains("VGA")) {
            writeLog(dmesgLines.at(i).trimmed());
        }
    }

#endif
}

void Logger::destroy()
{
    const QDateTime sevenDaysAgo = QDateTime::currentDateTime().addDays(-7);
    const QStringList oldLogs = m_logDir.entryList(QStringList("log_*.txt"), QDir::Files);
    for (const QString& log : oldLogs) {
        QString logDate = log;
        logDate.remove("log_");
        logDate.remove(".txt");
        if (QDateTime::fromString(logDate, m_logDateFormat) <= sevenDaysAgo) {
            qInfo() << "removing old log file:" << m_logDir.absoluteFilePath(log);
            m_logDir.remove(log);
        }
    }

    qInstallMessageHandler(nullptr);
    m_logFile.close();
    delete m_instance;
    m_instance = nullptr;
}

void Logger::debugLogHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    uint color = 39; // default color
    QString formattedMsg = msg;
    if (type == QtInfoMsg) {
        color = 34; // blue
    } else {
        // format full function path to class::function only
        QString classFunc(ctx.function);
        classFunc.remove(0, classFunc.lastIndexOf("evoplex::") + 9);
        classFunc.remove(classFunc.lastIndexOf("("), classFunc.lastIndexOf(")"));

        formattedMsg = "[" % classFunc  % "] " % formattedMsg;
        if (type == QtWarningMsg) {
            color = 35; // magenta
            emit (m_instance->warning(formattedMsg));
        } else if (type == QtCriticalMsg || type == QtFatalMsg) {
            color = 31; // red
            formattedMsg = formattedMsg % " (" % ctx.file % ":" % QString::number(ctx.line) % ")";
        }
    }

    fprintf(stderr, "\033[0;%um%s\033[0m\n", color, qPrintable(formattedMsg));
    fflush(stderr);

    m_instance->writeLog(formattedMsg);
}

void Logger::writeLog(QString msg)
{
    if (!msg.endsWith('\n')) {
        msg.append(QLatin1Char('\n'));
    }

    m_fileMutex.lock();
    m_logFile.write(qPrintable(msg), msg.size());
    m_log = m_log % msg;
    m_fileMutex.unlock();
}

} // evoplex
