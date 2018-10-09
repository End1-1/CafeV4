#include "gemployes.h"
#include "dlgemployee.h"
#include "dlgpassword.h"
#include <QCryptographicHash>

GEmployes::GEmployes(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "EMPLOYES";
    m_actions << "actionNew" << "actionEdit" << "actionRefresh" << "actionFilter"
              << "actionPrint_preview" << "actionPrint" << "actionExport_to_Excel"
              << "actionSet_password" << "actionClear_password" << "actionSearch";
    setWindowTitle(tr("Employes"));

    SqlField *fId = new SqlField("ID", tr("Id"), 100, 0, QVariant::Int, 0, false);
    m_fields.append(fId);

    SqlField *fGroupId = new SqlField("GROUP_ID", tr("Group id"), 0, 0, QVariant::Int, 0, false);
    fGroupId->addFilter(Filter::ftEdit, true, "EMPLOYES_GROUP");
    m_fields.append(fGroupId);

    SqlField *fGroupName = new SqlField("GROUP_NAME", tr("Position"), 200, 0, QVariant::String, 0, false);
    m_fields.append(fGroupName);

    SqlField *fStateId = new SqlField("STATE_ID", tr("State id"), 0, 0, QVariant::Int, 0, false);
    fStateId->addFilter(Filter::ftEdit, true, "EMPLOYEE_STATE");
    m_fields.append(fStateId);

    addField("STATE_NAME", tr("State"), 100, QVariant::String);

    SqlField *fFName = new SqlField("FNAME", tr("First name"), 200, 0, QVariant::String, 0, false);
    m_fields.append(fFName);

    SqlField *fLName = new SqlField("LNAME", tr("Last name"), 200, 0, QVariant::String, 0, false);
    m_fields.append(fLName);

    SqlField *fLogin = new SqlField("LOGIN", tr("Login"), 200, 0, QVariant::String, 0, false);
    m_fields.append(fLogin);

    actionBeforeSelect();
}

void GEmployes::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgEmployee>();
}

void GEmployes::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgEmployee>(false);
}

void GEmployes::actionCostum(int action)
{
    switch(action) {
    case ACTION_SET_PASSWORD:
        setPassword();
        break;
    case ACTION_CLEAR_PASSWORD:
        clearPassword();
        break;
    }
}

void GEmployes::actionBeforeSelect()
{
    m_sqlString = "select e.id, group_id, g.name as group_name, e.state_id, s.name as state_name, fname, lname, login "
            "from employes e, employes_group g, employee_state s "
            "where e.group_id=g.id and e.state_id=s.id ";
    SqlField *f = fieldByName("GROUP_ID");
    if (f->m_filter->listValues.length())
        m_sqlString += " and e.group_id in (" + f->m_filter->listValues + ")";
    f = fieldByName("STATE_ID");
    if (f->m_filter->listValues.length())
        m_sqlString += " and e.state_id in (" + f->m_filter->listValues + ")";
    actionSelect();
}

void GEmployes::setPassword()
{
    QModelIndexList l = selectedRows();
    if (!l.count())
        return;
    QString p1, p2; //P1 - login and password
    if (!DlgPassword::GetPasswords(p1, p2, this))
        return;
    if (p1.length()) {
        m_sqlDrv->prepare("update employes set password=:password where id=:id");
        m_sqlDrv->bind(":password", QString(QCryptographicHash(QCryptographicHash::Md5).hash(p1.toLatin1(), QCryptographicHash::Md5).toHex()));
        m_sqlDrv->bind(":id", cellValue(l.at(0).row(), 0));
        m_sqlDrv->execSQL();
    }
    if (p2.length()) {
        m_sqlDrv->prepare("update employes set password2=:password2 where id=:id");
        m_sqlDrv->bind(":password2", QString(QCryptographicHash(QCryptographicHash::Md5).hash(p2.toLatin1(), QCryptographicHash::Md5).toHex()));
        m_sqlDrv->bind(":id", cellValue(l.at(0).row(), 0));
        m_sqlDrv->execSQL();
    }
    m_sqlDrv->close();
}

void GEmployes::clearPassword()
{
    QModelIndexList l = selectedRows();
    if (!l.count())
        return;
    if (QMessageBox::warning(this, tr("Empty passwords"), tr("Are you shure to clear passwords for")
                             + cellValue(l.at(0).row(), 3).toString() + " "
                             + cellValue(l.at(0).row(), 4).toString()) != QMessageBox::Yes)
        return;

    m_sqlDriver->prepare("update employes set password=null, password2=null where id=:id");
    m_sqlDriver->bind(":id", cellValue(l.at(0).row(), 0));
    m_sqlDrv->execSQL();
    m_sqlDrv->close();

    QMessageBox::information(this, tr("Information"), tr("Done"));
}
