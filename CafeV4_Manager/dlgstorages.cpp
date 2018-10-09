#include "dlgstorages.h"
#include "ui_dlgstorages.h"

DlgStorages::DlgStorages(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgStorages)
{
    ui->setupUi(this);
}

DlgStorages::~DlgStorages()
{
    delete ui;
}
