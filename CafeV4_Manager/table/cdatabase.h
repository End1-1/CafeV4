#ifndef CDATABASE_H
#define CDATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QMap>

class CDatabase : public QObject
{
    Q_OBJECT
private:
    friend class CTableModel;
    static QString fCounter;
    QString fDatabaseNumber;
    QSqlDatabase fDatabase;
    QSqlQuery *fQuery;
    int fColumnCount;
    QMap<QString, int> fFieldsMapName;
    QMap<int, QString> fFieldsMapIndex;
    QString lastQuery();
    void init();
public:
    CDatabase();
    CDatabase(const QString &dbPath, const QString &dbUser, const QString &dbPass, const QString &dbConn = "");
    void setConnection(const QString &dbPath, const QString &dbUser, const QString &dbPass, const QString &dbConn = "");
    ~CDatabase();
    QList<QList<QVariant> > fRawData;
    bool prepare(const QString &sqlQuery);
    void bindValue(const QString &name, const QVariant &value);
    bool exec();
    bool exec(const QString &sqlQuery);
    inline QString columnNameByIndex(int index) const {return fFieldsMapIndex[index];}
    inline int columnIndexByName(const QString &field) {return fFieldsMapName[field];}
    static void log(const QString &error);
signals:
    void dataLoaded(CDatabase *);
};

#endif // CDATABASE_H
