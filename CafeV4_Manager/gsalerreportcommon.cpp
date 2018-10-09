#include "gsalerreportcommon.h"

GSalerReportCommon::GSalerReportCommon(QWidget *parent) :
    QGrid(parent)
{
    m_actions << "actionExport_to_Excel" << "actionRefresh" << "actionFilter"
              << "actionPrintPreview" << "actionPrint";

    setWindowTitle(tr("Common sales report"));

    SqlField *fDate1 = new SqlField("DATE1", tr("Date"), 150, 0, QVariant::Int, 0, false);
    fDate1->addFilter(Filter::ftDate, false, "");
    fDate1->m_filter->listValues = QDate::currentDate().toString(DATE_FORMAT);
    m_fields.append(fDate1);

    SqlField *fDate2 = new SqlField("DATE2", tr("Date"), 150, 0, QVariant::Int, 0, false);
    fDate2->addFilter(Filter::ftDate, false, "");
    fDate2->m_filter->listValues = QDate::currentDate().toString(DATE_FORMAT);
    fDate2->setInclude(false);
    m_fields.append(fDate2);


    SqlField *fName = new SqlField("STAFF_NAME", tr("Staff name"), 250, 0, QVariant::String, 0, false);
    m_fields.append(fName);

    SqlField *fQty = new SqlField("QTY", tr("Qty"), 150, 0, QVariant::Double, 0, false);
    m_fields.append(fQty);

    SqlField *fAmount = new SqlField("AMOUNT", tr("Amount"), 150, 0, QVariant::Double, 0, false);
    m_fields.append(fAmount);

    SqlField *fSign = new SqlField("SIGN", tr("Signature"), 250, 0, QVariant::String, 0, false);
    m_fields.append(fSign);

    m_totalRowTitles << fieldByName("QTY")->title << fieldByName("AMOUNT")->title;

    actionBeforeSelect();
}

void GSalerReportCommon::actionBeforeSelect()
{
    SqlField *fDate1 = fieldByName("DATE1");
    SqlField *fDate2 = fieldByName("DATE2");
    m_sqlString = "select o.date_cash as date1, e.fname || ' ' || e.lname as staff_name, "
            "count(o.id) as qty, sum(amount) as amount, ' ' as sign "
            "from o_order o, employes e "
            "where o.staff_id=e.id and o.state_id=2 "
            "and date_cash between '" + fDate1->m_filter->listValues + "' and '" + fDate2->m_filter->listValues + "'"
            "group by 1, 2 "
            "order by 1, 2 ";

    m_totalRowTitles << fieldByName("QTY")->title << fieldByName("AMOUNT")->title;

    actionSelect();
}
