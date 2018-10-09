#include "qmenunames.h"
#include "dlgmenuname.h"
#include "dlgmenucopy.h"

QMenuNames::QMenuNames(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "ME_MENUS";
    m_actions << "actionNew" << "actionDelete" << "actionEdit" << "actionRefresh" << "actionExport_to_Excel" << "actionCopy";
    setWindowTitle(tr("Menu names"));

    SqlField *fId = new SqlField("ID", tr("Id"), 100, 0, QVariant::Int, 0, false);
    m_fields.append(fId);

    SqlField *fName = new SqlField("NAME", tr("Name"), 300, 0, QVariant::String, 0, false);
    m_fields.append(fName);

    actionBeforeSelect();
}

void QMenuNames::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgMenuName>();
}

void QMenuNames::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgMenuName>(false);
}

void QMenuNames::actionDelete()
{
    CHECK_VIEWER_AND_MAINDB
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    m_sqlDriver->prepare("delete from me_dishes_menu where menu_id=:menu_id");
    m_sqlDriver->bind(":menu_id", cellValue(l.toList().at(0), 0));
    m_sqlDriver->execSQL();
    m_sqlDriver->prepare("delete from me_menus where id=:id");
    m_sqlDriver->bind(":id", cellValue(l.toList().at(0), 0));
    m_sqlDriver->execSQL();
    m_sqlDriver->close();

    removeRow(l.toList().at(0));
}

void QMenuNames::actionBeforeSelect()
{
    m_sqlString = "select id, name from me_menus order by name";
    actionSelect();
}

void QMenuNames::actionCostum(int action)
{
    CHECK_VIEWER_AND_MAINDB
    switch (action) {
    case ACTION_COPY:
        copyMenu();
        break;
    }
}

void QMenuNames::copyMenu()
{
    DlgMenuCopy *d = new DlgMenuCopy(this);
    d->exec();
    delete d;
}
