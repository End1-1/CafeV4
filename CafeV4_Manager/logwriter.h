#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QTimer>
#include <QProcess>

namespace LogWriterLevel {
    const QString verbose = "verbose";
    const QString warning = "warning";
    const QString errors = "errors";
    const QString special = "special";
}


class LogWriter : public QObject
{
    Q_OBJECT

public:
    LogWriter(const QString &processName);
    void setDb(const QString &logPath, const QString &dbName, const QString &dbUser, const QString &dbPass);
    QStringList m_argv;
    static void write(const QString &file, const QString &session, const QString &message);

private:
    QString m_processName;
    QProcess *m_process;
    QTimer m_timer;
    static void writeToFile(const QString &fileName, const QString &session, const QString &message);

public slots:
    void timeout();
};

#endif // LOGWRITER_H
