#include "qtotalrecipes.h"
#include "dlgrecipe.h"
#include "dlggetdoublevalue.h"
#include "gmenuitem.h"

#define COL_COUNT 12
#define COL_GOODS_ID 5
#define COL_GOODS_NAME 6
#define COL_GOODS_QTY 7
#define COL_PRICE_OF_GOODS 8
#define COL_AMOUNT_OF_GOODS 10

QTotalRecipes::QTotalRecipes(QWidget *parent) :
    QGrid(parent)
{
    m_actions << "actionFilter" << "actionRefresh" << "actionExport_to_Excel"
              << "actionPrint" << "actionPrint_preview" << "actionEdit" << "actionSearch"
              << "actionView_prices" << "actionDelete" << "actionMultiselection";
    setWindowTitle(tr("Recipes"));

    SqlField *fRowId = addField("ID", tr("Id"), 0, QVariant::Int);

    SqlField *fType = addField("TYPE_ID", tr("Type id"), 0, QVariant::Int);
    fType->addFilter(Filter::ftEdit, true, "ME_TYPES");
    addField("TYPE_NAME", tr("Type"), 150, QVariant::String);

    addField("DISH_ID", tr("Dish id"), 0, QVariant::Int);
    addField("DISH_NAME", tr("Dish"), 250, QVariant::String);

    SqlField *fGoods = addField("GOODS_ID", tr("Goods id"), 0, QVariant::Int);
    fGoods->addFilter(Filter::ftEdit, true, "FOOD_NAMES");
    addField("GOODS_NAME", tr("Goods"), 200, QVariant::String);

    addField("QTY", tr("Qty"), 80, QVariant::Double);
    addField("UNIT_NAME", tr("Unit"), 50, QVariant::String);
    addField("PRICE", tr("Price"), 80, QVariant::Double);
    addField("TOTAL", tr("Total"), 80, QVariant::Double);

    SqlField *fStore = addField("STORE_ID", tr("Store"), 0, QVariant::Int);
    fStore->addFilter(Filter::ftEdit, true, "ST_STORAGES");

    SqlField *fMenu = addField("MENU_ID", tr("Menu"), 0, QVariant::Int);
    fMenu->addFilter(Filter::ftEdit, true, "ME_MENUS");

    SqlField *fNoRecipe = addField("NO_RECIPE", tr("No recipe"), 0, QVariant::Int);
    fNoRecipe->addFilter(Filter::ftEdit, false, "");
    fNoRecipe->m_filter->m_showCheckBox = true;
    fNoRecipe->setInclude(false);

    QStringList captions;
    captions << tr("Row id") << tr("Type id") << tr("Type") << tr("Dish id") << tr("Dish")
             << tr("Goods id") << tr("Goods") << tr("Qty") << tr("Unit")
             << tr("Price") << tr("Total") << tr("Selfcost");
    QList<int> widths;
    widths << 0 << 0 << 150 << 0 << 250 << 0 << 200 << 80 << 50 << 80 << 80 << 80;
    setCaptions(captions, widths);

    tableWidget(0)->setSelectionMode(QAbstractItemView::SingleSelection);

    actionBeforeSelect();
}

void QTotalRecipes::actionEdit()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    int dishId = cellValue(l.toList().at(0), 3).toInt();
    if (!dishId) {
        QMessageBox::warning(this, tr("Warning"), tr("Row doesnt contain an dish id"));
        return;
    }

    GMenuItem *d = new GMenuItem(dishId, this);
    d->setTabIndex(2);
    d->show();
}

void QTotalRecipes::actionBeforeSelect()
{
    actionSelect();
}

void QTotalRecipes::actionSelect()
{
    QTableWidget *t = tableWidget(0);
    t->clearContents();
    t->setRowCount(500);

    QMap<int, QString> menus;
    QMap<int, int> menusCol;
    m_sqlDrv->execSQL("select id , name from me_MENUS");
    CustomHeader *h = static_cast<CustomHeader*>(t->horizontalHeader());
    while (m_sqlDrv->next()) {
        int menuId = m_sqlDrv->val().toInt();
        menus[menuId] = m_sqlDrv->val().toString();
        menusCol[menuId] = menus.count();
        h->m_captions.append(menus[menuId]);
    }
    t->setColumnCount(COL_COUNT + menus.count());

    QMap<int, QMap<int, float> > prices;
    int dish_id = 0;
    m_sqlDrv->execSQL("select dish_id, menu_id, price from me_dishes_menu");
    while (m_sqlDrv->next()) {
        dish_id = m_sqlDrv->val().toInt();
        if (!prices.contains(dish_id))
            prices[dish_id] = QMap<int, float>();
        prices[dish_id][m_sqlDrv->val().toInt()] = m_sqlDrv->val().toFloat();
    }

    QString where = "where md.id>0 and", join;

    SqlField *f = fieldByName("TYPE_ID");
    f->haveFilter(where, "mt.id");

    f = fieldByName("GOODS_ID");
    if (f->m_filter->listValues.length())
        where += " md.id in (select dish_id from me_recipes where goods_id in (" + f->m_filter->listValues + ")) and";

    f = fieldByName("STORE_ID");
    if (f->haveFilter(where, "mm.store_id"))
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");

    f = fieldByName("MENU_ID");
    if (f->haveFilter(where, "mm.menu_id")) {
        where += " mm.state_id=1 and";
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
    }

    SqlField *fNoRecipe = fieldByName("NO_RECIPE");

    remove_and(where);

    m_sqlDrv->prepare("select mr.id, mt.id as type_id, mt.name as type_name,"
                      "md.id as dish_id, md.name as dish_name,"
                      "fn.id as goods_id, fn.name as goods_name,"
                      "mr.qty,fu.name as unit_name,fn.price, mr.qty*fn.price as total "
                      "from me_dishes md "
                      "left join me_types mt on md.type_id=mt.id "
                      "left join me_recipes mr on md.id=mr.dish_id "
                      "left join food_names fn on mr.goods_id=fn.id "
                      "left join food_units fu on fn.unit_id=fu.id "
                      + join
                      + where
                      + "order by mt.name,md.name,mr.id ");

    m_sqlDrv->execSQL();
    int r = 0;
    dish_id = 0;
    int curre_dish_id = 0;
    double selfCost = 0;
    while (m_sqlDrv->next()) {
        if (fNoRecipe->isIncluded())
            if (m_sqlDrv->m_query->value(5).toInt())
                continue;

        dish_id = m_sqlDrv->m_query->value(3).toInt();

        if (curre_dish_id && (dish_id != curre_dish_id)) {
            for (int i = 0; i < t->columnCount(); i++)
                t->setItem(r, i, new QTableWidgetItem(""));
            t->setItem(r - 1, 11, new QTableWidgetItem(dts(selfCost)));
            if (prices.contains(curre_dish_id)) {
                for (QMap<int, float>::const_iterator i = prices[curre_dish_id].begin(); i != prices[curre_dish_id].end(); i++)
                    t->setItem(r - 1, COL_COUNT + menusCol[i.key() - 1], new QTableWidgetItem(dts(i.value())));
            }
            //addRow(t, r);
            for (int i = 0; i < t->columnCount(); i++)
                t->setItem(r, i, new QTableWidgetItem(""));
            addRow(t, r);
            selfCost = 0;
        }

        for (int i = 0; i < COL_COUNT; i++)
            t->setItem(r, i, new QTableWidgetItem(m_sqlDrv->val().toString()));
        for (int i = COL_COUNT; i < t->columnCount(); i++)
            t->setItem(r, i, new QTableWidgetItem(""));
        selfCost += m_sqlDrv->m_query->value(10).toFloat();
        curre_dish_id = dish_id;
        addRow(t, r);
    }
    if (dish_id) {
        for (int i = 0; i < t->columnCount(); i++)
            t->setItem(r, i, new QTableWidgetItem(""));
        t->setItem(r, 10, new QTableWidgetItem(dts(selfCost)));
    }
    t->setRowCount(r + 1);
    t->setSortingEnabled(false);
}

void QTotalRecipes::actionCostum(int action)
{
    CHECK_VIEWER_AND_MAINDB
    switch (action) {
    case ACTION_VIEW_PRICES:
        updateGoodsPrice();
        break;
    }
}

void QTotalRecipes::actionDelete()
{
    CHECK_VIEWER_AND_MAINDB
    QSet<int> rows;
    if (!QBaseSqlWindow::selectedRows(tableWidget(0), rows)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    QList<int> r = rows.toList();
    qSort(r.begin(), r.end());
    m_sqlDrv->openDB();
    m_sqlDrv->prepare("delete from me_recipes where id=:id");
    for (int i = r.count() - 1; i > -1; i--) {
        m_sqlDrv->bind(":id", cellValue(r.at(i), 0));
        m_sqlDrv->execSQL();
        tableWidget(0)->removeRow(r.at(i));
    }
    m_sqlDrv->close();
}

void QTotalRecipes::slotGridHeaderClicked(int index)
{
    Q_UNUSED (index)
}

void QTotalRecipes::addRow(QTableWidget *t, int &r)
{
    r++;
    if (r >= t->rowCount() - 1)
        t->setRowCount(r + 500);
}

void QTotalRecipes::updateGoodsPrice()
{
    QTableWidget *t = tableWidget(0);
    int curr = t->currentRow();

    if (!cellValue(curr, COL_GOODS_ID).toInt())
        return;

    double value = cellValue(curr, COL_PRICE_OF_GOODS).toDouble();
    int id = cellValue(curr, COL_GOODS_ID).toInt();
    if (!DlgGetDoubleValue::getDoubleValue(value, cellValue(curr, COL_GOODS_NAME).toString(), this))
        return;

    m_sqlDrv->prepare("update food_names set price=:price where id=:id");
    m_sqlDrv->bind(":price", value);
    m_sqlDrv->bind(":id", id);
    m_sqlDrv->execSQL();

    for (int i = 0; i < t->rowCount(); i++)
        if (cellValue(i, COL_GOODS_ID).toInt() == id) {
            setCellValue(i, COL_PRICE_OF_GOODS, value);
            int top = 0, bottom = 0;
            getRowTopBottom(i, top, bottom);
            updateSelfCost(top, bottom);
        }
}

void QTotalRecipes::updateSelfCost(int t, int b) {
    double total = 0;
    for (int i = t; i < b + 1; i++) {
        setCellValue(i, COL_AMOUNT_OF_GOODS, cellValue(i, COL_GOODS_QTY).toDouble() * cellValue(i, COL_PRICE_OF_GOODS).toDouble());
        total += cellValue(i, COL_AMOUNT_OF_GOODS).toDouble();
    }

    setCellValue(b + 1, COL_AMOUNT_OF_GOODS, total);
}

bool QTotalRecipes::getRowTopBottom(int curr, int &top, int &bottom)
{
    for (int i = curr - 1; i > -1; i--)
        if (cellValue(i, 0).toInt() == 0) {
            top = i + 1;
            break;
        }

    for (int i = curr + 1; i < tableWidget(0)->rowCount() - 1; i++)
        if (cellValue(i, 0).toInt() == 0 || i == tableWidget(0)->rowCount() - 1){
            bottom = i - 1;
            break;
        }

    return true;
}

