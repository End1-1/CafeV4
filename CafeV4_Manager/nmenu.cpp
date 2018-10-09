#include "nmenu.h"
#include "cgridwidget.h"
#include "qbasesqlwindow.h"

NMenu::NMenu(QWidget *parent) :
    MdiWindow(parent)
{
    showMaximized();
    fMdiButton = new MdiButton(this, tr("Menu"));
    QBaseSqlWindow::m_mdiArea->addSubWindow(this);
    setToolbar(QBaseSqlWindow::m_toolBarBtn);
    fGrid = new CGridWidget("8b6bd5fb-4434-434d-9f9a-ce4b54bc74df");
    setCentralWidget(fGrid);
    fGrid->setColumnName("ID", tr("Row"), 0)
            .setColumnName("MENUNAME", tr("Menu"), 120)
            .setColumnName("TYPENAME", tr("Type"), 250)
            .setColumnName("DISHNAME", tr("Dish"), 300)
            .setColumnName("PRICE", tr("Price"), 80)
            .setColumnName("STORENAME", tr("Store"), 150)
            .setColumnName("PRINT1", tr("Print 1"), 100)
            .setColumnName("PRINT2", tr("Print 2"), 100);
    fGrid->setQuery("select mm.id, mn.name as menuname, mt.name as typename, "
                    "md.name as dishname, mm.price, s.name as storename, mm.print1, mm.print2 "
                    "from me_dishes_menu mm "
                    "left join me_dishes md on md.id=mm.dish_id "
                    "left join me_types mt on mt.id=md.type_id "
                    "left join me_menus mn on mn.id=mm.menu_id "
                    "left join st_storages s on s.id=mm.store_id "
                    );
    fGrid->feMasterTableName = "me_dishes_menu";
    fGrid->fMasterTableKey[3] = "dish_id";
    fGrid->fSlaveTableName[3] = "me_dishes";
    fGrid->fSlaveTableKey[3] = "ID";
    fGrid->fSlaveTableValue[3] = "name";
}

NMenu::~NMenu()
{
    delete fGrid;
}
