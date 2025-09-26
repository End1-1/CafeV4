#include "logwriter.h"
#include <QMutex>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDateTime>

QMutex fMutex;

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

void LogWriter::write(const QString &file, const QString &session, const QString &message)
{
#ifdef QT_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ") << session << message.left(1000);
#else
    if (file == LogWriterLevel::special) {
        return;
    }
#endif;
    QMutexLocker ml(&fMutex);
    writeToFile(file, session, message);
    if (file != LogWriterLevel::verbose) {
        writeToFile(LogWriterLevel::verbose, session, message);
    }
}

void LogWriter::writeToFile(const QString &fileName, const QString &session, const QString &message)
{
    QString fnpath = QString("%1/%2/%3/Logs").arg(QDir::tempPath(), _APPLICATION_, "FastFDatabase");
    QString fn = fnpath + QString("/%4_%5.log").arg(QDate::currentDate().toString("dd_MM_yyyy"), fileName);
    QDir().mkpath(QDir().absoluteFilePath(fnpath));
    QFile file(fn);
    if (file.open(QIODevice::Append)) {
        file.write(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm:ss ").toUtf8());
        if (!session.isEmpty()) {
            file.write(session.toUtf8());
            file.write(" ");
        }
        file.write(message.toUtf8());
        file.write("\r\n");
        file.close();
    }
}
