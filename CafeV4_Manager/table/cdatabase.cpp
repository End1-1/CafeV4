#include "cdatabase.h"
#include <QMutexLocker>
#include <QFile>
#include <QDir>
#include <QDateTime>
#include <QMessageBox>
#include <QSqlRecord>
#include <QVariant>

#ifdef QT_DEBUG
#include <QDebug>
#endif

#define _DBDRIVER_ "QIBASE"

QString CDatabase::fCounter = "0";
QMutex mutex;

void CDatabase::log(const QString &error)
{
    QMutexLocker ml(&mutex);
    QFile file(QDir::tempPath() + "/cdatabase.log");
    file.open(QIODevice::Append);
    file.write(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss zzz-- ").toLatin1());
    file.write(" ");
    file.write(error.toLatin1());
    file.write("\r\n");
    file.close();
    ml.unlock();
#ifdef QT_DEBUG
    qDebug() << QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss zzz") << error;
#endif
}

QString CDatabase::lastQuery()
{
    QString sql = fQuery->lastQuery();
    QMapIterator<QString, QVariant> it(fQuery->boundValues());
    while (it.hasNext()) {
        it.next();
        QVariant value = it.value();
        switch (it.value().type()) {
        case QVariant::String:
            value = QString("'%1'").arg(value.toString().replace("'", "''"));
            break;
        case QVariant::Date:
            value = QString("'%1'").arg(value.toDate().toString("dd.MM.yyyy"));
            break;
        case QVariant::DateTime:
            value = QString("'%1'").arg(value.toDateTime().toString("dd.MM.yyyy HH:mm:ss"));
            break;
        case QVariant::Time:
            value = QString("'%1'").arg(value.toTime().toString("HH:mm:ss"));
            break;
        case QVariant::Double:
            value = QString("%1").arg(value.toDouble());
            break;
        case QVariant::Int:
            value = QString("%1").arg(value.toInt());
            break;
        default:
            break;
        }
        sql.replace(it.key(), value.toString());
    }
    return sql;
}

void CDatabase::init()
{
    QMutexLocker ml(&mutex);
    fDatabaseNumber = fCounter;
    fCounter = QString::number(fCounter.toInt() + 1);
    ml.unlock();
    fDatabase = QSqlDatabase::addDatabase(_DBDRIVER_, fDatabaseNumber);
    fQuery = 0;
}

CDatabase::CDatabase() :
    QObject()
{
    init();
}

CDatabase::CDatabase(const QString &dbPath, const QString &dbUser, const QString &dbPass, const QString &dbConn)
{
    setConnection(dbPath, dbUser, dbPass, dbConn);
}

void CDatabase::setConnection(const QString &dbPath, const QString &dbUser, const QString &dbPass, const QString &dbConn)
{
    init();
    fDatabase.setDatabaseName(dbPath);
    fDatabase.setUserName(dbUser);
    fDatabase.setPassword(dbPass);
    fDatabase.setConnectOptions(dbConn);
}

CDatabase::~CDatabase()
{
    fDatabase = QSqlDatabase::addDatabase(_DBDRIVER_);
    QSqlDatabase::removeDatabase(fDatabaseNumber);
}

bool CDatabase::prepare(const QString &sqlQuery)
{
#ifdef QT_DEBUG
    log("Query started");
#endif
    fRawData.clear();
    fFieldsMapName.clear();
    if (!fDatabase.isOpen()) {
        if (!fDatabase.open()) {
            log(fDatabase.lastError().databaseText());
            QMessageBox::critical(0, tr("Database connection error"), fDatabase.lastError().databaseText());
            return false;
        }
    }
#ifdef QT_DEBUG
    log("Connection established");
#endif
    if (!fQuery) {
        fQuery = new QSqlQuery(fDatabase);
    }
    if (!fQuery->prepare(sqlQuery)) {
        log(fQuery->lastError().databaseText());
        QMessageBox::critical(0, tr("SQL error"), fQuery->lastError().databaseText());
        return false;
    }
    return true;
}

void CDatabase::bindValue(const QString &name, const QVariant &value)
{
    if (!fQuery) {
        QMessageBox::critical(0, tr("Bind error"), tr("QSqlQuery is null"));
        return;
    }
    fQuery->bindValue(name, value);
}

bool CDatabase::exec()
{
#ifdef QT_DEBUG
    log("Query exec started");
#endif
    if (!fQuery) {
        QMessageBox::critical(0, tr("SQL error"), tr("QSqlQuery is null"));
        return false;
    }
    if (!fQuery->exec()) {
        log(fQuery->lastError().databaseText());
        QMessageBox::critical(0, tr("SQL error"), fQuery->lastError().databaseText());
        return false;
    }
    QSqlRecord rec = fQuery->record();
    fColumnCount = rec.count();
    fFieldsMapIndex.clear();
    fFieldsMapName.clear();
    for (int i = 0; i < rec.count(); i++) {
        fFieldsMapName[rec.fieldName(i).toUpper()] = i;
        fFieldsMapIndex[i] = rec.fieldName(i).toUpper();
    }
#ifdef QT_DEBUG
    log("Fetch records to buffer");
#endif
    while (fQuery->next()) {
        QList<QVariant> r;
        for (int i = 0; i < fColumnCount; i++) {
            r << fQuery->value(i);
        }
        fRawData.append(r);
    }
#ifdef QT_DEBUG
    log("End of fetch records to buffer");
#endif
    emit dataLoaded(this);
#ifdef QT_DEBUG
    log(lastQuery());
#endif
    fDatabase.close();
    delete fQuery;
    fQuery = 0;
    return true;
}

bool CDatabase::exec(const QString &sqlQuery)
{
    if (!prepare(sqlQuery)) {
        return false;
    }
    return exec();
}
