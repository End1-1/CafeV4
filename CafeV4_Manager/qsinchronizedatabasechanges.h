#ifndef QSINCHRONIZEDATABASECHANGES_H
#define QSINCHRONIZEDATABASECHANGES_H

#include "qbasesqlwindow.h"

namespace Ui {
class QSinchronizeDatabaseChanges;
}

class QSinchronizeDatabaseChanges : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit QSinchronizeDatabaseChanges(QWidget *parent = 0);
    ~QSinchronizeDatabaseChanges();
    virtual void actionRefresh();

private slots:
    void on_chSelectAll_clicked(bool checked);
    void updateThreadStatus(const QString &status, int row);
    void updateThreadRecord(int recordsLeft, int totalRecords, int row);
    void threadFinished();

private:
    Ui::QSinchronizeDatabaseChanges *ui;
    QMap<QString, QStringList > m_databases;
    QStringList m_mainDb;
    int m_runningThread;
    bool updateMenu(QSqlQuery &qSrc, QStringList &sqlQueries, int &a);
    int updateEmployes(QSqlQuery &qSrc, QStringList &sqlQueries, int a);
    int upDiscounts(QSqlQuery &qSrc, QStringList &sqlQueries, int a);
};

#endif // QSINCHRONIZEDATABASECHANGES_H
