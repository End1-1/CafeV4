#include "qdishremovereason.h"
#include "dlgdishremovereason.h"

QDishRemoveReason::QDishRemoveReason(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "O_REMOVE_REASON";
    m_actions << "actionNew" << "actionEdit" << "actionExport_to_Excel" << "actionRefresh" << "actionSearch";
    setWindowTitle(tr("Dishes remove reason"));
    createSimpleIdNameTable("O_REMOVE_REASON");
}

void QDishRemoveReason::actionNew()
{
    createEditorDlg<DlgDishRemoveReason>();
}

void QDishRemoveReason::actionEdit()
{
    createEditorDlg<DlgDishRemoveReason>(false);
}
