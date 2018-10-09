#include "qsinchronizedatabasechanges.h"
#include "ui_qsinchronizedatabasechanges.h"
#include "core.h"
#include "qsinchronizedatabasethread.h"
#include <QDateTime>
#include <QScrollBar>
#include <QProcess>
#include <QFile>

QSinchronizeDatabaseChanges::QSinchronizeDatabaseChanges(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::QSinchronizeDatabaseChanges)
{
    m_actions << "actionRefresh";

    ui->setupUi(this);
    setWindowTitle(tr("Synchronization"));

    m_sqlDrv->prepare("select db_path, db_user, db_pass "
                      "from sys_databases where group_id=:group_id "
                      "and main_id = 0 ");
    m_sqlDrv->bind(":group_id", QSystem::dbGroupId());
    m_sqlDrv->execSQL();
    m_sqlDrv->next();
    m_mainDb.append(m_sqlDrv->val().toString());
    m_mainDb.append(m_sqlDrv->val().toString());
    m_mainDb.append(m_sqlDrv->val().toString());

    m_sqlDrv->prepare("select obj_name, id, db_path, db_user, db_pass, id_update, id_delete "
                      "from sys_databases where group_id=:group_id "
                      "and main_id <> 0 ");
    m_sqlDrv->bind(":group_id", QSystem::dbGroupId());
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        QStringList l;
        QString name = m_sqlDrv->val().toString();
        l.append(m_sqlDrv->val().toString());
        l.append(m_sqlDrv->val().toString());
        l.append(m_sqlDrv->val().toString());
        l.append(m_sqlDrv->val().toString());
        l.append(m_sqlDrv->val().toString());
        l.append(m_sqlDrv->val().toString());
        m_databases[name] = l;
    }

    m_sqlDrv->prepare("select obj_name from sys_databases where main_id<>0 and group_id=:group_id");
    m_sqlDrv->bind(":group_id", QSystem::dbGroupId());
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        QListWidgetItem *i = new QListWidgetItem(ui->lstMenu);
        i->setText(m_sqlDrv->val().toString());
        i->setCheckState(Qt::Unchecked);
        i->setFlags(i->flags() | Qt::ItemIsUserCheckable);
        ui->lstMenu->addItem(i);
    }
    m_sqlDrv->close();

    ui->tblProgress->setColumnWidth(0, 150);
    ui->tblProgress->setColumnWidth(1, 200);
    ui->tblProgress->setColumnWidth(2, 100);
    ui->tblProgress->setColumnWidth(3, 100);

    m_runningThread = 0;
}

QSinchronizeDatabaseChanges::~QSinchronizeDatabaseChanges()
{
    delete ui;
}

void QSinchronizeDatabaseChanges::actionRefresh()
{
    if (m_runningThread) {
        QMessageBox::information(this, tr("Information"), tr("Wait for current threads finish."));
        return;
    }
    ui->tblProgress->clearContents();
    ui->tblProgress->setRowCount(0);
    ui->tabWidget->setCurrentIndex(0);
    for (QMap<QString, QStringList>::const_iterator it = m_databases.begin(); it != m_databases.end(); it++) {
        QList<QListWidgetItem*> l = ui->lstMenu->findItems(it.key(), Qt::MatchExactly);
        if (!l.count())
            continue;
        if (l.at(0)->checkState() != Qt::Checked)
            continue;
        int row = ui->tblProgress->rowCount();
        ui->tblProgress->setRowCount(row + 1);
        for (int i = 0; i < ui->tblProgress->columnCount(); i++)
            ui->tblProgress->setItem(row, i, new QTableWidgetItem());
        ui->tblProgress->item(row, 0)->setText(it.key());
        QSinchronizeDatabaseThread *t = new QSinchronizeDatabaseThread(m_mainDb.at(0), m_mainDb.at(1), m_mainDb.at(2),
                                                                       it.value().at(1), it.value().at(2), it.value().at(3),
                                                                       it.value().at(0).toInt(), row,
                                                                       ui->txtSql->document()->toPlainText().split(";", QString::SkipEmptyParts));
        connect(t, SIGNAL(sigStatus(QString,int)), this, SLOT(updateThreadStatus(QString,int)));
        connect(t, SIGNAL(sigDone()), this, SLOT(threadFinished()));
        connect(t, SIGNAL(sigRecordsLeft(int,int,int)), this, SLOT(updateThreadRecord(int,int,int)));
        m_runningThread++;
        t->start();
    }
}

void QSinchronizeDatabaseChanges::on_chSelectAll_clicked(bool checked)
{
    for (int i = 0; i < ui->lstMenu->count(); i++)
        ui->lstMenu->item(i)->setCheckState(checked ? Qt::Checked : Qt::Unchecked);
}

void QSinchronizeDatabaseChanges::updateThreadStatus(const QString &status, int row)
{
    ui->tblProgress->item(row, 1)->setText(status);
}

void QSinchronizeDatabaseChanges::updateThreadRecord(int recordsLeft, int totalRecords, int row)
{
    ui->tblProgress->item(row, 2)->setText(QString::number(totalRecords));
    ui->tblProgress->item(row, 3)->setText(QString::number(recordsLeft));
}

void QSinchronizeDatabaseChanges::threadFinished()
{
    m_runningThread--;
}
