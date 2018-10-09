#include "ggroupqueries.h"
#include "gdgroupquery.h"

GGroupQueries::GGroupQueries(QWidget *parent) :
    QGrid(parent)
{
    setWindowTitle(tr("Group queries"));
    m_editDlgTable = "GROUP_QUERY";
    m_actions << "actionNew" << "actionEdit" << "actionDelete" << "actionRefresh" << "actionExport_to_Excel";
    addField("ID", tr("Id"), 80, QVariant::Int);
    addField("TITLE", tr("Title"), 200, QVariant::String);
    addField("SQLTEXT", tr("Query"), 300, QVariant::String);
    addField("PARAMS", tr("Parameters"), 300, QVariant::String);
    actionBeforeSelect();
}

void GGroupQueries::actionBeforeSelect()
{
    m_sqlString = "select id, title, sqltext, params from group_query";
    actionSelect();
}

void GGroupQueries::actionNew()
{
    createEditorDlg<GDGroupQuery>();
}

void GGroupQueries::actionEdit()
{
    createEditorDlg<GDGroupQuery>(false);
}
