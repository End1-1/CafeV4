
#include "qfoodunits.h"
#include "dlggoodsunits.h"

QFoodUnits::QFoodUnits(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "FOOD_UNITS";
    m_actions << "actionNew" << "actionEdit" << "actionRefresh";
    setWindowTitle(tr("Goods units"));
    createSimpleIdNameTable("FOOD_UNITS");
}

void QFoodUnits::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgGoodsUnits>();
}

void QFoodUnits::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgGoodsUnits>(false);
}
