#include "qgroupquery.h"
#include "ui_qgroupquery.h"
#include "qgroupquerybindvalues.h"
#include "xlsxall.h"
#include "qbasegrid.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileDialog>
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

void QGroupQuery::actionExcel(QString fileName)
{
    int fXlsxFitToPage = 0;
    QString fXlsxPageOrientation = xls_page_orientation_portrait;
    int fXlsxPageSize = xls_page_size_a4;
    int colCount = ui->tblOutput->columnCount();
    int rowCount = ui->tblOutput->rowCount();
    if (colCount == 0 || rowCount == 0) {
        QMessageBox::information(this, "Error", tr("Empty report!"));
        return;
    }
    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    s->setupPage(fXlsxPageSize, fXlsxFitToPage, fXlsxPageOrientation);
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    d.style()->addHAlignment("header", xls_alignment_center);
    d.style()->addBorder("header", XlsxBorder());
    QBaseGrid::CustomHeader *h = static_cast<QBaseGrid::CustomHeader*>(ui->tblOutput->horizontalHeader());
    for (int i = 0; i < colCount; i++) {
        //s->addCell(1, i + 1, h->m_captions[i], d.style()->styleNum("header"));
        s->setColumnWidth(i + 1, ui->tblOutput->columnWidth(i) / 7);
    }
    /* BODY */
    QMap<int, QString> bgFill;
    QMap<int, QString> bgFillb;
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    d.style()->addBackgrounFill("body", QColor(Qt::white));
    d.style()->addVAlignment("body", xls_alignment_center);
    d.style()->addBorder("body", XlsxBorder());
    bgFill[QColor(Qt::white).rgb()] = "body";
    bodyFont.setBold(true);
    d.style()->addFont("body_b", bodyFont);
    d.style()->addBackgrounFill("body_b", QColor(Qt::white));
    d.style()->addVAlignment("body_b", xls_alignment_center);
    d.style()->addBorder("body_b", XlsxBorder());
    bgFillb[QColor(Qt::white).rgb()] = "body_b";
    for (int j = 0; j < rowCount; j++) {
        for (int i = 0; i < colCount; i++) {
            int bgColor = QColor(Qt::white).rgb();
            if (!bgFill.contains(bgColor)) {
                bodyFont.setBold(false);
                d.style()->addFont(QString::number(bgColor), bodyFont);
                d.style()->addBackgrounFill(QString::number(bgColor), QColor::fromRgb(bgColor));
                bgFill[bgColor] = QString::number(bgColor);
            }
            if (!bgFill.contains(bgColor)) {
                bodyFont.setBold(true);
                d.style()->addFont(QString::number(bgColor), bodyFont);
                d.style()->addBackgrounFill(QString::number(bgColor), QColor::fromRgb(bgColor));
                bgFillb[bgColor] = QString::number(bgColor);
            }
            QString bgStyle = bgFill[bgColor];
            if (ui->tblOutput->item(j, i)->data(Qt::FontRole).value<QFont>().bold()) {
                bgStyle = bgFillb[bgColor];
            }
            s->addCell(j + 2, i + 1, ui->tblOutput->item(j, i)->data(Qt::EditRole), d.style()->styleNum(bgStyle));
        }
    }

    QString err;
    if (fileName.isEmpty()) {
        const_cast<QString&>(fileName) = QFileDialog::getSaveFileName(this, "", "", "*.xlsx");
    }
    if (fileName.isEmpty()) {
        return;
    }
    if (!d.save(fileName, err)) {
        if (!err.isEmpty()) {
            QMessageBox::critical(this, "Error", err);
        }
    }
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
        auto *th = new QThread();
        connect(th, &QThread::finished, th, &QThread::deleteLater);
        connect(th, &QThread::started, dt, &QDbThread::run);
        connect(dt, &QDbThread::finish, th, &QThread::quit);
        dt->moveToThread(th);
        th->start();
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

QDbThread::QDbThread(const QString &dbName, const QString &dbUser, const QString &dbPass) :
    QObject()
{
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
                            int colcount = rec.count();
                            for (int i = 0; i < colcount; i++)
                                cols << QString::fromUtf8(rec.fieldName(i).toLatin1());
                            emit fetchColumns(cols);
                            int rows = 0;
                            do {
                                QList<QVariant> d;
                                d.append(alias);
                                for (int i = 0; i < colcount; i++)
                                    d.append(q.value(i));
                                data.append(d);
                            } while (q.next());
                            //QMutexLocker locker(&m_mutex);
                            qDebug() << "EMIT FETCH RESULT";
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
    emit finish();
}

QString QDbThread::getDbId()
{
    QMutexLocker locker(&m_mutex);
    m_dbNum = QString::number(m_dbNum.toInt() + 1);
    qDebug() << "NEW DB NUMBER" << m_dbNum;
    return "DbThread" + m_dbNum;
}

void QGroupQuery::on_lstHistory_itemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;
    ui->txtQuery->setPlainText(item->text());
    ui->tab->setCurrentIndex(1);
}
