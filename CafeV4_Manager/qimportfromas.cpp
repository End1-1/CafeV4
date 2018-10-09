#include "qimportfromas.h"
#include "ui_qimportfromas.h"
#include "dlgvaluepair.h"
#include "core.h"
#include <QMessageBox>

QImportFromAS::QImportFromAS(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::QImportFromAS)
{
    ui->setupUi(this);

    setWindowTitle(tr("AS synchronization"));

    m_actions << "actionSave" << "actionRefresh";

    ui->tblStore->addAction(ui->actionAdd);
    ui->tblStore->addAction(ui->actionEdit);
    ui->tblStore->addAction(ui->actionRemove);
    ui->tblFood->addAction(ui->actionAddFood);
    ui->tblFood->addAction(ui->actionEditFood);
    ui->tblFood->addAction(ui->actionRemoveFood);

    ui->tblStore->setColumnWidth(0, 150);
    ui->tblStore->setColumnWidth(1, 150);
    ui->tblFood->setColumnWidth(0, 150);
    ui->tblFood->setColumnWidth(1, 150);

    QDate d = QDate::currentDate().addMonths(-1);
    qDebug() << QString("01.%1.%2").arg(d.month(), 2, 10, QChar('0')).arg(d.year());
    qDebug() << QString("%1.%2.%3").arg(d.daysInMonth()).arg(d.month(), 2, 10, QChar('0')).arg(d.year());
    QDate d1 = QDate::fromString(QString("01.%1.%2").arg(d.month(), 2, 10, QChar('0')).arg(d.year()), DATE_FORMAT);
    QDate d2 = QDate::fromString(QString("%1.%2.%3").arg(d.daysInMonth()).arg(d.month(), 2, 10, QChar('0')).arg(d.year()), DATE_FORMAT);
    ui->deStart->setDate(d1);
    ui->deEnd->setDate(d2);

    m_sqlDrv->execSQL("select conn_str, username, pwd, database, sql_query from sys_as_conn");
    if (m_sqlDrv->m_query->next()) {
        ui->leConnectionString->setText(m_sqlDrv->valStr("CONN_STR"));
        ui->leUsername->setText(m_sqlDrv->valStr("USERNAME"));
        ui->lePassword->setText(m_sqlDrv->valStr("PWD"));
        ui->leDatabase->setText(m_sqlDrv->valStr("DATABASE"));
        ui->leQuery->setPlainText(m_sqlDrv->valStr("SQL_QUERY"));
    }

    m_sqlDrv->execSQL("select as_store, cafe_store from sys_as_store_conv order by id");
    while (m_sqlDrv->m_query->next())
        addRow(ui->tblStore, m_sqlDrv->valStr("AS_STORE"), m_sqlDrv->valStr("CAFE_STORE"));

    m_sqlDrv->execSQL("select as_food, cafe_food from sys_as_food_conv order by id");
    while (m_sqlDrv->m_query->next())
        addRow(ui->tblFood, m_sqlDrv->valStr("AS_FOOD"), m_sqlDrv->valStr("CAFE_FOOD"));

    m_sqlDrv->close();

}

QImportFromAS::~QImportFromAS()
{
    delete ui;
}

void QImportFromAS::actionSave()
{
    m_sqlDrv->m_db.transaction();
    m_sqlDrv->execSQL("delete from sys_as_conn");
    m_sqlDrv->prepare("insert into sys_as_conn values (gen_id(gen_sys_as_conn_id, 1), :conn_str, :username, :pass, :database, :sql_query)");
    m_sqlDrv->bind(":conn_str", ui->leConnectionString->text());
    m_sqlDrv->bind(":username", ui->leUsername->text());
    m_sqlDrv->bind(":pass", ui->lePassword->text());
    m_sqlDrv->bind(":database", ui->leDatabase->text());
    m_sqlDrv->bind(":sql_query", ui->leQuery->document()->toPlainText());
    m_sqlDrv->execSQL();

    m_sqlDrv->execSQL("delete from sys_as_store_conv");
    m_sqlDrv->prepare("insert into sys_as_store_conv values (gen_id(gen_sys_as_store_conv_id, 1), :as, :cafe4)");
    for (int i = 0; i < ui->tblStore->rowCount(); i++) {
        m_sqlDrv->bind(":as", ui->tblStore->item(i, 0)->text());
        m_sqlDrv->bind(":cafe4", ui->tblStore->item(i, 1)->text());
        m_sqlDrv->execSQL();
    }

    m_sqlDrv->execSQL("delete from sys_as_food_conv");
    m_sqlDrv->prepare("insert into sys_as_food_conv values (gen_id(gen_sys_as_food_conv_id,1), :as, :cafe4)");
    for (int i = 0; i < ui->tblFood->rowCount(); i++) {
        m_sqlDrv->bind(":as", ui->tblFood->item(i, 0)->text());
        m_sqlDrv->bind(":cafe4", ui->tblFood->item(i, 1)->text());
        m_sqlDrv->execSQL();
    }

    m_sqlDrv->close();
    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void QImportFromAS::actionRefresh()
{
    ui->tabWidget->setCurrentIndex(4);
    ui->txtLog->clear();
    log(tr("Synchronization started"));
    log(tr("Connecting to main database"));
    QSqlDrv dbFb(___ff_user->fullName, "clientdb");

    log(tr("Get convertions map"));
    QMap<QString, int> convStore;
    dbFb.execSQL("select as_store, cafe_store from sys_as_store_conv");
    while (dbFb.m_query->next())
        convStore[dbFb.valStr("AS_STORE")] = dbFb.valInt("CAFE_STORE");

    QMap<QString, int> convFood;
    dbFb.execSQL("select as_food, cafe_food from sys_as_food_conv");
    while (dbFb.m_query->next())
        convFood[dbFb.valStr("AS_FOOD")] = dbFb.valInt("CAFE_FOOD");

    if (!ui->chUpdateOnlyPrices->isChecked()) {
        log(tr("Deleting old synchronization data"));
        if (!dbFb.prepare("delete from st_documents where doc_date between :date1 and :date2 and comment like 'SYNC%'")) {
            log(tr("Deleting failed"));
            return;
        }
        dbFb.bind(":date1", ui->deStart->date());
        dbFb.bind(":date2", ui->deEnd->date());
        if (!dbFb.execSQL()) {
            log(tr("Deleting failed"));
            return;
        }
    }

    log(tr("Connecting to AS database"));
    QSqlDatabase dbAs = QSqlDatabase::addDatabase("QODBC", "def");
    dbAs.setDatabaseName(ui->leConnectionString->text());
    dbAs.setUserName(ui->leUsername->text());
    dbAs.setPassword(ui->lePassword->text());
    if (!dbAs.open()) {
        log(tr("Connecting to AS databases failed.") + dbAs.lastError().databaseText());
        return;
    }

    QStringList queriesAs = ui->leQuery->document()->toPlainText().arg(ui->leDatabase->text()).split(";");

    QMap<int, int> docType;
    docType[6] = 1;
    docType[7] = 2;
    docType[8] = 3;
    docType[17] = 5;

    QMap<QString, Document> docs;
    QMap<QString, QList<Record> > recs;
    QSqlQuery qAs(dbAs);
    log(tr("Get headers of documents"));
    if (!qAs.prepare(queriesAs.at(0))) {
        QMessageBox::critical(this, tr("SQL error"), qAs.lastError().databaseText());
        log(qAs.lastError().databaseText());
        dbAs.close();
        return;
    }
    qAs.bindValue(":date1", ui->deStart->date().toString("yyyy-MM-dd"));
    qAs.bindValue(":date2", ui->deEnd->date().toString("yyyy-MM-dd"));
    if (!qAs.exec()) {
        QMessageBox::critical(this, tr("SQL error"), qAs.lastError().databaseText());
        log(qAs.lastError().databaseText());
        dbAs.close();
        return;
    }
    while (qAs.next()) {
        Document d;
        d.isn = qAs.value(0).toString();
        d.num = qAs.value(1).toString();
        d.amount = qAs.value(2).toDouble();
        d.body = qAs.value(3).toString();
        d.type = docType[qAs.value(4).toInt()];
        d.date = qAs.value(5).toDate();
        getDocStorages(d, convStore);
        if (!d.storeIn && !d.storeOut) {
            QMessageBox::critical(this, tr("Convertor error"), tr("Storage doesnt exist in store map"));
            log(tr("Storage doesnt exist in store map") + d.body);
            dbAs.close();
            return;
        }
        docs[d.isn] = d;
        recs[d.isn] = QList<Record>();
    }

    log(tr("Get body of documents"));
    if (!qAs.prepare(queriesAs.at(1))) {
        QMessageBox::critical(this, tr("SQL error"), qAs.lastError().databaseText());
        log(qAs.lastError().databaseText());
        return;
    }
    qAs.bindValue(":date1", ui->deStart->date().toString("yyyy-MM-dd"));
    qAs.bindValue(":date2", ui->deEnd->date().toString("yyyy-MM-dd"));
    if (!qAs.exec()) {
        QMessageBox::critical(this, tr("SQL error"), qAs.lastError().databaseText());
        log(qAs.lastError().databaseText());
        return;
    }
    while (qAs.next()) {
        Record r;
        r.id = convFood.contains(qAs.value(1).toString()) ? convFood[qAs.value(1).toString()] : qAs.value(1).toInt();
        r.qty = qAs.value(2).toDouble();
        r.amount = qAs.value(3).toDouble();
        r.fdbcr = qAs.value(4).toInt();
        recs[qAs.value(0).toString()].append(r);
    }
    dbAs.close();

    log(tr("Process new documents"));
    if (!dbFb.openDB()) {
        log(tr("Cannot open destination database"));
        return;
    }
    int cnt = 1;
    for (QMap<QString, Document>::iterator di = docs.begin(); di != docs.end(); di++) {
        dbFb.m_db.transaction();
        int currDocId = dbFb.genId("GEN_ST_DOCUMENTS_ID");
        QString sql = QString("insert into st_documents values (%1, %2, 1, 1, '%3', %4, %5, %6, %7, %8, %9, %10, 'SYNC:%11', 0)")
                .arg(currDocId)
                .arg(di->type)
                .arg(di->date.toString(DATE_FORMAT))
                .arg(di->storeIn)
                .arg(di->storeOut)
                .arg(1).arg(1).arg(1).arg(0)
                .arg(QString::number(di->amount, 'f', 2))
                .arg(di->num);
        if (!dbFb.execSQL(sql)) {
            log(tr("SQL Error"));
            log(sql);
            return;
        }
        QList<Record> &r = recs[di->isn];
        bool odd = true;
        for (QList<Record>::iterator ri = r.begin(); ri != r.end(); ri++) {
            if (di->type == 3) {
                if (odd) {
                    odd = false;
                    continue;
                } else
                    odd = true;
            }
            if (di->type == 5) {
                if (!ri->fdbcr)
                    ri->qty *= -1;
            }
            sql = QString("insert into st_draft values (null, %1, %2, %3, %4, 0)")
                              .arg(currDocId)
                              .arg(ri->id)
                              .arg(QString::number(ri->qty, 'f', 3))
                              .arg(QString::number(ri->amount, 'f', 2));
            if (!dbFb.execSQL(sql)) {
                log(tr("SQL Error"));
                log(sql);
                return;
            }
            if (!ui->chNoPriceUPdate->isChecked()) {
                sql = QString("update food_names set price=%1 where id=%2")
                        .arg(ri->amount / ri->qty)
                        .arg(ri->id);
                dbFb.execSQL(sql);
            }
        }
        dbFb.m_db.commit();
        log(QString("%1 of %2").arg(cnt++).arg(docs.count()));
    }

    log(tr("All done"));
}

/*
void QImportFromAS::actionRefresh()
{
    ui->tabWidget->setCurrentIndex(4);
    ui->txtLog->clear();

    QMap<int, int> convDocType;
    convDocType[6] = 1;
    convDocType[8] = 1;
    convDocType[11] = 3;
    convDocType[17] = 5;

    log(tr("Synchronization started"));
    log(tr("Connecting to main database"));
    QSqlDrv dbFb(___ff_user->fullName, "clientdb");

    log(tr("Get convertions map"));
    QMap<QString, int> convStore;
    dbFb.execSQL("select as_store, cafe_store from sys_as_store_conv");
    while (dbFb.m_query->next())
        convStore[dbFb.valStr("AS_STORE")] = dbFb.valInt("CAFE_STORE");

    QMap<QString, int> convFood;
    dbFb.execSQL("select as_food, cafe_food from sys_as_food_conv");
    while (dbFb.m_query->next())
        convFood[dbFb.valStr("AS_FOOD")] = dbFb.valInt("CAFE_FOOD");

    if (!ui->chUpdateOnlyPrices->isChecked()) {
        log(tr("Deleting old synchronization data"));
        if (!dbFb.prepare("delete from st_documents where doc_date between :date1 and :date2 and comment='SYNC'")) {
            log(tr("Deleting failed"));
            return;
        }
        dbFb.bind(":date1", ui->deStart->date());
        dbFb.bind(":date2", ui->deEnd->date());
        if (!dbFb.execSQL()) {
            log(tr("Deleting failed"));
            return;
        }
    }

    log(tr("Connecting to AS database"));
    QSqlDatabase dbAs = QSqlDatabase::addDatabase("QODBC3", "def");
    dbAs.setDatabaseName(ui->leConnectionString->text());
    dbAs.setUserName(ui->leUsername->text());
    dbAs.setPassword(ui->lePassword->text());
    if (!dbAs.open()) {
        log(tr("Connecting to AS databases failed.") + dbAs.lastError().databaseText());
        return;
    }

    QDate d1 = ui->deStart->date();
    QDate d2 = ui->deEnd->date();
    int steps = 1;
    if (ui->chSeparateDate->isChecked()) {
        steps = d1.daysTo(d2) + 1;
        d2 = d1;
    }

    for (int i = 1; i < steps + 1; i++) {
        QSqlQuery q(dbAs);
        log(tr("Query") + ": " + ui->leQuery->document()->toPlainText().arg(ui->leDatabase->text()));
        q.prepare(ui->leQuery->document()->toPlainText().arg(ui->leDatabase->text()));
        q.bindValue(":date1", d1.toString("MM-dd-yyyy"));
        q.bindValue(":date2", d2.toString("MM-dd-yyyy"));
        if (!q.exec()) {
            QMessageBox::critical(this, tr("SQL Error"), q.lastError().databaseText());
            return;
        }

        log(QString("%1: %2 - %3").arg(tr("Get data")).arg(d1.toString("MM-dd-yyyy")).arg(d2.toString("MM-dd-yyyy")));
        QStringList sqlQueries;
        int currStore = 0;
        int currDocType = 0;
        int currDocId = 0;
        if (!ui->chUpdateOnlyPrices->isChecked())
            currDocId = m_sqlDrv->genId("GEN_ST_DOCUMENTS_ID");
        int currDBCR = -1;
        float amount = 0;
        bool notEmpty = false;
        while (q.next()) {
            notEmpty = true;
            bool isNew = false;
            int docType = convDocType[q.value("doctype").toInt()];
            int store = convStore[q.value("store").toString().trimmed()];
            if (!store) {
                log(tr("Store not in map") + ": " + q.value("store").toString().trimmed());
                continue;
            }
            int dbcr = q.value("fDBCR").toInt();

            if (!currStore) {
                currStore = store;
            }
            if (!currDocType)
                currDocType = docType;
            if (currDBCR < 0)
                currDBCR = q.value("fDBCR").toInt();

            if (docType != currDocType)
                isNew = true;
            if (store != currStore)
                isNew = true;
            if (dbcr != currDBCR)
                isNew = true;
            if (q.at() == q.size())
                isNew = true;

            if (isNew) {
                if (currDocType == 5 && currDBCR == 0)
                    currDocType = 6;
                int storeIn = 0;
                int storeOut = 0;
                switch (currDocType) {
                case 1:
                    storeIn = currStore;
                case 2:
                    storeOut = currStore;
                case 5:
                    storeIn = currStore;
                case 6:
                    storeOut = currStore;
                }

                if (!ui->chUpdateOnlyPrices->isChecked()) {
                    sqlQueries.append(QString("insert into st_documents values (%1, %2, 1, 1, '%3', %4, %5, %6, %7, %8, %9, %10, 'SYNC', 0)")
                                      .arg(currDocId)
                                      .arg(currDocType)
                                      .arg(d1.toString(DATE_FORMAT))
                                      .arg(storeIn)
                                      .arg(storeOut)
                                      .arg(1).arg(1).arg(1).arg(0)
                                      .arg(QString::number(amount, 'f', 2)));
                    currDocId = m_sqlDrv->genId("GEN_ST_DOCUMENTS_ID");
                }
                amount = 0;
                currDocType = docType;
                currStore = store;
                currDBCR = dbcr;
            }

            int goods = q.value("food").toInt();
            if (convFood.contains(q.value("food").toString().trimmed()))
                goods = convFood[q.value("food").toString().trimmed()];
            if (!goods) {
                log(QString("%1: %2").arg(tr("Unknown code")).arg(q.value("food").toString()));
                continue;
            }

            amount += q.value("amount").toFloat();
            if (!ui->chUpdateOnlyPrices->isChecked()) {
                sqlQueries.append(QString("insert into st_draft values (null, %1, %2, %3, %4, 0)")
                                  .arg(currDocId)
                                  .arg(goods)
                                  .arg(q.value("qty").toFloat())
                                  .arg(QString::number(q.value("amount").toFloat(), 'f', 2)));
            }
            if (!ui->chNoPriceUPdate->isChecked()) {
                sqlQueries.append(QString("update food_names set price=%1 where id=%2")
                                  .arg(q.value("amount").toFloat() / q.value("qty").toFloat())
                                  .arg(goods));
            }
        }
        if (notEmpty && !ui->chUpdateOnlyPrices->isChecked()) {
            if (currDocType == 5 && currDBCR == 0)
                currDocType = 6;
            sqlQueries.append(QString("insert into st_documents values (%1, %2, 1, 1, '%3', %4, %5, %6, %7, %8, %9, %10, 'SYNC', 0)")
                              .arg(currDocId)
                              .arg(currDocType)
                              .arg(d1.toString(DATE_FORMAT))
                              .arg(currStore)
                              .arg(0)
                              .arg(1).arg(1).arg(1).arg(0)
                              .arg(QString::number(amount, 'f', 2)));
        }

        log(tr("Insert new data"));
        std::reverse(sqlQueries.begin(), sqlQueries.end());
        int trCount = 0;
        if (!dbFb.m_db.transaction()) {
            log(dbFb.m_db.lastError().databaseText());
            return;
        }
        for (QStringList::const_iterator i = sqlQueries.begin(); i != sqlQueries.end(); i++) {
            if (!dbFb.execSQL(*i)) {
                dbFb.close();
                log(tr("Cannot execute query. "));
                return;
            }
            if (trCount > 100) {
                trCount = 0;
                dbFb.m_db.commit();
                dbFb.m_db.transaction();
            }
        }
        dbFb.m_db.commit();
        d1 = d1.addDays(1);
        d2 = d2.addDays(1);
    }
    dbFb.m_db.commit();
    log(tr("All done"));
}
*/

void QImportFromAS::on_actionAdd_triggered()
{
    addPairValues(ui->tblStore);
}

void QImportFromAS::on_actionEdit_triggered()
{
    editPairValues(ui->tblStore);
}

void QImportFromAS::on_actionRemove_triggered()
{
    delPairValues(ui->tblStore);
}

void QImportFromAS::on_actionAddFood_triggered()
{
    addPairValues(ui->tblFood);
}

void QImportFromAS::on_actionEditFood_triggered()
{
    editPairValues(ui->tblFood);
}

void QImportFromAS::on_actionRemoveFood_triggered()
{
    delPairValues(ui->tblFood);
}

void QImportFromAS::editPairValues(QTableWidget *t)
{
    QModelIndexList l = t->selectionModel()->selectedRows();
    if (!l.count())
        return;

    QString v1 = t->item(l.at(0).row(), 0)->text();
    QString v2 = t->item(l.at(0).row(), 1)->text();

    if (!DlgValuePair::getValuePair(v1, v2, this))
        return;

    t->setItem(l.at(0).row(), 0, new QTableWidgetItem(v1));
    t->setItem(l.at(0).row(), 1, new QTableWidgetItem(v2));
}

void QImportFromAS::addPairValues(QTableWidget *t)
{
    QString v1, v2;
    if (!DlgValuePair::getValuePair(v1, v2, this))
        return;

    addRow(t, v1, v2);
}

void QImportFromAS::delPairValues(QTableWidget *t)
{
    QModelIndexList l = t->selectionModel()->selectedRows();
    if (!l.count())
        return;

    t->removeRow(l.at(0).row());
}

void QImportFromAS::addRow(QTableWidget *t, const QString &v1, const QString &v2)
{
    int row = t->rowCount();
    t->setRowCount(row + 1);
    t->setItem(row, 0, new QTableWidgetItem(v1));
    t->setItem(row, 1, new QTableWidgetItem(v2));
}

void QImportFromAS::log(const QString &message)
{
    ui->txtLog->moveCursor(QTextCursor::End);
    ui->txtLog->textCursor().insertText(QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    ui->txtLog->textCursor().insertText(": ");
    ui->txtLog->textCursor().insertText(message);
    ui->txtLog->textCursor().insertText("\n");
    ui->txtLog->verticalScrollBar()->setValue(ui->txtLog->verticalScrollBar()->maximum());
    qApp->processEvents();
}

void QImportFromAS::getDocStorages(Document &d, QMap<QString, int> &s)
{
    QStringList b = d.body.split("\r\n");
    QString s1, s2;
    switch(d.type) {
    case 1:
        for (QStringList::const_iterator it = b.begin(); it != b.end(); it++)
            if (it->contains("STORAGE")) {
                s1 = it->mid(it->indexOf(":") + 1, it->length() - it->indexOf(":"));
            }
        break;
    case 2:
        for (QStringList::const_iterator it = b.begin(); it != b.end(); it++)
            if (it->contains("STORAGE")) {
                s2 = it->mid(it->indexOf(":") + 1, it->length() - it->indexOf(":"));
            }
        break;
    case 3:
    case 5:
        for (QStringList::const_iterator it = b.begin(); it != b.end(); it++) {
            if (it->contains("STORAGEEXPENSE")) {
                s2 = it->mid(it->indexOf(":") + 1, it->length() - it->indexOf(":"));
            }
            if (it->contains("STORAGEINCOME")) {
                s1 = it->mid(it->indexOf(":") + 1, it->length() - it->indexOf(":"));
            }
        }
        break;
    }
    if (!s.contains(s1)) {
        s[s1] = 1;
    }
    if (!s.contains(s2)) {
        s[s2] = 1;
    }
    d.storeIn = s[s1];
    d.storeOut = s[s2];
}

void QImportFromAS::on_chNoPriceUPdate_clicked(bool checked)
{
    ui->chUpdateOnlyPrices->setEnabled(!checked);
    if (checked)
        if (ui->chUpdateOnlyPrices->isChecked())
            ui->chUpdateOnlyPrices->setChecked(false);
}
