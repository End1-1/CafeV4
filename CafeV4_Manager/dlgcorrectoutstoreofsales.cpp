#include "dlgcorrectoutstoreofsales.h"
#include "ui_dlgcorrectoutstoreofsales.h"
#include <QMessageBox>

DlgCorrectOutStoreOfSales::DlgCorrectOutStoreOfSales(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCorrectOutStoreOfSales)
{
    ui->setupUi(this);
    m_sqlDriver = new QSqlDrv("admin", "clientdb");
    ui->deDate1->setDate(QDate::currentDate());
    ui->deDate2->setDate(QDate::currentDate());

}

DlgCorrectOutStoreOfSales::~DlgCorrectOutStoreOfSales()
{
    delete ui;
    delete m_sqlDriver;
}

void DlgCorrectOutStoreOfSales::setData(const QString &food, int oldStore, int cashId)
{
    ui->leDishCode->setText(food);
    ui->cbCurrStore->setIndexOfData(oldStore);
    ui->cbCash->setIndexOfData(cashId);
}

void DlgCorrectOutStoreOfSales::setDate(QDate d1, QDate d2)
{
    ui->deDate1->setDate(d1);
    ui->deDate2->setDate(d2);
}

void DlgCorrectOutStoreOfSales::on_btnStart_clicked()
{
    if (!m_sqlDriver->openDB())
        return;
    QString sql = QString("update o_dishes set store_id=:store_id_new "
                          "where store_id=:store_id_old and dish_id in (%1) "
                          "and order_id in (select id from o_order where date_cash between :date1 and :date2 and cash_id=:cash_id)")
            .arg(ui->leDishCode->text());
    if (!m_sqlDriver->prepare(sql))
        return;
    m_sqlDriver->bind(":store_id_new", ui->cbNewStore->currentItemData());
    m_sqlDriver->bind(":store_id_old", ui->cbCurrStore->currentItemData());
    m_sqlDriver->bind(":date1", ui->deDate1->date());
    m_sqlDriver->bind(":date2", ui->deDate2->date());
    m_sqlDriver->bind(":cash_id", ui->cbCash->currentItemData());
    if (!m_sqlDriver->execSQL())
        return;
    m_sqlDriver->close();
    QMessageBox::information(this, tr("Information"), tr("Done"));
}
