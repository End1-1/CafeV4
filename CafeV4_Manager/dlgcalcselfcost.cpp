#include "dlgcalcselfcost.h"
#include "ui_dlgcalcselfcost.h"

DlgCalcSelfCost::DlgCalcSelfCost(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgCalcSelfCost)
{
    ui->setupUi(this);
    ui->cbUpdateMode->addItem(tr("Price from selected range"));
    ui->cbUpdateMode->addItem(tr("If price not found, use last input price"));
    ui->cbUpdateMode->setCurrentIndex(0);

    Toolkit::setFirstAndLastDateEdit(ui->date1, ui->date2);

    m_sqlDrv = new QSqlDrv(___ff_user->fullName, "clientdb");
}

DlgCalcSelfCost::~DlgCalcSelfCost()
{
    delete m_sqlDrv;
    delete ui;
}

void DlgCalcSelfCost::on_btnStart_clicked()
{
    ui->btnStart->setEnabled(false);
    ui->btnStop->setEnabled(true);
    m_started = true;
    ui->progressBar->setValue(0);
    switch (ui->cbUpdateMode->currentIndex()) {
    case 0:
        updatePrices_1();
        break;
    case 1:
        updatePrices_2();
        break;
    }
}

bool DlgCalcSelfCost::getPricesInRange(QMap<int, double> &data)
{
    QSqlQuery q(m_sqlDrv->m_db);
    QString sql = "select ss.goods_id, sum(ss.amount) / sum(ss.qty) as price "
            "from st_documents st, st_store ss "
            "where st.id=ss.doc_id and st.action_id=1 and ss.doc_date between :date1 and :date2 "
            "group by 1";
    if (!m_sqlDrv->prepare()) {
        finish();
        return false;
    }
    q.bindValue(":date1", ui->date1->date());
    q.bindValue(":date2", ui->date2->date());
    if (!m_sqlDrv->execSQL()) {
        finish();
        return false;
    }
    while (q.next())
        data[q.value(0).toInt()] = q.value(1).toDouble();
    m_sqlDrv->close();
    return true;
}

void DlgCalcSelfCost::updateGoods(int goodsid, double price)
{
    if (!m_sqlDrv->prepare("update me_recipes set price=:price where goods_id=:goods_id"))
        return;
    m_sqlDrv->bind(":price", price);
    m_sqlDrv->bind(":goods_id", goodsid);
    m_sqlDrv->execSQL();
}

void DlgCalcSelfCost::updatePrices_1()
{
    QMap<int, double> prices;
    if (!getPricesInRange(prices)) {
        finish();
        return;
    }
    QSqlQuery q(m_sqlDrv->m_db);
    int progress = 0;
    ui->progressBar->setValue(0);
    ui->progressBar->setMaximum(prices.count());
    for (QMap<int, double>::const_iterator it = prices.begin(); it != prices.end(); it++) {
        if (!m_started) {
            finish();
            return;
        }

        updateGoods(it.key(), it.value());
        progress++;
        if (progress % 10) {
            ui->progressBar->setValue(progress);
            qApp->processEvents();
        }
    }
    ui->progressBar->setValue(ui->progressBar->maximum());
    finish();
}

void DlgCalcSelfCost::updatePrices_2()
{
    QMap<int, double> prices;
    if (!getPricesInRange(prices)) {
        finish();
        return;
    }
}

void DlgCalcSelfCost::finish()
{
    m_started = false;
    ui->btnStart->setEnabled(true);
    ui->btnStop->setEnabled(false);
}

void DlgCalcSelfCost::on_btnStop_clicked()
{
    finish();
}
