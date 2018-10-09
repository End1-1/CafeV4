#include "qfoodgroup.h"
#include "dlgfoodgroups.h"

QFoodGroup::QFoodGroup(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "FOOD_GROUPS";
    m_actions << "actionNew" << "actionEdit" << "actionRefresh";
    setWindowTitle(tr("Goods groups"));
    createSimpleIdNameTable("FOOD_GROUPS");
}

void QFoodGroup::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgFoodGroups>();
}

void QFoodGroup::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgFoodGroups>(false);
}
