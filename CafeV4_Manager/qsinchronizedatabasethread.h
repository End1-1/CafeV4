#ifndef QSINCHRONIZEDATABASETHREAD_H
#define QSINCHRONIZEDATABASETHREAD_H

#include <QThread>
#include <QSqlQuery>

class QSinchronizeDatabaseThread : public QThread
{
    Q_OBJECT

public:
    QSinchronizeDatabaseThread(const QString &dbSrcName, const QString &dbSrcUser, const QString &dbSrcPassword,
                               const QString &dbDstName, const QString &dbDstUser, const QString &dbDstPassword,
                               int dstDbId, int row, QStringList costumQueries);
    ~QSinchronizeDatabaseThread();

protected:
    virtual void run();

private:
    QString m_dbSrcName;
    QString m_dbSrcUser;
    QString m_dbSrcPassword;
    QString m_dbDstName;
    QString m_dbDstUser;
    QString m_dbDstPassword;
    QStringList m_costumQueries;
    int m_dstDbId;
    int m_row;

    bool updateMenu(QSqlQuery &qSrc, QStringList &sqlQueries, int &a);
    int upDiscounts(QSqlQuery &qSrc, QStringList &sqlQueries, int a);
    int updateEmployes(QSqlQuery &qSrc, QStringList &sqlQueries, int a);
signals:
    void sigStatus(const QString &status, int row);
    void sigRecordsLeft(int recordsLeft, int recordsCount, int row);
    void sigDone();
};

#endif // QSINCHRONIZEDATABASETHREAD_H
