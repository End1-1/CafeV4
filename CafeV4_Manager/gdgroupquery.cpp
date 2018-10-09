#include "gdgroupquery.h"
#include "ui_gdgroupquery.h"

GDGroupQuery::GDGroupQuery(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::GDGroupQuery)
{
    m_tableName = "GROUP_QUERY";
    ui->setupUi(this);
}

GDGroupQuery::~GDGroupQuery()
{
    delete ui;
}
