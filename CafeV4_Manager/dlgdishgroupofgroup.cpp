#include "dlgdishgroupofgroup.h"
#include "ui_dlgdishgroupofgroup.h"

DlgDishGroupOfGroup::DlgDishGroupOfGroup(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgDishGroupOfGroup)
{
    ui->setupUi(this);
}

DlgDishGroupOfGroup::~DlgDishGroupOfGroup()
{
    delete ui;
}
