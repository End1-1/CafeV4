#include "qemployesgroups.h"
#include "gdemployesgroups.h"
#include "qdlgemployesaccess.h"
#include "dlgsalaryformula.h"

QEmployesGroups::QEmployesGroups(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "EMPLOYES_GROUP";
    setWindowTitle(tr("Empoyes groups"));
    m_actions << "actionNew" << "actionEdit" << "actionRefresh"
              << "actionExport_to_Excel" << "actionPrint" << "actionPrint_preview"
              << "actionView_prices" << "actionAccess_control";

    SqlField *fId = new SqlField("ID", tr("Id"), 100, 0, QVariant::Int, 0, false);
    m_fields.append(fId);

    SqlField *fName = new SqlField("NAME", tr("Name"), 300, 0, QVariant::String, 0, false);
    m_fields.append(fName);

    actionBeforeSelect();
}

void QEmployesGroups::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<GDEmployesGroups>();
}

void QEmployesGroups::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<GDEmployesGroups>(false);
}

void QEmployesGroups::actionBeforeSelect()
{
    m_sqlString = "select id, name from employes_group order by 2";
    actionSelect();
}

void QEmployesGroups::actionCostum(int action)
{
    switch(action) {
    case ACTION_ACCESS_CONTROL:
        accessControl();
        break;
    case ACTION_GROUP_SALARY:
        salary();
        break;
    }
}

void QEmployesGroups::accessControl()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }
    auto *d = ___mainWindow->createWindow<QDlgEmployesAccess>(nullptr);
    d->setGroup(cellValue(l.toList().at(0), 0).toString());
}

void QEmployesGroups::salary()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }
    DlgSalaryFormula *d = ___mainWindow->createWindow<DlgSalaryFormula>(nullptr);
    d->setPostId(cellValue(l.toList().at(0), 0).toString(), cellValue(l.toList().at(0), 1).toString());
}
