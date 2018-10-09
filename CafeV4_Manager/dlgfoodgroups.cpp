#include "dlgfoodgroups.h"
#include "ui_dlgfoodgroups.h"
#include "qsqlcache.h"

DlgFoodGroups::DlgFoodGroups(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgFoodGroups)
{
    ui->setupUi(this);
}

DlgFoodGroups::~DlgFoodGroups()
{
    delete ui;
}

void DlgFoodGroups::btnOk()
{
    QSqlCache::removeChache("FOOD_GROUPS");
    QDlgQuery::btnOK();
}
