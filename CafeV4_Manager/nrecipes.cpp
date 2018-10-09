#include "nrecipes.h"
#include "cgridwidget.h"
#include "qbasesqlwindow.h"

nrecipes::nrecipes(QWidget *parent) :
    MdiWindow(parent)
{
    showMaximized();
    fMdiButton = new MdiButton(this, tr("Recipes"));
    QBaseSqlWindow::m_mdiArea->addSubWindow(this);
    setToolbar(QBaseSqlWindow::m_toolBarBtn);
    fGrid = new CGridWidget("6bfced70-bf3d-441e-9189-0fe25470abf8");
    setCentralWidget(fGrid);
    fGrid->setColumnName("DISH_ID", tr("Code"), 80)
            .setColumnName("MENUNAME", tr("Menu"), 120)
            .setColumnName("TYPENAME", tr("Type"), 250)
            .setColumnName("DISHNAME", tr("Dish"), 300)
            .setColumnName("STORENAME", tr("Store"), 150)
            .setColumnName("foodname", tr("Goods"), 200)
            .setColumnName("QTY", tr("Qty"), 100);
    fGrid->setQuery("select mm.dish_id , mn.name as menuname, tn.name as typename, dn.name as dishname, \
                    s.name as storename, fn.name as foodname, r.qty \
                    from me_dishes_menu mm \
                    left join st_storages s on s.id=mm.store_id \
                    left join me_recipes r on r.dish_id=mm.dish_Id \
                    left join me_dishes dn on dn.id=mm.dish_id \
                    left join me_types tn on tn.id=dn.type_id \
                    left join me_menus mn on mn.id=mm.menu_id \
                    left join food_names fn on fn.id=r.goods_id \
                    order by 1, 2, 3 "
                    );
}
