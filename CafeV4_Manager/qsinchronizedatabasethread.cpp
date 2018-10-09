#include "qsinchronizedatabasethread.h"
#include <QSqlDatabase>
#include <QSqlError>
#include <QVariant>

QSinchronizeDatabaseThread::QSinchronizeDatabaseThread(const QString &dbSrcName, const QString &dbSrcUser, const QString &dbSrcPassword,
                                                       const QString &dbDstName, const QString &dbDstUser, const QString &dbDstPassword,
                                                       int dstDbId, int row, QStringList costumQueries) :
    m_dbSrcName(dbSrcName),
    m_dbSrcUser(dbSrcUser),
    m_dbSrcPassword(dbSrcPassword),
    m_dbDstName(dbDstName),
    m_dbDstUser(dbDstUser),
    m_dbDstPassword(dbDstPassword),
    m_costumQueries(costumQueries),
    m_dstDbId(dstDbId),
    m_row(row)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

QSinchronizeDatabaseThread::~QSinchronizeDatabaseThread()
{
    QSqlDatabase::removeDatabase("DSTDB" + m_dbDstName);
    QSqlDatabase::removeDatabase("SRCDB" + QString::number(m_row));
    emit sigDone();
}

void QSinchronizeDatabaseThread::run()
{
    QSqlDatabase dbDst = QSqlDatabase::addDatabase("QIBASE", "DSTDB" + m_dbDstName);
    QSqlDatabase dbSrc = QSqlDatabase::addDatabase("QIBASE", "SRCDB" + QString::number(m_row));
    dbDst.setDatabaseName(m_dbDstName);
    dbDst.setUserName(m_dbDstUser);
    dbDst.setPassword(m_dbDstPassword);
    dbSrc.setDatabaseName(m_dbSrcName);
    dbSrc.setUserName(m_dbSrcUser);
    dbSrc.setPassword(m_dbSrcPassword);
    emit sigStatus(tr("Connecting to destination database."), m_row);
    if (!dbDst.open()) {
        emit sigStatus(tr("Cannot connect to destination database.") + dbDst.lastError().text(), m_row);
        return;
    }
    emit sigStatus(tr("Connecting to source database."), m_row);
    if (!dbSrc.open()) {
        emit sigStatus(tr("Cannot connect to source database.") + dbSrc.lastError().text(), m_row);
        return;
    }
    QStringList sqlQueries = m_costumQueries;
    QSqlQuery qSrc(dbSrc);
    qSrc.prepare("select id_update, id_delete from sys_databases where id=:id");
    qSrc.bindValue(":id", m_dstDbId);
    qSrc.exec();
    int a = 0;
    int del = 0;
    while (qSrc.next()) {
        a = qSrc.value(0).toInt();
        del = qSrc.value(1).toInt();
    }
    emit sigStatus(tr("Get list of deleted records"), m_row);
    qSrc.prepare("select id, table_name, id_delete from sys_sync_del where id>:id order by id");
    qSrc.bindValue(":id", del);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("delete from %1 where id=%2")
                          .arg(qSrc.value(1).toString())
                          .arg(qSrc.value(2).toInt()));
        del = qSrc.value(0).toInt();
    }
    if (!updateMenu(qSrc, sqlQueries, a)) {
        emit sigStatus(tr("SQL error in src database."), m_row);
        return;
    }
    if ((a = updateEmployes(qSrc, sqlQueries, a)) < 0) {
        emit sigStatus(tr("SQL error in src database."), m_row);
        return;
    }
    if ((a = upDiscounts(qSrc, sqlQueries, a)) < 0) {
        emit sigStatus(tr("SQL error in src database."), m_row);
        return;
    }
    if (sqlQueries.count()) {
        QStringList generators;
        generators << "gen_employes_id"
                   << "gen_me_dishes_id"
                   << "gen_me_dishes_menu_id"
                   << "gen_me_group_types_id"
                   << "gen_me_group_types_members_id"
                   << "gen_me_recipes_id"
                   << "gen_st_storages_id";
        for (QStringList::const_iterator it = generators.begin(); it != generators.end(); it++) {
            qSrc.exec("select gen_id(" + *it + ",0) from rdb$database");
            if (qSrc.next())
                 sqlQueries.insert(0, "alter sequence " + *it + " restart with " + qSrc.value(0).toString());
        }
    }
    emit sigStatus(tr("Applying updates"), m_row);
    int i = 0, c = sqlQueries.count();
    QSqlQuery qDst(dbDst);
    bool err = false;
    emit sigRecordsLeft(c, c, m_row);
    for (QStringList::const_iterator sql = sqlQueries.begin(); sql != sqlQueries.end(); sql++) {
        i++;
        if (!(i % 30))
            emit sigRecordsLeft(c - i, c, m_row);

        if (!qDst.exec(*sql)) {
            emit sigStatus(qDst.lastError().databaseText() + ": " + *sql, m_row);
            dbDst.rollback();
            dbDst.close();
            dbSrc.close();
            err = true;
            break;
        }

    }
    if (err)
        return;
    dbDst.commit();
    qSrc.prepare("update sys_databases set id_update=:id_update, id_delete=:id_delete where id=:id");
    qSrc.bindValue(":id_update", a);
    qSrc.bindValue(":id_delete", del);
    qSrc.bindValue(":id", m_dstDbId);
    qSrc.exec();
    emit sigRecordsLeft(0, c, m_row);
    emit sigStatus(tr("Mission complete."), m_row);
}

bool QSinchronizeDatabaseThread::updateMenu(QSqlQuery &qSrc, QStringList &sqlQueries, int &a)
{
    int A = a;

    emit sigStatus(tr("Get menu names"), m_row);
    qSrc.prepare("select id, name, a from me_menus where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into me_menus (id, name) values (%1, '%2') matching(id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toString()));
        a = a < qSrc.value(2).toInt() ?  qSrc.value(2).toInt() : a;
    }

    emit sigStatus(tr("Get menu groups of groups"), m_row);
    qSrc.prepare("select id, name, img_link, menu_id, a from me_group_types where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into me_group_types (id, name, img_link, menu_id) values (%1, '%2', '%3', %4) matching(id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toString())
                          .arg(qSrc.value(2).toString())
                          .arg(qSrc.value(3).toInt()));
        a = a < qSrc.value(4).toInt() ?  qSrc.value(4).toInt() : a;
    }

    qSrc.prepare("select id, group_id, member_id, a from me_group_types_members where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into me_group_types_members (id, group_id, member_id) values (%1, %2, %3) matching(id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt())
                          .arg(qSrc.value(2).toInt()));
        a = a < qSrc.value(3).toInt() ?  qSrc.value(3).toInt() : a;
    }

    emit sigStatus(tr("Get menu types"), m_row);
    qSrc.prepare("select id, name, img_link, adgcode, a from me_types where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into me_types (id, name, img_link, adgcode) values "
                                  "(%1, '%2', '%3', '%4') matching (id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toString())
                          .arg(qSrc.value(2).toString())
                          .arg(qSrc.value(3).toString()));
        a = a < qSrc.value(4).toInt() ?  qSrc.value(4).toInt() : a;
    }

    emit sigStatus(tr("Get dishes names"), m_row);
    qSrc.prepare("select id, type_id, name, payment_mod, color, queue, remind, img_link, descr, a from me_dishes where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into me_dishes (id, type_id, name, payment_mod, color, queue, remind, img_link, descr) "
                                  "values (%1, %2, '%3', %4, %5, %6, %7, '%8', '%9') matching (id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt())
                          .arg(qSrc.value(2).toString())
                          .arg(qSrc.value(3).toString())
                          .arg(qSrc.value(4).toInt())
                          .arg(qSrc.value(5).toInt())
                          .arg(qSrc.value(6).toInt())
                          .arg(qSrc.value(7).toString())
                          .arg(qSrc.value(8).toString()));
        a = a < qSrc.value(9).toInt() ?  qSrc.value(9).toInt() : a;
    }

    emit sigStatus(tr("Get menu"), m_row);
    qSrc.prepare("select id, menu_id, dish_id, price, store_id, print_schema, state_id, print1, print2, a from me_dishes_menu where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into me_dishes_menu (id, menu_id, dish_id, price, store_id, print_schema, state_id, print1, print2) values "
                          "(%1, %2, %3, %4, %5, %6, %7, '%8', '%9') matching(id)")
                .arg(qSrc.value(0).toInt())
                .arg(qSrc.value(1).toInt())
                .arg(qSrc.value(2).toInt())
                .arg(qSrc.value(3).toFloat())
                .arg(qSrc.value(4).toInt())
                .arg(qSrc.value(5).toInt())
                .arg(qSrc.value(6).toInt())
                .arg(qSrc.value(7).toString())
                .arg(qSrc.value(8).toString()));
        a = a < qSrc.value(9).toInt() ?  qSrc.value(9).toInt() : a;
    }

    emit sigStatus (tr("Get print schemas"), m_row);
    qSrc.prepare("select id, name, queue, a from me_printer_schema where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into me_printer_schema (id, name, queue) values (%1, '%2', %3) matching(id);")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toString())
                          .arg(qSrc.value(2).toInt()));
        a = a < qSrc.value(3).toInt() ?  qSrc.value(3).toInt() : a;
    }

    emit sigStatus(tr("Get recipes"), m_row);
    qSrc.prepare("select id, dish_id, goods_id, qty, a from me_recipes where a>:a ");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("insert into me_recipes (id, dish_id, goods_id, qty) values (%1, %2, %3, %4);")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt())
                          .arg(qSrc.value(2).toInt())
                          .arg(qSrc.value(3).toFloat()));
        a = a < qSrc.value(4).toInt() ?  qSrc.value(4).toInt() : a;
    }

    emit sigStatus(tr("Get food names"), m_row);
    qSrc.prepare("select id, group_id, unit_id, name, price, auto_out, a from food_names where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into food_names (id, group_id, unit_id, name, price, auto_out) "
                                  "values (%1, %2, %3, '%4', %5, %6);")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt())
                          .arg(qSrc.value(2).toInt())
                          .arg(qSrc.value(3).toString())
                          .arg(qSrc.value(4).toFloat())
                          .arg(qSrc.value(5).toInt()));
        a = a < qSrc.value(6).toInt() ?  qSrc.value(6).toInt() : a;
    }

    emit sigStatus(tr("Get dish presents"), m_row);
    qSrc.prepare("select id, set_id, a from me_dishes_present where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("insert into me_dishes_present (id, set_id) values (%1, %2)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt()));
        a = a < qSrc.value(2).toInt() ?  qSrc.value(2).toInt() : a;
    }

    emit sigStatus(tr("Get storages"), m_row);
    qSrc.prepare("select id, name, a from st_storages where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into st_storages (id, name) values (%1, '%2') matching (id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toString()));
        a = a < qSrc.value(2).toInt() ? qSrc.value(2).toInt() : a;
    }

    emit sigStatus(tr("Get remove reasons"), m_row);
    qSrc.prepare("select id, name, a from o_remove_reason where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into o_remove_reason (id, name) values (%1, '%2') matching (id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toString()));
        a = a < qSrc.value(2).toInt() ? qSrc.value(2).toInt() : a;
    }

    return true;
}

int QSinchronizeDatabaseThread::upDiscounts(QSqlQuery &qSrc, QStringList &sqlQueries, int a)
{
    int A = a;
    emit sigStatus(tr("GetDiscounts"), m_row);
    qSrc.prepare("select id, state_id, name, mod_id, info, code, a from costumers_names where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into costumers_names (id, state_id, name, mod_id, info, code) "
                                  "values (%1, %2, '%3', %4, '%5', '%6') matching (id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt())
                          .arg(qSrc.value(2).toString())
                          .arg(qSrc.value(3).toInt())
                          .arg(qSrc.value(4).toString())
                          .arg(qSrc.value(5).toString()));
        a = a < qSrc.value(6).toInt() ? qSrc.value(6).toInt() : a;
    }
    return a;
}

int QSinchronizeDatabaseThread::updateEmployes(QSqlQuery &qSrc, QStringList &sqlQueries, int a)
{
    int A = a;
    emit sigStatus(tr("Get employes groups"), m_row);
    qSrc.prepare("select id, name, a from employes_group where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into employes_group (id, name) values (%1, '%2') matching (id);")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toString()));
        a = a < qSrc.value(2).toInt() ? qSrc.value(2).toInt() : a;
    }
    emit sigStatus(tr("Get employes names"), m_row);
    qSrc.prepare("select id, state_id, group_id, fname, lname, login, password, a from employes where a>:a");
    qSrc.bindValue(":a", A);
    qSrc.exec();
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into employes (id, state_id, group_id, fname, lname, login, password) "
                                  "values (%1, %2, %3, '%4', '%5', '%6', '%7') matching(id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt())
                          .arg(qSrc.value(2).toInt())
                          .arg(qSrc.value(3).toString())
                          .arg(qSrc.value(4).toString())
                          .arg(qSrc.value(5).toString())
                          .arg(qSrc.value(6).toString()));
        a = a < qSrc.value(7).toInt() ? qSrc.value(7).toInt() : a;
    }
    emit sigStatus(tr("Get employes roles"), m_row);
//    qSrc.prepare("select id, group_id, module_id, key_name, key_value, a from employes_access where a>:a");
//    qSrc.bindValue(":a", A);
//    qSrc.exec();
//    while (qSrc.next()) {
//        sqlQueries.append(QString("update or insert into employes_access (id, group_Id, module_id, key_name, key_value) "
//                                  "values (%1, %2, %3, '%4', '%5') matching(id)")
//                          .arg(qSrc.value(0).toInt())
//                          .arg(qSrc.value(1).toInt())
//                          .arg(qSrc.value(2).toInt())
//                          .arg(qSrc.value(3).toString())
//                          .arg(qSrc.value(4).toString()));
//        a = a < qSrc.value(5).toInt() ? qSrc.value(5).toInt() : a;
//    }
    qSrc.prepare("select id, group_id, module_id, key_name, key_value, a from employes_access");
    qSrc.exec();
    sqlQueries.append("delete from employes_access");
    while (qSrc.next()) {
        sqlQueries.append(QString("update or insert into employes_access (id, group_Id, module_id, key_name, key_value) "
                                  "values (%1, %2, %3, '%4', '%5') matching(id)")
                          .arg(qSrc.value(0).toInt())
                          .arg(qSrc.value(1).toInt())
                          .arg(qSrc.value(2).toInt())
                          .arg(qSrc.value(3).toString())
                          .arg(qSrc.value(4).toString()));
        a = a < qSrc.value(5).toInt() ? qSrc.value(5).toInt() : a;
    }
    return a;
}
