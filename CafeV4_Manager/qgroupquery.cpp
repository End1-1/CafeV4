#include "qgroupquery.h"
#include "ui_qgroupquery.h"
#include "qgroupquerybindvalues.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QLibrary>
#include <QDir>
#include <QMutexLocker>
#include <QDebug>

QToolBar *QGroupQuery::m_toolBarBtn;
QToolBar *QGroupQuery::m_toolBarWnd;
QStringList QGroupQuery::m_mainDb;
QString QDbThread::m_dbNum = "0";
QMutex QDbThread::m_mutex;

QGroupQuery::QGroupQuery(QWidget *parent) :
    MdiWindow(parent),
    ui(new Ui::QGroupQuery)
{
    m_actions << "actionExport_to_Excel" << "actionRefresh" << "actionFilter";
    ui->setupUi(this);
    setWindowTitle(tr("Group query"));
    setToolbar(m_toolBarBtn);
    init();
}

QGroupQuery::~QGroupQuery()
{
    delete ui;
}

void QGroupQuery::actionExcel(const QString &fileName)
{
    typedef int* (*Excel_Create)();
    typedef int (*Excel_Show)(void*);
    typedef int* (*Workbook_Add)(void *);
    typedef int* (*Sheet_Select)(void *, int);
    typedef int (*Sheet_Set_Cell_Value)(void*, int, int, const wchar_t *);
    typedef int (*Sheet_Set_Col_Width)(void *, int, int);
    typedef int (*Clear)(void*);
    typedef int (*SaveToFile)(void*, const wchar_t *);

    QLibrary lib(QDir::currentPath() + "/excel");
    if (!lib.load())
    {
        QMessageBox::critical(this, "Program error", "Could not load library " + QDir::currentPath() + "/excel.dll");
        return;
    }

    Excel_Create excel_create = (Excel_Create)(lib.resolve("Excel_Create"));
    Excel_Show excel_show = (Excel_Show)(lib.resolve("Excel_Show"));
    Workbook_Add workbook_add = (Workbook_Add)(lib.resolve("Workbook_Add"));
    Sheet_Select sheet_select = (Sheet_Select)(lib.resolve("Sheet_Select"));
    Sheet_Set_Cell_Value sheet_set_cell_value = (Sheet_Set_Cell_Value)(lib.resolve("Sheet_Set_Cell_Value"));
    Sheet_Set_Col_Width sheet_set_col_width = (Sheet_Set_Col_Width)(lib.resolve("Sheet_Set_Col_Width"));
    Clear clear = (Clear)(lib.resolve("Clear"));
    SaveToFile save = (SaveToFile)(lib.resolve("SaveToFile"));

    int *excel = excel_create();
    int *workbook = workbook_add(excel);
    int *sheet = sheet_select(workbook, 1);

    QStringList headers;
    for (int i = 0; i < ui->tblOutput->columnCount(); i++) {
        sheet_set_cell_value(sheet, i + 1, 1, ui->tblOutput->model()->headerData(i, Qt::Horizontal).toString().toStdWString().c_str());
        sheet_set_col_width(sheet, i + 1, ui->tblOutput->columnWidth(i) / 7);
        headers << ui->tblOutput->model()->headerData(i, Qt::Horizontal).toString();
    }

    for (int i = 0; i < ui->tblOutput->rowCount(); i++)
        for (int j =  0; j < ui->tblOutput->columnCount(); j++)
            sheet_set_cell_value(sheet, j + 1, i + 2, ui->tblOutput->item(i, j)->data(Qt::EditRole).toString().toStdWString().c_str());

//    int footerRow = ui->tblOutputTotal->rowCount() + ui->tblOutput->rowCount() + 2;
//    for (int i = 0; i < ui->tblOutput->rowCount(); i++)
//        for (int j =  0; j < ui->tblOutputTotal->columnCount(); j++) {
//            QTableWidgetItem *item = ui->tblOutputTotal->item(i, j);
//            if (item)
//                sheet_set_cell_value(sheet, j + 1, i + footerRow, item->text().toStdWString().c_str());
//        }

    excel_show(excel);
    if (fileName.length())
        save(workbook, fileName.toStdWString().c_str());
    clear(sheet);
    clear(workbook);
    clear(excel);
}

void QGroupQuery::actionRefresh()
{
    QGroupQueryBindValues *bv = new QGroupQueryBindValues(m_bindValues, this);
    if (bv->exec() != QDialog::Accepted) {
        delete bv;
        return;
    }
    delete bv;
    QSettings s("Jazzve", "Cafe4\\GroupQuery\\" + m_queryTitle);
    for (QList<BindValue>::const_iterator it = m_bindValues.constBegin(); it != m_bindValues.constEnd(); it++)
        s.setValue(it->valueName, it->value);
    ui->tblOutput->setRowCount(0);
    ui->tblOutput->setColumnCount(0);
    qRegisterMetaTypeStreamOperators<DATA>("DATA");
    for (int i = 0; i < ui->tblDatabases->rowCount(); i++) {
        QDbThread *dt = new QDbThread(ui->tblDatabases->item(i, 1)->data(Qt::DisplayRole).toString(),
                                      ui->tblDatabases->item(i, 2)->data(Qt::DisplayRole).toString(),
                                      ui->tblDatabases->item(i, 2)->data(Qt::UserRole).toString());
        connect(dt, SIGNAL(status(int,QString)), this, SLOT(dbthreadStatus(int,QString)));
        connect(dt, SIGNAL(fetchResult(DATA)), this, SLOT(dbthreadFetchResult(DATA)));
        connect(dt, SIGNAL(fetchColumns(QStringList)), this, SLOT(dbthreadFetchColumns(QStringList)));
        dt->row = i;
        dt->alias = ui->tblDatabases->item(i, 0)->data(Qt::DisplayRole).toString();
        dt->sqlText = ui->txtQuery->document()->toPlainText();
        for (QList<BindValue>::const_iterator it = m_bindValues.constBegin(); it != m_bindValues.constEnd(); it++)
            dt->bindValue[it->valueName] = it->value;
        dt->start();
    }
    saveHistory();
    ui->tab->setCurrentIndex(3);
}

bool QGroupQuery::actionFilter()
{
    QGroupQueryFilter *g = new QGroupQueryFilter(this);
    if (g->exec() == QDialog::Accepted) {
        ui->txtQuery->setPlainText(g->m_sqlText);
        m_queryTitle = g->m_queryTitle;
        setWindowTitle(m_queryTitle);
        m_bindValues = g->m_bindValues;
        actionRefresh();
        return true;
    }
    return false;
}

void QGroupQuery::init()
{
    QList<int> w;
    w << 200 << 300 << 150 << 500;
    for (int i = 0; i < w.count(); i++)
        ui->tblDatabases->setColumnWidth(i, w.at(i));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE", "group_query");
        db.setDatabaseName(m_mainDb.at(0));
        db.setUserName(m_mainDb.at(1));
        db.setPassword(m_mainDb.at(2));
        if (!db.open()) {
            QMessageBox::critical(this, tr("Error"), db.lastError().databaseText());
            return;
        }
        QSqlQuery q(db);
        QString sql = "select alias, db_path, db_user, db_pass "
                "from sys_databases "
                "where main_id=1 and group_id=:group_id";
        if (q.prepare(sql)) {
            q.bindValue(":group_id", 1);
            if (q.exec()) {
                ui->tblDatabases->clearContents();
                while (q.next()) {
                    int row = ui->tblDatabases->rowCount();
                    ui->tblDatabases->setRowCount(row + 1);
                    for (int i = 0; i < ui->tblDatabases->columnCount(); i++)
                        ui->tblDatabases->setItem(row, i, new QTableWidgetItem());
                    ui->tblDatabases->item(row, 0)->setData(Qt::DisplayRole, q.value(0));
                    ui->tblDatabases->item(row, 1)->setData(Qt::DisplayRole, q.value(1));
                    ui->tblDatabases->item(row, 2)->setData(Qt::DisplayRole, q.value(2));
                    ui->tblDatabases->item(row, 2)->setData(Qt::UserRole, q.value(3));
                }
            } else {
                QMessageBox::critical(this, tr("Error"), q.lastError().databaseText());
            }
        } else {
            QMessageBox::critical(this, tr("Error"), q.lastError().databaseText());
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("group_query");
    loadHistory();
}

void QGroupQuery::saveHistory()
{

}

void QGroupQuery::loadHistory()
{

}

void QGroupQuery::dbthreadStatus(int row, const QString &msg)
{
    //QMutexLocker locker(&m_mutex);
    ui->tblDatabases->item(row, 3)->setText(msg);
}

void QGroupQuery::dbthreadFetchResult(DATA data)
{
    int row = ui->tblOutput->rowCount();
    ui->tblOutput->setRowCount(row + data.count());
    for (QList<QList<QVariant> >::const_iterator it = data.constBegin(); it != data.constEnd(); it++) {
        int col = 0;
        for (QList<QVariant>::const_iterator vt = (*it).begin(); vt != (*it).end(); vt++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::EditRole, *vt);
            ui->tblOutput->setItem(row, col++, item);
        }
        row++;
    }
}

void QGroupQuery::dbthreadFetchColumns(QStringList colNames)
{
    if (ui->tblOutput->columnCount())
        return;
    ui->tblOutput->setColumnCount(colNames.count());
    ui->tblOutput->setHorizontalHeaderLabels(colNames);
}

QDbThread::QDbThread(const QString &dbName, const QString &dbUser, const QString &dbPass)
{
    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_dbName = dbName;
    m_userName = dbUser;
    m_password = dbPass;
}

void QDbThread::run()
{
    bool error = false;
    QString dbId = getDbId();
    {
        qDebug() << dbId;
        QSqlDatabase db = QSqlDatabase::addDatabase("QIBASE", dbId);
        db.setDatabaseName(m_dbName);
        db.setUserName(m_userName);
        db.setPassword(m_password);
        db.setConnectOptions("lc_ctype=utf8");
        emit status(row, tr("Connecting..."));
        QStringList queries = sqlText.split(";", QString::SkipEmptyParts);
        if (db.open()) {
            QSqlQuery q(db);
            for (QStringList::const_iterator sq = queries.begin(); sq != queries.end(); sq++) {
                if (q.prepare(*sq)) {
                    for (QMap<QString, QVariant>::const_iterator it = bindValue.begin(); it != bindValue.end(); it++)
                        q.bindValue(it.key(), it.value());
                    if (q.exec()) {
                        QList<QList<QVariant> > data;
                        if (q.next()) {
                            emit status(row, tr("Fetching data..."));
                            QSqlRecord rec = q.record();
                            QStringList cols;
                            cols << tr("Cafe");
                            for (int i = 0; i < rec.count(); i++)
                                cols << QString::fromUtf8(rec.fieldName(i).toLatin1());
                            emit fetchColumns(cols);
                            do {
                                QList<QVariant> d;
                                d.append(alias);
                                for (int i = 0; i < q.record().count(); i++)
                                    d.append(q.value(i));
                                data.append(d);
                            } while (q.next());
                            QMutexLocker locker(&m_mutex);
                            emit fetchResult(data);
                        }
                    } else {
                        error = true;
                        emit status(row, q.lastError().databaseText());
                    }
                } else {
                    error = true;
                    emit status(row, q.lastError().databaseText());
                }
            }
        } else {
            error = true;
            emit status(row, db.lastError().databaseText());
        }
        db.close();
    }
    if (!error)
        emit status(row, tr("Done."));
    QSqlDatabase::removeDatabase(dbId);
    terminate();
}

QString QDbThread::getDbId()
{
    QMutexLocker locker(&m_mutex);
    m_dbNum = QString::number(m_dbNum.toInt() + 1);
    return "DbThread" + m_dbNum;
}

void QGroupQuery::on_lstHistory_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    ui->txtQuery->setPlainText(item->text());
    ui->tab->setCurrentIndex(1);
}
