#include "qsale.h"
#include "dlgorder.h"
#include "dlgselectsalereptype.h"
#include "dlgcorrectoutstoreofsales.h"
#include "gmenuitem.h"
#include "core.h"
#include "dlgorderremovereason.h"
#include <QMenu>
#include <QClipboard>
#include <QApplication>

QSale::QSale(QWidget *parent) :
    QGrid(parent)
{
    m_actions << "actionEdit" << "actionRefresh" << "actionFilter"
              << "actionExport_to_Excel" << "actionPrint_preview" << "actionPrint" << "actionSearch"
              << "actionCorrect_output_store_of_sales_table" << "actionRecipe";
    if (___ff_user->roleRead(ROLE_M_REMOVE_ORDERS))
        m_actions << "actionDelete";

    setWindowTitle(tr("Sales"));

    SqlField *fId = new SqlField("ID", tr("Id"), 100, 0, QVariant::String, 0, false);
    fId->addFilter(Filter::ftEdit, false, "");
    fId->m_filter->m_showCheckBox = true;
    fId->setInclude(m_settings->value("qsale_id_check", true).toBool());
    m_fields.append(fId);

    SqlField *fStateId = new SqlField("OO_STATE_ID", tr("Order state id"), 0, 0, QVariant::Int, 0, false);
    fStateId->addFilter(Filter::ftEdit, true, "O_ORDER_STATE");
    fStateId->m_filter->m_showCheckBox = true;
    fStateId->m_filter->listValues = "2";
    fStateId->setInclude(false);
    m_fields.append(fStateId);

    SqlField *fStateName = new SqlField("OO_STATE_NAME", tr("Order state"), 150, 0, QVariant::String, 0, false);
    fStateName->setIncludeRef(fStateId);
    m_fields.append(fStateName);

    SqlField *fCashId = new SqlField("CASH_ID", tr("Cash id"), 0, 0, QVariant::Int, 0, false);
    fCashId->addFilter(Filter::ftEdit, true, "CASH");
    fCashId->m_filter->m_showCheckBox = true;
    fCashId->setInclude(false);
    m_fields.append(fCashId);

    SqlField *fCashName = new SqlField("CASH_NAME", tr("Cash name"), 150, 0, QVariant::String, 0, false);
    fCashName->setIncludeRef(fCashId);
    m_fields.append(fCashName);

    SqlField *fDateOpen = new SqlField("DATE_OPEN", tr("Date open"), 120, 0, QVariant::DateTime, 0, false);
    fDateOpen->addFilter(Filter::ftDateTime, false, "");
    fDateOpen->setInclude(false);
    fDateOpen->m_filter->listValues = QDateTime::currentDateTime().toString(DATETIME_FORMAT);
    fDateOpen->m_filter->m_showCheckBox = true;
    m_fields.append(fDateOpen);

    SqlField *fDateOpen2 = new SqlField("DATE_OPEN2", tr("Date open"), 120, 0, QVariant::DateTime, 0, false);
    fDateOpen2->addFilter(Filter::ftDateTime, false, "");
    fDateOpen2->setInclude(false);
    fDateOpen2->m_filter->listValues = QDateTime::currentDateTime().toString(DATETIME_FORMAT);
    fDateOpen2->m_filter->m_showCheckBox = true;
    m_fields.append(fDateOpen2);

    SqlField *fDateClose = new SqlField("DATE_CLOSE", tr("Date close"), 120, 0, QVariant::DateTime, 0, false);
    fDateClose->addFilter(Filter::ftDateTime, false, "");
    fDateClose->setInclude(false);
    fDateClose->m_filter->m_showCheckBox = true;
    fDateClose->m_filter->listValues = QDateTime::currentDateTime().toString(DATETIME_FORMAT);
    m_fields.append(fDateClose);

    SqlField *fDateClose1 = new SqlField("DATE_CLOSE2", tr("Date close"), 120, 0, QVariant::DateTime, 0, false);
    fDateClose1->addFilter(Filter::ftDateTime, false, "");
    fDateClose1->setInclude(false);
    fDateClose1->m_filter->m_showCheckBox = true;
    fDateClose1->m_filter->listValues = QDateTime::currentDateTime().toString(DATETIME_FORMAT);
    m_fields.append(fDateClose1);

    SqlField *time1 = addField("TIME1", tr("Time"), 100, QVariant::Time);
    time1->addFilter(Filter::ftTime, false, "");
    time1->m_filter->listValues = "00:00:00";
    time1->setInclude(false);
    time1->m_filter->m_showCheckBox = true;

    SqlField *time2 = addField("TIME2", tr("Time"), 100, QVariant::Time);
    time2->addFilter(Filter::ftTime, false, "");
    time2->m_filter->listValues = "23:59:59";
    time2->m_filter->m_showCheckBox = true;
    time2->setInclude(false);

    QString date = QDate::currentDate().toString(DATE_FORMAT);
    if (!m_settings->value("qsale_date_cash1").toString().length()) {
        m_settings->setValue("qsale_date_cash1", date);
        m_settings->setValue("qsale_date_cash2", date);
    }
    SqlField *fDateCash1 = new SqlField("DATE_CASH1", tr("Date cash"), 120, 0, QVariant::Date, 0, false);
    fDateCash1->addFilter(Filter::ftDate, false, "");
    fDateCash1->m_filter->m_showCheckBox = true;
    fDateCash1->setInclude(m_settings->value("qsale_date_cash_checked", true).toBool());
    fDateCash1->m_filter->listValues = m_settings->value("qsale_date_cash1").toString();
    m_fields.append(fDateCash1);

    SqlField *fDateCash2 = new SqlField("DATE_CASH2", tr("Date cash"), 0, 0, QVariant::Date, 0, false);
    fDateCash2->addFilter(Filter::ftDate, false, "");
    fDateCash2->setInclude(false);
    fDateCash2->m_filter->listValues = m_settings->value("qsale_date_cash2").toString();
    m_fields.append(fDateCash2);

    SqlField *fStaffId = new SqlField("STAFF_ID", tr("Staff id"), 0, 0, QVariant::Int, 0, false);
    fStaffId->addFilter(Filter::ftEdit, true, "EMPLOYES_ID_NAME");
    fStaffId->setInclude(false);
    fStaffId->m_filter->m_showCheckBox = true;
    m_fields.append(fStaffId);

    SqlField *fStaffName = new SqlField("STAFF_NAME", tr("Staff name"), 200, 0, QVariant::String, 0, false);
    fStaffName->setIncludeRef(fStaffId);
    m_fields.append(fStaffName);

    SqlField *fTableId = new SqlField("TABLE_ID", tr("Table id"), 0, 0, QVariant::Int, 0, false);
    fTableId->addFilter(Filter::ftEdit, true, "H_TABLE");
    fTableId->setInclude(false);
    fTableId->m_filter->m_showCheckBox = true;
    m_fields.append(fTableId);

    SqlField *fTableName = new SqlField("TABLE_NAME", tr("Table"), 100, 0, QVariant::String, 0, false);
    fTableName->setIncludeRef(fTableId);
    m_fields.append(fTableName);

    SqlField *fStoreId = new SqlField("STORE_ID", tr("Store id"), 0, 0, QVariant::Int, 0, false);
    fStoreId->addFilter(Filter::ftEdit, true, "ST_STORAGES");
    fStoreId->m_filter->m_showCheckBox = true;
    fStoreId->setInclude(false);\
    m_fields.append(fStoreId);

    SqlField *fStoreName = new SqlField("STORE_NAME", tr("Store"), 150, 0, QVariant::String, 0, false);
    fStoreName->setIncludeRef(fStoreId);
    m_fields.append(fStoreName);

    SqlField *fDishTypeId = new SqlField("DISH_TYPE_ID", tr("Dish type code"), 0, 0, QVariant::Int, 0, false);
    fDishTypeId->addFilter(Filter::ftEdit, true, "ME_TYPES");
    fDishTypeId->setInclude(m_settings->value("qsale_dish_type_checked", false).toBool());
    fDishTypeId->m_filter->m_showCheckBox = true;
    m_fields.append(fDishTypeId);

    SqlField *fDishTypeName = new SqlField("DISH_TYPE_NAME", tr("Dish type"), 150, 0, QVariant::String, 0, false);
    fDishTypeName->setIncludeRef(fDishTypeId);
    m_fields.append(fDishTypeName);

    SqlField *fDishId = new SqlField("DISH_ID", tr("Dish id"), 0, 0, QVariant::Int, 0, false);
    fDishId->addFilter(Filter::ftEdit, true, "ME_DISHES");
    fDishId->setInclude(m_settings->value("qsale_dish_id_checked", false).toBool());
    fDishId->m_filter->m_showCheckBox = true;
    m_fields.append(fDishId);

    SqlField *fDishName = new SqlField("DISH_NAME", tr("Dish name"), 250, 0, QVariant::String, 0, false);
    fDishName->setIncludeRef(fDishId);
    m_fields.append(fDishName);

    SqlField *fIncValue = new SqlField("AMOUNT_INC_VALUE", tr("Service"), 100, 0, QVariant::Double, 0, false);
    fIncValue->addFilter(Filter::ftEdit, false, "");
    fIncValue->m_filter->m_showCheckBox = true;
    fIncValue->setInclude(m_settings->value("qsale_inc_value_checked").toBool());
    m_fields.append(fIncValue);

    SqlField *fInc = new SqlField("AMOUNT_INC", tr("Amount inc"), 100, 0, QVariant::Double, 0, false);
    fInc->setIncludeRef(fIncValue);
    m_fields.append(fInc);

    SqlField *fDecValue = new SqlField("AMOUNT_DEC_VALUE", tr("Discount"), 100, 0, QVariant::Double, 0, false);
    fDecValue->addFilter(Filter::ftEdit, false, "");
    fDecValue->m_filter->m_showCheckBox = true;
    fDecValue->setInclude(m_settings->value("qsale_dec_value_checked").toBool());
    m_fields.append(fDecValue);

    SqlField *fDec = new SqlField("AMOUNT_DEC", tr("Amount dec"), 100, 0, QVariant::Double, 0, false);
    fDec->setIncludeRef(fDecValue);
    m_fields.append(fDec);

    SqlField *fQty = new SqlField("QTY", tr("Qty"), 100, 0, QVariant::Double, 0, false);
    m_fields.append(fQty);

    SqlField *fAmount = new SqlField("AMOUNT", tr("Amount"), 180, 0, QVariant::Double, 0, false);
    m_fields.append(fAmount);

    SqlField *fFlags = new SqlField("FLAG_ID", tr("Flags"), 0, 0, QVariant::Int, 0, false);
    fFlags->addFilter(Filter::ftEdit, false, "O_FLAGS");
    fFlags->setInclude(false);
    m_fields.append(fFlags);

    tableWidget(0)->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(tableWidget(0), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotContextMenu(QPoint)));

    actionBeforeSelect();
}

void QSale::actionEdit()
{
    SqlField *f = fieldByName("ID");
    if (!f->isIncluded()) {
        QMessageBox::warning(this, tr("Warning"), tr("The id field must be included in the query"));
        return;
    }

    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    DlgOrder *d = new DlgOrder(cellValue(l.toList().at(0), 0).toString(), this);
    d->show();
}

void QSale::actionBeforeSelect()
{
    QString select = "select ", group,
            from = "from o_order oo,";

    SqlField *f = fieldByName("DATE_CASH1");
    QString where = " where oo.date_cash between '" + f->m_filter->listValues + "' and '";
    f = fieldByName("DATE_CASH2");
    where += f->m_filter->listValues + "' and";

    f = fieldByName("ID");
    if (f->isIncluded()) {
        select += "oo.id,";
        group += "oo.id,";
    }
    f->haveFilter(where, "oo.id");

    f = fieldByName("OO_STATE_ID");
    if (f->isIncluded()) {
        select += "oo.state_id as oo_state_id,os.name as oo_state_name,";
        group += "oo.state_id,os.name,";
        f->sqlAddFrom(from, where, "o_order_state os", "os.id=oo.state_id");
    }
    if (f->haveFilter(where, "oo.state_id"))
        f->sqlAddFrom(from, where, "o_order_state os", "os.id=oo.state_id");

    f = fieldByName("CASH_ID");
    if (f->isIncluded()) {
        select += "oo.cash_id,c.name as cash_name,";
        group += "oo.cash_id,c.name,";
        f->sqlAddFrom(from, where, "cash c", "oo.cash_id=c.id");
    }
    if (f->haveFilter(where, "OO.CASH_ID"))
        f->sqlAddFrom(from, where, "cash c", "oo.cash_id=c.id");

    f = fieldByName("DATE_OPEN");
    if (f->isIncluded()) {
        select += "oo.date_open,";
        group += "oo.date_open,";
    }

    f = fieldByName("DATE_OPEN2");
    if (f->isIncluded()) {
        SqlField *f2 = fieldByName("DATE_OPEN");
        where += " oo.date_open between '" + f2->m_filter->listValues + "' and '" + f->m_filter->listValues + "' and";
    }

    f = fieldByName("DATE_CLOSE");
    if (f->isIncluded()) {
        select += "oo.date_close,";
        group += "oo.date_close,";
    }

    bool dateInclude = false;
    f = fieldByName("DATE_CLOSE2");
    if (f->isIncluded()) {
        dateInclude = true;
        f->setInclude(false);
        SqlField *f2 = fieldByName("DATE_CLOSE");
        where += " oo.date_close between '" + f2->m_filter->listValues + "' and '" + f->m_filter->listValues + "' and";
    }

    f = fieldByName("TIME1");
    if (f->isIncluded()) {
        select += "cast(oo.date_close as time) as time1,";
        group += "cast(oo.date_close as time),";
    }

    bool timeInclude = false;
    f = fieldByName("TIME2");
    if (f->isIncluded()) {
        f->setInclude(false);
        timeInclude = true;
        SqlField *fTime1 = fieldByName("TIME1");
        where += " cast(oo.date_close as time) between '" + fTime1->m_filter->listValues + "' and '" + f->m_filter->listValues + "' and";
    }

    f = fieldByName("DATE_CASH1");
    if (f->isIncluded()) {
        select += "oo.date_cash as date_cash1,";
        group += "oo.date_cash,";
    }

    f = fieldByName("TABLE_ID");
    if (f->isIncluded()) {
        select += "oo.table_id,ht.name as table_name,";
        group += "oo.table_id,ht.name,";
        f->sqlAddFrom(from, where, "h_table ht", "oo.table_id=ht.id");
    }
    if (f->haveFilter(where, "oo.table_id"))
        f->sqlAddFrom(from, where, "h_table ht", "oo.table_id=ht.id");

    f = fieldByName("STAFF_ID");
    if (f->isIncluded()) {
        select += "oo.staff_id,e.fname || ' ' || e.lname as staff_name,";
        group += "oo.staff_id,e.fname || ' ' || e.lname,";
        f->sqlAddFrom(from, where, "employes e", "oo.staff_id=e.id");
    }
    if (f->haveFilter(where, "oo.staff_id"))
        f->sqlAddFrom(from, where, "employes e", "oo.staff_id=e.id");

    f = fieldByName("STORE_ID");
    if (f->isIncluded()) {
        select += "od.store_id,st.name as store_name,";
        group += "od.store_id,st.name,";
        f->sqlAddFrom(from, where, "o_dishes od", "oo.id=od.order_id");
        f->sqlAddFrom(from, where, "st_storages st", "od.store_id=st.id");
    }
    if (f->haveFilter(where, "od.store_id"))
        f->sqlAddFrom(from, where, "o_dishes od", "oo.id=od.order_id");

    f = fieldByName("DISH_TYPE_ID");
    if (f->isIncluded()) {
        select += "md.type_id as dish_type_id,mt.name as dish_type_name,";
        group += "md.type_id,mt.name,";
        f->sqlAddFrom(from, where, "o_dishes od", "oo.id=od.order_id");
        f->sqlAddFrom(from, where, "me_dishes md", "od.dish_id=md.id");
        f->sqlAddFrom(from, where, "me_types mt", "md.type_id=mt.id");
    }
    if (f->haveFilter(where, "md.type_id")) {
        f->sqlAddFrom(from, where, "o_dishes od", "oo.id=od.order_id");
        f->sqlAddFrom(from, where, "me_dishes md", "od.dish_id=md.id");
        f->sqlAddFrom(from, where, "me_types mt", "md.type_id=mt.id");
    }

    f = fieldByName("DISH_ID");
    if (f->isIncluded()) {
        select += "od.dish_id,md.name as dish_name,";
        group += "od.dish_id,md.name,";
        f->sqlAddFrom(from, where, "o_dishes od", "oo.id=od.order_id");
        f->sqlAddFrom(from, where, "me_dishes md", "od.dish_id=md.id");
    }
    if (f->haveFilter(where, "od.dish_id"))
        f->sqlAddFrom(from, where, "o_dishes od", "oo.id=od.order_id");

    if (fieldByName("STORE_ID")->isIncluded()
            || fieldByName("DISH_TYPE_ID")->isIncluded()
            || fieldByName("DISH_ID")->isIncluded()) {
        if (fieldByName("AMOUNT_INC_VALUE")->isIncluded()) {
            select += "od.price_inc as amount_inc_value,sum(od.price_inc*od.price*od.qty) as amount_inc,";
            group += "od.price_inc,";
        }
        if (fieldByName("AMOUNT_DEC_VALUE")->isIncluded()) {
            select += "od.price_dec as amount_dec_value,sum(od.price_dec*(od.price*od.qty)+(od.price*od.price_inc*od.qty)) as amount_dec,";
            group += "od.price_dec,";
        }
        select += "sum(od.qty) as qty, sum(od.qty*od.price) as amount,";
        where += " od.state_id=1 and";
    } else {
        if (fieldByName("AMOUNT_INC_VALUE")->isIncluded()) {
            select += "oo.amount_inc_value,sum(oo.amount_inc) as amount_inc,";
            group += "oo.amount_inc_value,";
        }
        if (fieldByName("AMOUNT_DEC_VALUE")->isIncluded()) {
            select += "oo.amount_dec_value,sum(oo.amount_dec) as amount_dec,";
            group += "oo.amount_dec_value,";
        }
        select += "count(oo.id) as qty, sum(oo.amount) as amount,";
    }
    if (group.length())
        group = " group by " + group;

    f = fieldByName("FLAG_ID");
    if (f->haveFilter(where, "F.FLAG_ID"))
        f->sqlAddFrom(from, where, "O_ORDER_FLAGS f", "f.order_id=oo.id");

    remove_zap(select);
    select += " ";
    remove_and(where);
    remove_zap(from);
    remove_zap(group);

    m_totalRowTitles << fieldByName("QTY")->title << fieldByName("AMOUNT")->title << fieldByName("AMOUNT_INC")->title << fieldByName("AMOUNT_DEC")->title;

    m_sqlString = select + from + where + group;
    actionSelect();

    f = fieldByName("DATE_CLOSE2");
    f->setInclude(dateInclude);
    f = fieldByName("TIME2");
    f->setInclude(timeInclude);
}

void QSale::actionDelete()
{
    CHECK_VIEWER
    SqlField *f = fieldByName("ID");
    if (!f->isIncluded()) {
        QMessageBox::warning(this, tr("Warning"), tr("The report doesnt contain field with order id"));
        return;
    }

    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    QString reason;
    if (!DlgOrderRemoveReason::reason(reason)) {
        return;
    }

    m_sqlDrv->prepare("update o_order set state_id=:state_id where id=:id");
    m_sqlDrv->bind(":state_id", 5);
    m_sqlDrv->bind(":id", cellValue(l.toList().at(0), 0));
    m_sqlDrv->execSQL();

    m_sqlDrv->prepare("update o_dishes set state_id=:state_id where state_id=1 and order_id=:id");
    m_sqlDrv->bind(":state_id", 3);
    m_sqlDrv->bind(":id", cellValue(l.toList().at(0), 0));
    m_sqlDrv->execSQL();

    m_sqlDrv->prepare("update h_table set order_id='' where order_id=:order_id");
    m_sqlDrv->bind(":order_id", cellValue(l.toList().at(0), 0));
    m_sqlDrv->execSQL();

    QString sql = "insert into o_important (order_id, user_id, action_id, data) values (:order_id, :user_id, :action_id, :data)";
    m_sqlDrv->prepare(sql);
    m_sqlDrv->bind(":order_id", cellValue(l.toList().at(0), 0));
    m_sqlDrv->bind(":user_id", ___ff_user->id);
    m_sqlDrv->bind(":action_id", 2);
    m_sqlDrv->bind(":data", reason);
    m_sqlDrv->execSQL();

    m_sqlDrv->close();
    QMessageBox::information(this, tr("Information"), tr("Removed. Refresh report"));
}

void QSale::actionCostum(int action)
{
    switch (action) {
    case ACTION_CORRECT_STORE:
        CHECK_VIEWER
        correctStore();
        break;
    case ACTION_RECIPE:
        CHECK_VIEWER_AND_MAINDB
        viewDishParams();
        break;
    }
}

bool QSale::actionFilter()
{
    if (QBaseGrid::actionFilter()) {
        m_settings->setValue("qsale_id_check", fieldByName("ID")->isIncluded());
        m_settings->setValue("qsale_date_cash_checked", fieldByName("DATE_CASH1")->isIncluded());
        m_settings->setValue("qsale_date_cash1", fieldByName("DATE_CASH1")->m_filter->listValues);
        m_settings->setValue("qsale_date_cash2", fieldByName("DATE_CASH2")->m_filter->listValues);
        m_settings->setValue("qsale_inc_value_checked", fieldByName("AMOUNT_INC_VALUE")->isIncluded());
        m_settings->setValue("qsale_dec_value_checked", fieldByName("AMOUNT_DEC_VALUE")->isIncluded());
        m_settings->setValue("qsale_dish_type_checked", fieldByName("DISH_TYPE_ID")->isIncluded());
        m_settings->setValue("qsale_dish_id_checked", fieldByName("DISH_ID")->isIncluded());
    }
    return true;
}

void QSale::getPrintData(XmlPrintMaker &x)
{
    DlgSelectSaleRepType *d = new DlgSelectSaleRepType(this);
    if (d->exec() == QDialog::Rejected) {
        QBaseGrid::getPrintData(x);
        delete d;
        return;
    }
    delete d;

    x.setFontName(font().family());
    x.setFontSize(9);

    int top = 5, left = 0, rowHeight = 6;
    x.text(windowTitle() + " / " + QSystem::objectName(), 0, top);
    top += x.lastTextHeight();

    SqlField *d1 = fieldByName("DATE_CASH1");
    SqlField *d2 = fieldByName("DATE_CASH2");
    SqlField *dishGroups = fieldByName("DISH_TYPE_ID");
    SqlField *dish = fieldByName("DISH_ID");

    x.setFontSize(12);
    x.text(tr("Common sale report"), left, top);
    top += x.lastTextHeight();
    x.text(QString("%1: %2 - %3").arg(tr("Date range")).arg(d1->m_filter->listValues).arg(d2->m_filter->listValues), left, top);
    top += x.lastTextHeight();
    x.text(QString("%1: %2 / %3").arg(tr("Orders"))
           .arg(tableWidget(1)->item(0, tableWidget(0)->columnCount() - 2)->data(Qt::DisplayRole).toDouble())
           .arg(dts(tableWidget(1)->item(0, tableWidget(0)->columnCount() - 1)->data(Qt::DisplayRole).toDouble())), left, top);
    top += x.lastTextHeight();


    x.setFontSize(9);
    x.text(QString("%1: %2").arg(tr("Dishes name view")
            .arg((dish->isIncluded() || dishGroups->isIncluded() ? QString("+") : QString("-")))),
           left, top);
    top += x.lastTextHeight();

    x.setFontSize(8);
    x.setFontItalic(true);
    top += 5;
    x.text(QString("%1: %2, %3, %4").arg(tr("Printed")).arg(QDateTime::currentDateTime().toString(DATETIME_FORMAT))
           .arg(getHostUsername()).arg(QSystem::username()), 0, top);
    x.finishPage();
}

void QSale::correctStore()
{
    SqlField *f = fieldByName("STORE_ID");
    if (!f->isIncluded()) {
        QMessageBox::warning(this, tr("Warning"), tr("The Store field must be included in the query"));
        return;
    }

    f = fieldByName("CASH_ID");
    if (!f->isIncluded()) {
        QMessageBox::warning(this, tr("Warining"), tr("The cash must be included in the query"));
        return;
    }

    f = fieldByName("DISH_ID");
    if (!f->isIncluded()) {
        QMessageBox::warning(this, tr("Warning"), tr("The Dish field must be included in the query"));
        return;
    }

    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    DlgCorrectOutStoreOfSales *d = new DlgCorrectOutStoreOfSales(this);
    d->setData(cellValue(l.toList()[0], fieldIndexInGrid("DISH_ID")).toString(),
            cellValue(l.toList()[0], fieldIndexInGrid("STORE_ID")).toInt(),
            cellValue(l.toList()[0], fieldIndexInGrid("CASH_ID")).toInt());
    d->setDate(QDate::fromString(fieldByName("DATE_CASH1")->m_filter->listValues, DATE_FORMAT), QDate::fromString(fieldByName("DATE_CASH2")->m_filter->listValues, DATE_FORMAT));
    d->exec();
    delete d;

}

void QSale::viewDishParams()
{
    SqlField *f = fieldByName("DISH_ID");
    if (!f->isIncluded()) {
        QMessageBox::warning(this, tr("Warning"), tr("The Dish field must be included in the query"));
        return;
    }

    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    GMenuItem *d = new GMenuItem(cellValue(l.toList()[0], fieldIndexInGrid("DISH_ID")).toInt(), this);
    d->show();
}

void QSale::slotContextMenu(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(tr("Copy total amount to clipboard"), this, SLOT(slotCopyContextMenuToClipboard()));
    m->popup(mapToGlobal(pos));
}

void QSale::slotCopyContextMenuToClipboard()
{
    int i = fieldIndexInGrid("AMOUNT");
    if (i < 0)
        return;
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }
    QClipboard *c = QApplication::clipboard();
    c->setText(cellValue(l.toList()[0], i).toString());
}
