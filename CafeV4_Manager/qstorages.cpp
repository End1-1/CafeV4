#include "qstorages.h"
#include "dlgstorages.h"

QStorages::QStorages(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "ST_STORAGES";
    m_actions << "actionNew" << "actionEdit" << "actionRefresh" << "actionExport_to_Excel";
    setWindowTitle(tr("Storages"));
    createSimpleIdNameTable("ST_STORAGES");
}

void QStorages::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgStorages>();
}

void QStorages::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgStorages>(false);
}
