#include "gsalarydoc.h"
#include "dlgsalarydoc.h"

GSalaryDoc::GSalaryDoc(QWidget *parent) :
    QGrid(parent)
{
    m_actions << "actionNew" << "actionEdit" << "actionDelete" << "actionFilter"
              << "actionRefresh" << "actionExport_to_Excel" << "actionPrint" << "actionPrint_preview";

    setWindowTitle(tr("Salary documents"));

    SqlField *fDocId = new SqlField("ID", tr("Id"), 100, 0, QVariant::Int, 0, false);
    m_fields.append(fDocId);

    SqlField *fDate1 = new SqlField("DATE1", tr("Date"), 120, 0, QVariant::Date, 0, false);
    fDate1->addFilter(Filter::ftDate, false, "");
    fDate1->m_filter->listValues = QDate::currentDate().addDays(-1 * (QDate::currentDate().day() - 1)).toString(DATE_FORMAT);
    m_fields.append(fDate1);

    SqlField *fDate2 = new SqlField("DATE2", tr("Date"), 0, 0, QVariant::Date, 0, false);
    fDate2->addFilter(Filter::ftDate, false, "");
    fDate2->m_filter->listValues = QDate::currentDate().toString(DATE_FORMAT);
    fDate2->setInclude(false);
    m_fields.append(fDate2);

    SqlField *fRecId = new SqlField("REC_ID", tr("Row id"), 0, 0, QVariant::Int, 0, false);
    m_fields.append(fRecId);

    SqlField *fGroupId = new SqlField("ROLE_ID", tr("Group id"), 0, 0, QVariant::Int, 0, false);
    fGroupId->addFilter(Filter::ftEdit, true, "EMPLOYES_GROUP");
    m_fields.append(fGroupId);

    SqlField *fGroupName = new SqlField("GROUP_NAME", tr("Group"), 200, 0, QVariant::String, 0, false);
    m_fields.append(fGroupName);

    SqlField *fEmployeeId = new SqlField("EMPLOYEE_ID", tr("Employee id"), 0, 0, QVariant::Int, 0, false);
    fEmployeeId->addFilter(Filter::ftEdit, true, "EMPLOYES_ID_NAME");
    fEmployeeId->m_filter->m_showCheckBox = true;
    fEmployeeId->setInclude(false);
    fRecId->setIncludeRef(fEmployeeId);
    fGroupId->setIncludeRef(fEmployeeId);
    fGroupName->setIncludeRef(fEmployeeId);
    m_fields.append(fEmployeeId);

    SqlField *fEmployeeName = new SqlField("EMPLOYEE_NAME", tr("Employee"), 250, 0, QVariant::String, 0, false);
    fEmployeeName->setIncludeRef(fEmployeeId);
    m_fields.append(fEmployeeName);

    SqlField *fAmount = new SqlField("AMOUNT", tr("Amount"), 150, 0, QVariant::Double, 0, false);
    fAmount->setInclude(true);
    m_fields.append(fAmount);

    SqlField *fHc = new SqlField("HC", tr("As"), 50, 0, QVariant::Int, 0, false);
    //fHc->setInclude(true);
    m_fields.append(fHc);

    fGroupId->setIncludeRef(fEmployeeId);

    actionBeforeSelect();

}

void GSalaryDoc::actionNew()
{
    if (___ff_user->id != 1)
        CHECK_VIEWER
    DlgSalaryDoc *d = new DlgSalaryDoc("0", this);
    d->show();
}

void GSalaryDoc::actionEdit()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    DlgSalaryDoc *d = new DlgSalaryDoc(cellValue(l.toList().at(0), 0).toString(), this);
    d->show();
}

void GSalaryDoc::actionDelete()
{
    CHECK_VIEWER
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    for (int i = l.count() - 1; i > -1; i--) {
        m_sqlDrv->prepare("delete from salary_doc where id=:id");
        m_sqlDrv->bind(":id", cellValue(l.toList().at(i), 0).toInt());
        m_sqlDrv->execSQL();
        tableWidget(0)->removeRow(l.toList().at(i));
    }
    m_sqlDrv->close();
}

void GSalaryDoc::actionBeforeSelect()
{
    SqlField *fDate1 = fieldByName("DATE1");
    SqlField *fDate2 = fieldByName("DATE2");

    QString select = "select sd.id, sd.doc_date as date1,", group = " group by sd.id,sd.doc_date,",
            from = " from salary_doc sd,salary_body sb,",
            where = " where sd.doc_date between '" + fDate1->m_filter->listValues + "' and '" + fDate2->m_filter->listValues
            + "' and sb.doc_id=sd.id and";

    SqlField *fGroup = fieldByName("ROLE_ID");
    if (fGroup->isIncluded()) {
        select += "sb.id as rec_id,sb.role_id,eg.name as group_name,sb.employee_id,e.fname || ' ' || e.lname as employee_name,";
        group += "sb.id,sb.role_id,eg.name,sb.employee_id,e.fname || ' ' || e.lname,";
    }

    if (fGroup->m_filter->listValues.length())
        where += " sb.role_id in (" + fGroup->m_filter->listValues + ") and";

    if (fGroup->isIncluded()) {
        fGroup->sqlAddFrom(from, where, "employes e", "sb.employee_id=e.id");
        fGroup->sqlAddFrom(from, where, "employes_group eg", "sb.role_id=eg.id");
        SqlField *fName = fieldByName("EMPLOYEE_ID");
        if (fName->m_filter->listValues.length())
            where += " e.id in (" + fName->m_filter->listValues + ") and";
    }

    select += "sum(sb.amount) as amount, sd.hc,";
    group += "sd.hc,";

    remove_zap(select);
    remove_zap(from);
    remove_zap(group);
    remove_and(where);

    m_totalRowTitles.clear();
    m_totalRowTitles << tr("Amount");
    m_sqlString = select + from + where + group;

    actionSelect();
}
