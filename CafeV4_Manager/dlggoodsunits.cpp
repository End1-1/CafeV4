#include "dlggoodsunits.h"
#include "ui_dlggoodsunits.h"

DlgGoodsUnits::DlgGoodsUnits(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgGoodsUnits)
{
    ui->setupUi(this);
}

DlgGoodsUnits::~DlgGoodsUnits()
{
    delete ui;
}
