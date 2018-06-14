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
#include <QDir>
#include <QProcess>
#include <QStandardPaths>
#include <QSysInfo>
#ifdef Q_OS_WIN
  #include <windows.h>
#endif

#include "logger.h"

namespace evoplex
{

QFile Logger::m_logFile;
QString Logger::m_log;
QMutex Logger::m_fileMutex;

void Logger::init()
{
    QDir dir(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation));
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    m_logFile.setFileName(dir.absoluteFilePath("log.txt"));
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text | QIODevice::Unbuffered)) {
        qWarning() << "[Logger] ERROR! Unable to write the log file.";
        return;
    }

    qInstallMessageHandler(Logger::debugLogHandler);

    writeLog(QString("%1").arg(QDateTime::currentDateTime().toString(Qt::ISODate)));

    writeLog(QString("Operating System: %1").arg(QSysInfo::prettyProductName()));

    // write compiler version
#if defined __GNUC__ && !defined __clang__
    #ifdef __MINGW32__
        #define COMPILER "MinGW GCC"
    #else
        #define COMPILER "GCC"
    #endif
    writeLog(QString("Compiled using %1 %2.%3.%4").arg(COMPILER).arg(__GNUC__).arg(__GNUC_MINOR__).arg(__GNUC_PATCHLEVEL__));
#elif defined __clang__
    writeLog(QString("Compiled using %1 %2.%3.%4").arg("Clang").arg(__clang_major__).arg(__clang_minor__).arg(__clang_patchlevel__));
#elif defined _MSC_VER
    writeLog(QString("Compiled using %1").arg(getMsvcVersionString(_MSC_VER)));
#else
    writeLog("Unknown compiler");
#endif

    // write Qt version
    writeLog(QString("Qt runtime version: %1").arg(qVersion()));
    writeLog(QString("Qt compilation version: %1").arg(QT_VERSION_STR));

    // write addressing mode
#if defined(__LP64__) || defined(_WIN64)
    writeLog(QString("Addressing mode: 64-bit"));
#else
    writeLog("Addressing mode: 32-bit");
#endif

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

    HKEY hKey = NULL;
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
            if(RegQueryValueExA(hKey, "~MHz", NULL, &dwType, (LPBYTE)&numVal, &dwSize) == ERROR_SUCCESS) {
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
            if (RegQueryValueExA(hKey, "ProcessorNameString", NULL, &dwType, (LPBYTE)&nameStr, &nameSize) == ERROR_SUCCESS) {
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

void Logger::deinit()
{
    qInstallMessageHandler(0);
    m_logFile.close();
}

void Logger::debugLogHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg)
{
    QString formattedMessage = qFormatLogMessage(type, ctx, msg);
    if (formattedMessage.isNull()) {
        return;
    }

    fprintf(stderr, "%s\n", qPrintable(formattedMessage));
    fflush(stderr);

    writeLog(formattedMessage);
}

void Logger::writeLog(QString msg)
{
    if (!msg.endsWith('\n')) {
        msg.append(QLatin1Char('\n'));
    }

    m_fileMutex.lock();
    m_logFile.write(qPrintable(msg), msg.size());
    m_log += msg;
    m_fileMutex.unlock();
}

QString Logger::getMsvcVersionString(int ver)
{
    switch(ver) {
    case 1310:
        return "MSVC++ 7.1 (Visual Studio 2003)";
    case 1400:
        return "MSVC++ 8.0 (Visual Studio 2005)";
    case 1500:
        return "MSVC++ 9.0 (Visual Studio 2008)";
    case 1600:
        return "MSVC++ 10.0 (Visual Studio 2010)";
    case 1700:
        return "MSVC++ 11.0 (Visual Studio 2012)";
    case 1800:
        return "MSVC++ 12.0 (Visual Studio 2013)";
    case 1900:
        return "MSVC++ 14.0 (Visual Studio 2015)";
    case 1910:
        return "MSVC++ 14.1 (Visual Studio 2017)";
    default:
        return "unknown MSVC++ version";
    }
}

} // evoplex
