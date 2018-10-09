#include "logwriter.h"

LogWriter::LogWriter(const QString &processName)
{
    m_processName = processName;
    m_process = new QProcess();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
    m_timer.start(60000);
}

void LogWriter::setDb(const QString &logPath, const QString &dbName, const QString &dbUser, const QString &dbPass)
{
    m_argv.clear();
    m_argv.append(logPath);
    m_argv.append(dbName);
    m_argv.append(dbUser);
    m_argv.append(dbPass);
}

void LogWriter::timeout()
{
    return;
    m_process->start(m_processName, m_argv);
}
