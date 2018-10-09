#include "dlgdishtype.h"
#include "ui_dlgdishtype.h"
#include "qsqlcache.h"

DlgDishType::DlgDishType(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgDishType)
{
    ui->setupUi(this);
}

DlgDishType::~DlgDishType()
{
    delete ui;
}

void DlgDishType::btnOK()
{
    QSqlCache::removeChache("ME_TYPES");
    QDlgQuery::btnOK();
}
