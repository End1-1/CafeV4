#include "qfoods.h"
#include "dlggoods.h"
#include "dlgdatepair.h"

QFoods::QFoods(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "FOOD_NAMES";
    m_actions << "actionNew" << "actionEdit" << "actionRefresh" << "actionFilter"
              << "actionExport_to_Excel" << "actionSearch" << "actionView_prices";
    setWindowTitle(tr("Goods"));

    SqlField *fId = new SqlField("ID", tr("Id"), 100, 0, QVariant::Int, 0, false);
    m_fields.append(fId);

    SqlField *fGroup = new SqlField("GROUP_ID", tr("Group id"), 0, 0, QVariant::Int, 0, false);
    fGroup->addFilter(Filter::ftEdit, true, "FOOD_GROUPS");
    m_fields.append(fGroup);

    SqlField *fGroupName = new SqlField("GROUP_NAME", tr("Group"), 150, 0, QVariant::String, 0, false);
    m_fields.append(fGroupName);

    SqlField *fUnit = new SqlField("UNIT_ID", tr("Unit id"), 0, 0, QVariant::Int, 0, false);
    fUnit->addFilter(Filter::ftEdit, true, "FOOD_UNITS");
    m_fields.append(fUnit);

    SqlField *fUnitName = new SqlField("UNIT_NAME", tr("Unit"), 100, 0, QVariant::String, 0, false);
    m_fields.append(fUnitName);

    SqlField *fName = new SqlField("NAME", tr("Name"), 300, 0, QVariant::String, 0, false);
    m_fields.append(fName);

    SqlField *fAutoWaste = new SqlField("AUTO_WASTE", tr("Waste"), 80, 0, QVariant::Double, 0, false);
    m_fields.append(fAutoWaste);

    addField("PRICE", tr("Price"), 100, QVariant::Double);
    addField("AUTO_OUT", tr("Autoout"), 100, QVariant::Int);

    actionBeforeSelect();
}

void QFoods::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgGoods>();
}

void QFoods::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgGoods>(false);
}

void QFoods::actionBeforeSelect()
{
    QString where = "where fn.group_id=fg.id and fn.unit_id=fu.id and";
    SqlField *f = fieldByName("GROUP_ID");
    f->haveFilter(where, "fn.group_id");

    f = fieldByName("UNIT_ID");
    f->haveFilter(where, "fn.unit_id");

    remove_and(where);

    m_sqlString = "select fn.id, fn.group_id, fg.name as group_name, "
            "fn.unit_id, fu.name as unit_name, fn.name, fn.price, fn.auto_out "
            "from food_names fn, food_groups fg, food_units fu "
            + where
            + " order by fg.name, fn.name";
    actionSelect();
}

void QFoods::actionCostum(int action)
{
    switch (action) {
    case ACTION_VIEW_PRICES:
        updatePrices();
        break;
    }
}

void QFoods::updatePrices()
{
    if (DlgDatePair::getDate(date1, date2)) {
        QMap<int, double> goodsPrices;
        m_sqlDriver->prepare("select sd.goods_id, sum(sd.amount / sd.qty) "
                             "from st_documents so, st_draft sd "
                             "where so.id=sd.doc_id and so.doc_date between :d1 and :d2 "
                             "and so.action_id in (1,5) "
                             "group by 1");
        m_sqlDriver->bind(":d1", date1);
        m_sqlDriver->bind(":d2", date2);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            goodsPrices[m_sqlDriver->val().toInt()] = m_sqlDriver->val().toDouble();

        for (QMap<int, double>::const_iterator it = goodsPrices.begin(); it != goodsPrices.end(); it++) {
            m_sqlDriver->prepare("update food_names set price=:price where id=:id");
            m_sqlDriver->bind(":price", it.value());
            m_sqlDriver->bind(":id", it.key());
            m_sqlDriver->execSQL();
        }

        QMessageBox::information(this, tr("Information"), tr("You need refresh view to see changes"));
    }
}
