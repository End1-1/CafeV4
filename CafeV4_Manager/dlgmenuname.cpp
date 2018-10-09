#include "dlgmenuname.h"
#include "ui_dlgmenuname.h"

DlgMenuName::DlgMenuName(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgMenuName)
{
    ui->setupUi(this);
}

DlgMenuName::~DlgMenuName()
{
    delete ui;
}
