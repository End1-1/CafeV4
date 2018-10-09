#include "gmenu.h"
#include "gmenuitem.h"
#include "magicmenu.h"
#include "dlgrecipe.h"
#include "dlgviewimage.h"

GMenu::GMenu(QWidget *parent) :
    QGrid(parent)
{
    m_lePrice = 0;
    m_rowDeleted = false;

    setItemDelegate(new ID(this));
    m_actions << "actionNew" << "actionEdit" << "actionDelete" << "actionRefresh" << "actionFilter"
              << "actionRecipe" << "actionMenu_2" << "actionMagic" << "actionMultiselection"
              << "actionExport_to_Excel" << "actionSearch" << "actionImage" << "actionCopy";
    m_editDlgTable = "ME_DISHES";

    setWindowTitle(tr("Dishes"));;

    SqlField *fId = new SqlField("ID", tr("Dish id"), 150, 0, QVariant::Int, 0, false);
    m_fields.append(fId);

    SqlField *fTypeId = new SqlField("TYPE_ID", tr("Type id"), 0, 0, QVariant::Int, 0, false);
    fTypeId->addFilter(Filter::ftEdit, true, "ME_TYPES");
    fTypeId->setInclude(true);
    m_fields.append(fTypeId);
    SqlField *fTypeName = new SqlField("TYPE_NAME", tr("Type name"), 250, 0, QVariant::String, 0, false);
    m_fields.append(fTypeName);

    SqlField *fMenuId = new SqlField("MENU_ID", tr("Menu id"), 0, 0, QVariant::Int, 0, false);
    fMenuId->addFilter(Filter::ftEdit, true, "ME_MENUS");
    fMenuId->m_filter->m_showCheckBox = true;
    fMenuId->setInclude(false);
    m_fields.append(fMenuId);

    SqlField *fMMID = new SqlField("MENU_REC_ID", tr("Menu rec id"), 0, 0, QVariant::Int, 0, false);
    fMMID->setIncludeRef(fMenuId);
    m_fields.append(fMMID);

    SqlField *fMenuName = new SqlField("MENU_NAME", tr("Menu name"), 200, 0, QVariant::String, 0, false);
    fMenuName->setIncludeRef(fMenuId);
    m_fields.append(fMenuName);

    SqlField *fDishName = new SqlField("NAME", tr("Dish name"), 350, 0, QVariant::String, 0, false);
    fDishName->setInclude(true);
    m_fields.append(fDishName);

    SqlField *fPrice = new SqlField("PRICE", tr("Price"), 80, 0, QVariant::Double, 0, false);
    fPrice->setInclude(false);
    fPrice->addFilter(Filter::ftEdit, false, "");
    fPrice->m_filter->m_showCheckBox = true;
    m_fields.append(fPrice);

    SqlField *fStoreId = new SqlField("STORE_ID", tr("Store id"), 0, 0, QVariant::Int, 0, false);
    fStoreId->addFilter(Filter::ftEdit, true, "ST_STORAGES");
    fStoreId->m_filter->m_showCheckBox = true;
    fStoreId->setInclude(false);
    m_fields.append(fStoreId);

    SqlField *fStoreName = new SqlField("STORE_NAME", tr("Store"), 200, 0, QVariant::String, 0, false);
    fStoreName->setIncludeRef(fStoreId);
    m_fields.append(fStoreName);

    SqlField *fPrintId = new SqlField("PRINT_SCHEMA_ID", tr("Print schema id"), 0, 0, QVariant::Int, 0, false);
    fPrintId->addFilter(Filter::ftEdit, true, "ME_PRINTER_SCHEMA");
    fPrintId->m_filter->m_showCheckBox = true;
    fPrintId->setInclude(false);
    m_fields.append(fPrintId);

    SqlField *fPrintName = new SqlField("PRINT_SCHEMA_NAME", tr("Print schema"), 200, 0, QVariant::String, 0, false);
    fPrintName->setIncludeRef(fPrintId);
    m_fields.append(fPrintName);

    SqlField *fPaymentMod = new SqlField("PAYMENT_MOD", tr("Payment mod id"), 0, 0, QVariant::Int, 0, false);
    m_fields.append(fPaymentMod);
    SqlField *fPaymentName = new SqlField("PAYMENT_NAME", tr("Payment"), 150, 0, QVariant::String, 0, false);
    m_fields.append(fPaymentName);
    SqlField *fColor = new SqlField("COLOR", tr("Color"), 0, 0, QVariant::Int, 0, false);
    m_fields.append(fColor);
    SqlField *fQueue = new SqlField("QUEUE", tr("Queue"), 100, 0, QVariant::Int, 0, false);
    m_fields.append(fQueue);

    SqlField *fImgLink = new SqlField("IMG_LINK", tr("Image link"), 10, 0, QVariant::String, 0, false);
    m_fields.append(fImgLink);
    actionBeforeSelect();

    QTableWidget *t = tableWidget(0);
    t->setSelectionMode(QAbstractItemView::SingleSelection);
    t->setSelectionBehavior(QAbstractItemView::SelectItems);
    connect(t, SIGNAL(currentCellChanged(int,int,int,int)), this, SLOT(gridCellChanged(int,int,int,int)));
}

void GMenu::actionBeforeSelect()
{
    if (m_lePrice) {
        m_lePrice->disconnect();
        tableWidget(0)->removeCellWidget(m_lePrice->m_data.at(0).toInt(), m_lePrice->m_data.at(1).toInt());
        delete m_lePrice;
        m_lePrice = 0;
    }

    QString select = "select md.id as id, md.type_id, t.name as type_name,";

    QString from = "from me_dishes md ";
    QString join = "left join me_dishes_mod pm on md.payment_mod=pm.id "
            "left join me_types t on md.type_id=t.id ";
    QString where = "";

    SqlField *f = fieldByName("TYPE_ID");
    f->haveFilter(where, "md.type_id");

    bool menuState = false;
    f = fieldByName("MENU_ID");
    if (f->isIncluded()) {
        select += "mm.menu_id, mm.id as menu_rec_id, mmm.name as menu_name,";
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
        f->sqlLeftJoin(join, "me_menus mmm", "mm.menu_id=mmm.id");
        menuState = true;
    }
    if (f->haveFilter(where, "mm.menu_id")) {
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
        menuState = true;
    }
    if (menuState)
        where += " mm.state_id=1 and";

    select += "md.name,";

    f = fieldByName("PRICE");
    if (f->isIncluded()) {
        select += "mm.price,";
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
    }

    f = fieldByName("STORE_ID");
    if (f->haveFilter(where, "mm.store_id")) {
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
    }
    if (f->isIncluded()) {
        select += "mm.store_id, st.name as store_name,";
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
        f->sqlLeftJoin(join, "st_storages st", "mm.store_id=st.id");
    }

    f = fieldByName("PRINT_SCHEMA_ID");
    if (f->haveFilter(where, "mm.print_schema" )) {
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
    }

    if (f->isIncluded()) {
        select += "mm.print_schema as print_schema_id,ps.name as print_schema_name,";
        f->sqlRightJoin(join, "me_dishes_menu mm", "mm.dish_id=md.id");
        f->sqlLeftJoin(join, "me_printer_schema ps", "mm.print_schema=ps.id");
    }

    if (where.length())
        where = "where " + where;

    remove_and(where);
    select += "md.payment_mod, pm.name as payment_name, md.color, md.queue, md.img_link ";
    m_sqlString = select + from + join + where;

    actionSelect();
}

void GMenu::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    GMenuItem *i = new GMenuItem(0, this);
    i->show();
}

void GMenu::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    int indexId = fieldIndexInGrid("ID");
    if (indexId < 0)
        return;

    QModelIndexList rowsIndexes;
    QTableWidget *t = tableWidget(0);
    rowsIndexes = t->selectionModel()->selectedIndexes();
    QSet<int> rows;
    for (QModelIndexList::const_iterator it = rowsIndexes.begin(); it != rowsIndexes.end(); it++)
        rows.insert(it->row());
    if (!rows.count()) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }

    GMenuItem *g = new GMenuItem(cellValue(rows.values().at(0), indexId).toInt(), this);
    g->show();
}

void GMenu::actionDelete()
{
    CHECK_VIEWER_AND_MAINDB
    int indexId = fieldIndexInGrid("ID");
    if (indexId < 0)
        return;

    QSet<int> rows;
    if (!QBaseSqlWindow::selectedRows(tableWidget(0), rows)) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
        return;

    if (m_lePrice) {
        m_lePrice->disconnect();
        tableWidget(0)->removeCellWidget(m_lePrice->m_data.at(0).toInt(), m_lePrice->m_data.at(0).toInt());
        delete m_lePrice;
        m_lePrice = 0;
    }
    // If no menu is specified, remove item from all menus, otherwise remove only from selected menus
    int indexMenuId = fieldIndexInGrid("MENU_ID");
    m_sqlDriver->openDB();
    int currCol = tableWidget(0)->currentColumn();
    for (QSet<int>::const_iterator it = rows.begin(); it != rows.end(); it++) {
        if (!m_rowDeleted) {
            m_rowDeleted = true;
            tableWidget(0)->setCurrentCell(*it, 0);
        }
        if (indexMenuId < 0) {
            m_sqlDriver->prepare("update me_dishes_menu set state_id=0 where dish_id=:dish_id");
            m_sqlDriver->bind(":dish_id", cellValue(*it, indexId));
            m_sqlDriver->execSQL();
        } else {
            m_sqlDriver->prepare("update me_dishes_menu set state_id=0 where menu_id=:menu_id and dish_id=:dish_id");
            m_sqlDriver->bind(":menu_id", cellValue(*it, indexMenuId));
            m_sqlDriver->bind(":dish_id", cellValue(*it, indexId));
            m_sqlDriver->execSQL();

        }
    }

    QList<int> r = rows.toList();
    qSort(r.begin(), r.end());
    for (int i = r.count() - 1; i > -1; i--)
        removeRow(r.at(i));

    m_rowDeleted = false;
    tableWidget(0)->setCurrentCell(tableWidget(0)->currentRow(), currCol);
    m_sqlDriver->close();
}

void GMenu::actionMultiSelect(QToolBar *tb)
{
    QBaseGrid::actionMultiSelect(tb);
}

void GMenu::actionCostum(int action)
{
    CHECK_VIEWER_AND_MAINDB
    switch (action) {
    case ACTION_MAGIC:
        editMagic();
        break;
    case ACTION_RECIPE:
        editRecipe();
        break;
    case ACTION_IMG:
        editImage();
        break;
    }
}


void GMenu::editMagic()
{
    int indexId = fieldIndexInGrid("ID");
    if (indexId < 0)
        return;
    QSet<int> rows;
    if (!selectedRows(rows)) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }

    MagicMenu *m = new MagicMenu(this);
    if (m->exec() == QDialog::Accepted) {
        QList<int> dishes;
        for (QSet<int>::const_iterator i = rows.begin(); i != rows.end(); i++)
            dishes.append(cellValue(*i, indexId).toInt());

        QMap<QString, QVariant> values = m->values();
        QStringList menus = values["menuList"].toString().split(",");
        QString menuCondition = "(";
        bool first = true;
        for (QStringList::const_iterator i = menus.begin(); i != menus.end(); i++) {
            if (first)
                first = false;
            else
                menuCondition += ",";
            menuCondition += *i;
        }
        menuCondition += ")";

        /* Remove items from unused menus */
        if (values["deleteFromMenu"].toBool()) {
            m_sqlDrv->prepare("delete from me_dishes_menu where dish_id=:dish_id and menu_id in " + menuCondition);
            for (QList<int>::const_iterator i = dishes.begin(); i != dishes.end(); i++) {
                m_sqlDrv->bind(":dish_id", *i);
                m_sqlDrv->execSQL();
            }
        }
        /* End of remove items from unused menus */

        /* ME_DISHES */
        /* Edit color */
        if (values["editColor"].toBool()) {
            m_sqlDrv->m_sql = "update me_dishes set color=:color where id=:id";
            if (values["applyInSelectedMenu"].toBool())
                m_sqlDrv->m_sql += " and id in (select dish_id from me_dishes_menu where menu_id in " + menuCondition + ")";

            m_sqlDrv->prepare();
            m_sqlDrv->bind(":color", values["color"].toInt());
            for (QList<int>::const_iterator i = dishes.begin(); i != dishes.end(); i++) {
                m_sqlDrv->bind(":id", *i);
                m_sqlDrv->execSQL();
            }
        }
        /* End of edit color */
        /* Remind */
        if (values["chEditRemind"].toBool()) {
            m_sqlDrv->m_sql = "update me_dishes set remind=:remind where id=:id";
            if (values["applyInSelectedMenu"].toBool())
                m_sqlDrv->m_sql += " and id in (select dish_id from me_dishes_menu where menu_id in " + menuCondition + ")";

            m_sqlDrv->prepare();
            m_sqlDrv->bind(":remind", values["remind"].toInt());
            for (QList<int>::const_iterator i = dishes.begin(); i != dishes.end(); i++) {
                m_sqlDrv->bind(":id", *i);
                m_sqlDrv->execSQL();
            }
        }
        /* End of remind */
        /* Payment mod */
        if (values["chEditPayment"].toBool()) {
            m_sqlDrv->m_sql = "update me_dishes set payment_mod=:payment_mod where id=:id";
            if (values["applyInSelectedMenu"].toBool())
                m_sqlDrv->m_sql += " and id in (select dish_id from me_dishes_menu where menu_id in " + menuCondition + ")";

            m_sqlDrv->prepare();
            m_sqlDrv->bind(":payment_mod", values["payment"].toInt());
            for (QList<int>::const_iterator i = dishes.begin(); i != dishes.end(); i++) {
                m_sqlDrv->bind(":id", *i);
                m_sqlDrv->execSQL();
            }
        }
        /* End of payment mod */

        /* ME_DISHES_MENU */
        bool flagUpdate = false;
        QString sqlUpdate = "update me_dishes_menu set ";
        if (values["editStore"].toBool()) {
            flagUpdate = true;
            sqlUpdate += "store_id=" + values["store"].toString() + ",";
        }
        if (values["editPrintSchema"].toBool()) {
            flagUpdate = true;
            sqlUpdate += "print_schema=" + values["printSchema"].toString() + ",";
        }
        if (values["chState"].toBool()) {
            flagUpdate = true;
            sqlUpdate += "state_id=" + values["state"].toString() + ",";
        }
        if (flagUpdate) {
            remove_zap(sqlUpdate);
            QString dishIds;
            bool first = true;
            for (QList<int>::const_iterator i = dishes.begin(); i != dishes.end(); i++) {
                if (first)
                    first = false;
                else
                    dishIds += ",";
                dishIds += its(*i);
            }
            sqlUpdate += " where dish_id in (" + dishIds + ")";
            if (values["applyInSelectedMenu"].toBool())
                sqlUpdate += " and id in (select id from me_dishes_menu where menu_id in " + menuCondition + ")";
            m_sqlDrv->execSQL(sqlUpdate);
        }

        m_sqlDrv->close();
        actionRefresh();
    }
    delete m;
}

void GMenu::editRecipe()
{
    int indexId = fieldIndexInGrid("ID");
    if (indexId < 0)
        return;
    QSet<int> rows;
    if (!selectedRows(rows)) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }

    DlgRecipe *d = new DlgRecipe(cellValue(rows.toList().at(0), 0).toInt(), this);
    d->show();
}

void GMenu::editImage()
{
    int indexId = fieldIndexInGrid("ID");
    int indexImg = fieldIndexInGrid("IMG_LINK");
    if ((indexId < 0) || (indexImg < 0))
        return;
    QSet<int> rows;
    if (!selectedRows(rows)) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }

    QString imgLink = cellValue(rows.toList().at(0), indexImg).toString();
    DlgViewImage *d = new DlgViewImage(cellValue(rows.toList().at(0), indexId).toInt(), imgLink, 3, this);
    d->exec();
    setCellValue(rows.toList().at(0), indexImg, d->m_imgLink);
    delete d;
}

void GMenu::gridCellChanged(int new_row, int new_col, int previous_row, int previous_col)
{
    QTableWidget *t = tableWidget(0);
    int priceCol = fieldIndexInGrid("PRICE");
    if (previous_col == priceCol && previous_row > -1) {
        m_lePrice->disconnect();
        t->removeCellWidget(previous_row, previous_col);
        m_lePrice->deleteLater();
        m_lePrice = 0;
    }

    if (new_col < 0 || new_row < 0)
        return;

    if (fieldByName("PRICE")->isIncluded() && fieldByName("MENU_REC_ID")->isIncluded()) {
        if (m_rowDeleted)
            return;
        if (new_col == priceCol && new_row > -1) {

            m_lePrice = new QPLineEdit();
            m_lePrice->setFrame(false);
            m_lePrice->setValidator(new QDoubleValidator(0, 1000000, 2));
            connect(m_lePrice, SIGNAL(returnPressed()), this, SLOT(gridPriceChanged()));
            t->setCellWidget(new_row, priceCol, m_lePrice);

            m_lePrice->setText(cellValue(new_row, priceCol).toString());
            m_lePrice->m_data.clear();
            m_lePrice->m_data.append(new_row);
            m_lePrice->m_data.append(priceCol);
            m_lePrice->m_data.append(cellValue(new_row, fieldIndexInGrid("MENU_REC_ID")));
            m_lePrice->setFocus();
            m_lePrice->selectAll();
        }
    }

}

void GMenu::gridPriceChanged()
{
    CHECK_VIEWER_AND_MAINDB
    QPLineEdit *e = qobject_cast<QPLineEdit*>(sender());
    e->disconnect();
    m_sqlDriver->openDB();
    m_sqlDriver->prepare("update me_dishes_menu set price=:price where id=:id");
    m_sqlDriver->bind(":price", e->text().toDouble());
    m_sqlDriver->bind(":id", e->m_data.at(2));
    m_sqlDriver->execSQL();
    m_sqlDriver->close();
    setCellValue(e->m_data.at(0).toInt(), e->m_data.at(1).toInt(), e->text().toDouble());
    int row = e->m_data.at(0).toInt();
    QTableWidget *t = tableWidget(0);
    if (row < t->rowCount() - 1)
        t->setCurrentCell(row + 1, e->m_data.at(1).toInt());

}

void GMenu::on_grid_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    actionEdit();
}


GMenu::ID::ID(GMenu *gmenu)
{
    m_gmenu = gmenu;
}

void GMenu::ID::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    int i = m_gmenu->fieldIndexInGrid("COLOR");
    if (i < 0)
        return;
    if (option.state & QStyle::State_Selected) {
        QItemDelegate::paint(painter, option, index);
        return;
    }
    painter->save();
    QBrush brush(QColor(m_gmenu->itemData(index.row(), i).toInt()));
    painter->setBrush(brush);
    QPen pen(Qt::NoPen);
    painter->setPen(pen);
    QRect r = option.rect;
    r.adjust(1, 1, 0, 0);
    painter->drawRect(option.rect);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawText(r, index.data().toString());
    painter->restore();
}
