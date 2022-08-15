#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "qsqlcache.h"
#include "core.h"
#include <QClipboard>

DlgOrder::DlgOrder(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::DlgOrder)
{
    m_actions << "actionSave" << "actionCopy" << "actionExport_to_Excel";
    if (___ff_user->roleRead(ROLE_M_REMOVE_ORDERS))
        m_actions << "actionDelete";

    ui->setupUi(this);

    QList<int> widths;
    widths << 0 << 0 << 100 << 0 << 200 << 80 << 100 << 100 << 150;
    for (int i = 0; i < ui->tblDishes->columnCount(); i++)
         ui->tblDishes->setColumnWidth(i, widths[i]);
    widths.clear();
    widths << 150 << 150 << 300 << 600 << 300;
    for (int i = 0; i < ui->tblImportant->columnCount(); i++)
         ui->tblImportant->setColumnWidth(i, widths[i]);

    setWindowTitle(tr("Order"));


}

void DlgOrder::setId(const QString &id)
{
    m_id = id;
    if (m_id.length() > 0) {
        loadOrder();
    }
}

void DlgOrder::actionDelete()
{
    QModelIndexList l = ui->tblDishes->selectionModel()->selectedRows();
    if (!l.count()) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    int state_id = ui->tblDishes->item(l.at(0).row(), 1)->text().toInt();
    if (state_id != DISH_STATE_NORMAL) {
        QMessageBox::warning(this, tr("Warning"), tr("Dish state cannot be changed"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    QMap<int, QString> &c = QSqlCache().getCache("O_DISH_STATE");
    ui->tblDishes->item(l.at(0).row(), 1)->setText(its(DISH_STATE_REMOVED_PRINTED));
    ui->tblDishes->item(l.at(0).row(), 2)->setText(c[DISH_STATE_REMOVED_PRINTED]);
}

void DlgOrder::actionCostum(int action)
{
    switch (action) {
    case ACTION_HISTORY:
        showHistory();
        break;
    case ACTION_COPY:
        copyDishes();
        break;
    }
}

void DlgOrder::actionSave()
{
    CHECK_VIEWER
    double amount = 0;
    m_sqlDrv->prepare("update o_dishes set state_id=:state_id where id=:id");
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        m_sqlDrv->bind(":state_id", ui->tblDishes->item(i, 1)->text().toInt());
        m_sqlDrv->bind(":id", ui->tblDishes->item(i, 0)->text().toInt());
        m_sqlDrv->execSQL();
        if (ui->tblDishes->item(i, 1)->text().toInt() == DISH_STATE_NORMAL)
            amount += ui->tblDishes->item(i, 7)->text().toFloat();
    }

    m_sqlDrv->prepare("update o_order set date_cash=:date_cash, amount=:amount where id=:id");
    m_sqlDrv->bind(":date_cash", ui->deCash->date());
    m_sqlDrv->bind(":amount", amount);
    m_sqlDrv->bind(":id", m_id);
    m_sqlDrv->execSQL();
    m_sqlDrv->close();

    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

DlgOrder::~DlgOrder()
{
    delete ui;
}

void DlgOrder::loadOrder()
{
    m_sqlDrv->prepare("select id, state_id, table_id, staff_id, date_open, date_close, date_cash, "
                      "amount, payment, taxprint from o_order where id=:id");
    m_sqlDrv->bind(":id", m_id);
    m_sqlDrv->execSQL();
    m_sqlDrv->next();
    setWindowTitle(tr("Order") + " #" + m_sqlDrv->val().toString());

    ui->cbState->setIndexOfData(m_sqlDrv->val().toInt());
    ui->cbTable->setIndexOfData(m_sqlDrv->val().toInt());
    ui->cbStaff->setIndexOfData(m_sqlDrv->val().toInt());
    ui->deOpen->setDateTime(m_sqlDrv->val().toDateTime());
    ui->deClose->setDateTime(m_sqlDrv->val().toDateTime());
    ui->deCash->setDate(m_sqlDrv->val().toDate());
    ui->leAmount->setText(dts(m_sqlDrv->val().toDouble()));
    ui->cbPayment->setIndexOfData(m_sqlDrv->val().toInt());
    ui->leTax->setText(m_sqlDrv->val().toString());

    m_sqlDrv->prepare("select od.id, od.state_id, ods.name, od.dish_id, md.name, od.qty, od.price, od.qty*od.price, od.lu "
                      "from o_dishes od, o_dish_state ods, me_dishes md "
                      "where od.order_id=:order_id and od.state_id=ods.id and od.dish_id=md.id and od.state_id=1 "
                      "order by 2, 1");
    m_sqlDrv->bind(":order_id", m_id);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        int row =ui->tblDishes->rowCount();
        ui->tblDishes->setRowCount(row + 1);
        for (int i = 0; i < ui->tblDishes->columnCount(); i++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setData(Qt::DisplayRole, m_sqlDrv->val());
            ui->tblDishes->setItem(row, i, item);
        }
    }

    m_sqlDrv->prepare("select ia.name as action_name, i.log_date as date1, e.fname || ' ' || e.lname as staff, i.data, r.name "
                      "from o_important i "
                      "left join o_important_actions ia on ia.id=i.action_id "
                      "left join employes e on i.user_id=e.id "
                      "left join o_remove_reason r on r.id=i.reason_id "
                      "where i.order_id=:order_id");
    m_sqlDrv->bind(":order_id", m_id);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        int row = ui->tblImportant->rowCount();
        ui->tblImportant->setRowCount(row + 1);
        for (int i = 0; i < ui->tblImportant->columnCount(); i++)
            ui->tblImportant->setItem(row, i, new QTableWidgetItem(m_sqlDrv->val().toString()));
    }

    m_sqlDrv->close();
}

void DlgOrder::showHistory()
{

}

void DlgOrder::copyDishes()
{
    QString clipBoard;
    for (int i = 0; i < ui->tblDishes->rowCount(); i++) {
        for (int j = 0; j < ui->tblDishes->columnCount(); j++) {
            clipBoard += ui->tblDishes->item(i, j)->data(Qt::DisplayRole).toString() + "\t";
        }
        clipBoard += "\r\n";
    }
    qApp->clipboard()->setText(clipBoard);
}

void DlgOrder::loadHistory()
{
    ui->tblHistory->clear();
    ui->tblHistory->setRowCount(0);
    m_sqlDrv->prepare("select fdate, ftime, fcomp, fuser, ftr, fdata from o_tr where forder=:forder order by fdate, ftime");
    m_sqlDrv->bind(":forder", m_id);
    m_sqlDrv->execSQL();
    QList<int> colWidths;
    colWidths << 120 << 100 << 100 << 250 << 200 << 400;
    for (int i = 0; i < ui->tblHistory->columnCount(); i++) {
        ui->tblHistory->setColumnWidth(i, colWidths.at(i));
    }
    while (m_sqlDrv->next()) {
        int r = ui->tblHistory->rowCount();
        ui->tblHistory->setRowCount(ui->tblHistory->rowCount() + 1);
        ui->tblHistory->setItem(r, 0, new QTableWidgetItem(m_sqlDrv->valStr("FDATE")));
        ui->tblHistory->setItem(r, 1, new QTableWidgetItem(m_sqlDrv->valStr("FTIME")));
        ui->tblHistory->setItem(r, 2, new QTableWidgetItem(m_sqlDrv->valStr("FCOMP")));
        ui->tblHistory->setItem(r, 3, new QTableWidgetItem(m_sqlDrv->valStr("FUSER")));
        ui->tblHistory->setItem(r, 4, new QTableWidgetItem(m_sqlDrv->valStr("FTR")));
        ui->tblHistory->setItem(r, 5, new QTableWidgetItem(m_sqlDrv->valStr("FDATA")));
    }
}

void DlgOrder::on_tabWidget_currentChanged(int index)
{
    if (index == 3) {
        loadHistory();
    }
}
