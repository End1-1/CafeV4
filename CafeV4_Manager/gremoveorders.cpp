#include "gremoveorders.h"
#include "dlgorder.h"

gremoveorders::gremoveorders(QWidget *parent) :
    QGrid(parent)
{
    m_actions << "actionRefresh" << "actionFilter" << "actionExport_to_Excel" << "actionEdit" << "actionSearch";
    setWindowTitle(tr("Removed orders"));

    QStringList colCaptions;
    colCaptions << tr("Date") << tr("Order") << tr("Dish") << tr("Qty");
    QList<int> colWidths;
    colWidths << 150 << 100 << 250 << 100;
    setCaptions(colCaptions, colWidths);

    SqlField *fActionId = addField("ACTION_ID", tr("Action id"), 0, QVariant::Int);
    fActionId->addFilter(Filter::ftEdit, true, "O_IMPORTANT_ACTIONS");
    fActionId->m_filter->listValues = m_settings->value("gremoveorders_action_id").toString();
    addField("ACTION_NAME", tr("Action"), 150, QVariant::String);

    SqlField *fDate1 = new SqlField("DATE1", tr("Date start"), 140, 0, QVariant::Date, 0, false);
    fDate1->addFilter(Filter::ftDate, false, "");
    fDate1->m_filter->listValues = m_settings->value("gremoveorders_date1", QDate::currentDate().toString(DATE_FORMAT)).toString();
    m_fields.append(fDate1);

    SqlField *fDate2 = new SqlField("DATE2", tr("Date end"), 0, 0, QVariant::Date, 0, false);
    fDate2->addFilter(Filter::ftDate, false, "");
    fDate2->m_filter->listValues = m_settings->value("gremoveorders_date2", QDate::currentDate().toString(DATE_FORMAT)).toString();
    m_fields.append(fDate2);

    addField("ORDER_ID", tr("Order id"), 100, QVariant::String);
    addField("USER_ID", tr("User id"), 0, QVariant::Int);
    addField("STAFF", tr("Staff"), 300, QVariant::String);
    addField("DATA", tr("Data"), 600, QVariant::String);
    addField("REASON_NAME", tr("Reason"), 300, QVariant::String);

    actionBeforeSelect();
}

void gremoveorders::actionBeforeSelect()
{
    /* Select removed order */
    int row = addRow();
    setCellValue(row, 0, tr("Removed rows"));

    SqlField *fDate1 = fieldByName("DATE1");
    SqlField *fDate2 = fieldByName("DATE2");
    QString where = QString(" i.log_date between '%1' and '%2' and").arg(fDate1->m_filter->listValues).arg(fDate2->m_filter->listValues);
    SqlField *fActionId = fieldByName("ACTION_ID");
    fActionId->haveFilter(where, "i.action_id");

    m_sqlString = "select i.action_id, ia.name as action_name, i.log_date as date1, '' as date2, i.order_id, i.user_id, e.fname || ' ' || e.lname as staff, i.data, "
            "r.name as reason_name "
            "from o_important i "
            "left join o_important_actions ia on i.action_id=ia.id "
            "left join employes e on i.user_id=e.id "
            "left join o_remove_reason r on i.reason_id=r.id "
            "where  " + where;
    remove_and(m_sqlString);
    actionSelect();
}

bool gremoveorders::actionFilter()
{
    if (QBaseGrid::actionFilter()) {
        m_settings->setValue("gremoveorders_date1", fieldByName("DATE1")->m_filter->listValues);
        m_settings->setValue("gremoveorders_date2", fieldByName("DATE2")->m_filter->listValues);
        m_settings->setValue("gremoveorders_action_id", fieldByName("ACTION_ID")->m_filter->listValues);
    }
    return true;
}

void gremoveorders::actionEdit()
{
    SqlField *f = fieldByName("ORDER_ID");
    if (!f->isIncluded()) {
        QMessageBox::warning(this, tr("Warning"), tr("The id field must be included in the query"));
        return;
    }

    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    auto *d = ___mainWindow->createWindow<DlgOrder>(nullptr);
    d->setId(cellValue(l.toList().at(0), 4).toString());
}

int gremoveorders::addRow()
{
    QTableWidget *t = tableWidget(0);
    int row = t->rowCount();
    t->setRowCount(row + 1);
    for (int i = 0; i < t->columnCount(); i++)
        t->setItem(row, i, new QTableWidgetItem());
    return row;
}
