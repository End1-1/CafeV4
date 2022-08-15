#include "grmenu.h"
#include "imenuitem.h"

GRMenu::GRMenu() :
    GridWidget()
{
    mMainTable = "me_dishes_menu mu";
    mLeftJoinTables["mm"] = "left join me_menus mm on mm.id=mu.menu_id";
    mLeftJoinTables["mt"] = "left join me_types mt on mt.id=md.type_id";
    mLeftJoinTables["md"] = "left join me_dishes md on md.id=mu.dish_id";
    mLeftJoinTables["st"] = "left join st_storages st on st.id=mu.store_id";
    addColumn("md.id as code", tr("Code"), true, "md", "");
    addColumn("mm.name as menuname", tr("Menu"), true, "mm", "");
    addColumn("mt.name as typename", tr("Type"), true, "mt", "md");
    addColumn("mu.state_id", tr("State"), false, "", "");
    addColumn("md.name as dishname", tr("Dish"), true, "md", "");
    addColumn("mu.price", tr("Price"), true, "", "");
    addColumn("mu.store_id", tr("Store code"), false, "", "");
    addColumn("st.name as storename", tr("Store"), true, "st", "");
    addColumn("mu.print1 as print1", tr("Printer 1"), true, "", "");
    addColumn("mu.print2 as print2", tr("Printer 2"), true, "", "");
    mEditorWidget = new IMenuItem();
}

void GRMenu::prepare()
{
    mMenu.addAction(QIcon(":/img/new.png"), tr("New"), this, SLOT(newItem()));
}
