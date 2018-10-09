#include "gdocinstore.h"
#include "storeorder.h"

GDocInStore::GDocInStore(QWidget *parent) :
    QGrid(parent)
{
    m_actions << "actionNew" << "actionEdit" << "actionDelete" << "actionFilter"
              << "actionRefresh" << "actionExport_to_Excel" << "actionPrint" << "actionPrint_preview";

    setWindowTitle(tr("Storage documents"));

    SqlField *fDocId = new SqlField("ID", tr("Id"), 100, 0, QVariant::Int, 0, false);
    fDocId->addFilter(Filter::ftEdit, false, "");
    fDocId->setInclude(true);
    fDocId->m_filter->m_showCheckBox = true;
    m_fields.append(fDocId);

    SqlField *fDate1 = new SqlField("DATE1", tr("Date start"), 120, 0, QVariant::Date, 0, false);
    fDate1->addFilter(Filter::ftDate, false, "");
    fDate1->setInclude(true);
    fDate1->m_filter->m_showCheckBox = true;
    fDate1->m_filter->listValues = QDate::currentDate().toString(DATE_FORMAT);
    m_fields.append(fDate1);

    SqlField *fDate2 = new SqlField("DATE2", tr("Date end"), 0, 0, QVariant::Date, 0, false);
    fDate2->addFilter(Filter::ftDate, false, "");
    fDate2->m_filter->listValues = QDate::currentDate().toString(DATE_FORMAT);
    fDate2->setInclude(false);
    m_fields.append(fDate2);

    SqlField *fActionId = new SqlField("ACTION_ID", tr("Action id"), 0, 0, QVariant::Int, 0, false);
    fActionId->addFilter(Filter::ftEdit, true, "ST_ACTIONS");
    fActionId->m_filter->m_showCheckBox = true;
    m_fields.append(fActionId);

    SqlField *fActionName = new SqlField("ACTION_NAME", tr("Action"), 200, 0, QVariant::String, 0, false);
    fActionName->setIncludeRef(fActionId);
    m_fields.append(fActionName);

    SqlField *fStoreIn = new SqlField("STORE_INPUT", tr("Store in code"), 0, 0, QVariant::Int, 0, false);
    fStoreIn->addFilter(Filter::ftEdit, true, "ST_STORAGES");
    fStoreIn->m_filter->m_showCheckBox = true;
    m_fields.append(fStoreIn);

    SqlField *fStoreInName = new SqlField("STORE_IN_NAME", tr("Store in"), 100, 0, QVariant::String, 0, false);
    fStoreInName->setIncludeRef(fStoreIn);
    m_fields.append(fStoreInName);

    SqlField *fStoreOut = new SqlField("STORE_OUTPUT", tr("Store out code"), 0, 0, QVariant::Int, 0, false);
    fStoreOut->addFilter(Filter::ftEdit, true, "ST_STORAGES");
    fStoreOut->m_filter->m_showCheckBox = true;
    m_fields.append(fStoreOut);

    SqlField *fStoreOutName = new SqlField("STORE_OUT_NAME", tr("Store out"), 100, 0, QVariant::String, 0, false);
    fStoreOutName->setIncludeRef(fStoreOut);
    m_fields.append(fStoreOutName);

    SqlField *fGoodsId = new SqlField("GOODS_ID", tr("Goods code"), 0, 0, QVariant::Int, 0, false);
    fGoodsId->addFilter(Filter::ftEdit, true, "FOOD_NAMES");
    fGoodsId->m_filter->m_showCheckBox = true;
    fGoodsId->setInclude(false);
    m_fields.append(fGoodsId);

    SqlField *fGoodsName = new SqlField("GOODS_NAME", tr("Goods"), 200, 0, QVariant::String, 0, false);
    fGoodsName->setIncludeRef(fGoodsId);
    m_fields.append(fGoodsName);

    SqlField *fComment = new SqlField("COMMENT", tr("Comment"), 200, 0, QVariant::String, 0, false);
    m_fields.append(fComment);

    SqlField *fQty = new SqlField("QTY", tr("Qty"), 100, 0, QVariant::Double, 0, false);
    fQty->setInclude(false);
    m_fields.append(fQty);

    SqlField *fAmount = new SqlField("AMOUNT", tr("Amount"), 120, 0, QVariant::Double, 0, false);
    m_fields.append(fAmount);

    actionBeforeSelect();
}

void GDocInStore::actionNew()
{
    CHECK_VIEWER
    StoreOrder *s = new StoreOrder("0", this);
    s->show();
}

void GDocInStore::actionBeforeSelect()
{
    SqlField *fDate1 = fieldByName("DATE1");
    SqlField *fDate2 = fieldByName("DATE2");

    QString select = "select ",
            from = " from st_documents sd ",
            join = " ",
            group = " group by ",
            where = " where sd.doc_date between '" + fDate1->m_filter->listValues + "' and '" + fDate2->m_filter->listValues + "' and";

    SqlField *f;
    f = fieldByName("ID");
    if (f->isIncluded()) {
        select += "sd.id,";
        group += "sd.id,";
    }
    f->haveFilter(where, "sd.id");

    f = fieldByName("DATE1");
    if (f->isIncluded()) {
        select += "sd.doc_date as date1,";
        group += "sd.doc_date,";
    }

    f = fieldByName("ACTION_ID");
    if (f->isIncluded()) {
        select += "sa.id as action_id,sa.name as action_name,";
        group += "sa.id,sa.name,";
        join += "left join st_actions sa on sd.action_id=sa.id ";
    }
    if (f->haveFilter(where, "sa.id"))
        f->sqlLeftJoin(join, "st_actions sa", "sd.action_id=sa.id");

    f = fieldByName("STORE_INPUT");
    if (f->isIncluded()) {
        select += "sd.store_input,s1.name as store_in_name,";
        group += "sd.store_input,s1.name,";
        join += "left join st_storages s1 on s1.id=sd.store_input ";
    }
    if (f->haveFilter(where, "s1.id"))
        f->sqlLeftJoin(join, "st_storages s1", "s1.id=sd.store_input");

    f = fieldByName("STORE_OUTPUT");
    if (f->isIncluded() || f->m_filter->listValues.length()) {
        select += "sd.store_output,s2.name as store_out_name,";
        group += "sd.store_output,s2.name,";
        join += "left join st_storages s2 on s2.id=sd.store_output ";
    }
    if (f->haveFilter(where, "s2.id"))
        f->sqlLeftJoin(join, "st_storages s2", "s2.id=sd.store_output");

    f = fieldByName("GOODS_ID");
    if (f->isIncluded()) {
        select += "fn.id as goods_id,fn.name as goods_name,";
        group += "fn.id,fn.name,";
        join += "left join st_draft sdd on sdd.doc_id=sd.id ";
        join += "left join food_names fn on fn.id=sdd.goods_id ";
    }
    if (f->haveFilter(where, "sdd.goods_id")) {
        f->sqlLeftJoin(join, "st_draft sdd", "sdd.doc_id=sd.id");
    }

    select += "sd.comment,";
    group += "sd.comment,";

    SqlField *fQty = fieldByName("QTY");
    f = fieldByName("GOODS_ID");
    if (f->isIncluded()) {
        fQty->setInclude(true);
        select += "sum(sdd.qty) as qty, sum(sdd.amount) as amount,";
        m_totalRowTitles << fieldByName("QTY")->title << fieldByName("AMOUNT")->title;
        m_totalRowTitles << fieldByName("AMOUNT")->title << fieldByName("AMOUNT")->title;
    } else {
        fQty->setInclude(false);
        select += "sum(sd.amount) as amount";
        m_totalRowTitles << fieldByName("AMOUNT")->title;
    }

    remove_zap(select);
    remove_zap(from);
    remove_zap(group);
    remove_and(where);

    m_sqlString = select + from + join + where + group;

    actionSelect();
}

void GDocInStore::actionEdit()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    StoreOrder *storeOrder = new StoreOrder(cellValue(l.toList().at(0), 0).toString(), this);
    storeOrder->show();
}

void GDocInStore::actionDelete()
{
    CHECK_VIEWER
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    for (int i = l.count() - 1; i > -1; i--) {
        m_sqlDrv->prepare("delete from st_documents where id=:id");
        m_sqlDrv->bind(":id", cellValue(l.toList().at(i), 0).toInt());
        m_sqlDrv->execSQL();
        removeRow(l.toList().at(i));
    }

    m_sqlDrv->close();
}
