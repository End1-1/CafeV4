#ifndef LOGWRITER_H
#define LOGWRITER_H

#include <QTimer>
#include <QProcess>

class LogWriter : public QObject
{
    Q_OBJECT

public:
    LogWriter(const QString &processName);
    void setDb(const QString &logPath, const QString &dbName, const QString &dbUser, const QString &dbPass);
    QStringList m_argv;

private:
    QString m_processName;
    QProcess *m_process;
    QTimer m_timer;

public slots:
    void timeout();
};

#endif // LOGWRITER_H
