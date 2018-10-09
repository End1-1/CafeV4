#include "qhall.h"
#include "dlghall.h"

QHall::QHall(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "H_HALL";
    m_actions << "actionNew" << "actionEdit" << "actionRefresh";
    setWindowTitle(tr("Halls"));
    createSimpleIdNameTable("h_hall");
}

void QHall::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgHall>();
}

void QHall::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgHall>();
}
