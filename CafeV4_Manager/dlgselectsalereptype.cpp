#include "dlgselectsalereptype.h"
#include "ui_dlgselectsalereptype.h"

DlgSelectSaleRepType::DlgSelectSaleRepType(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgSelectSaleRepType)
{
    ui->setupUi(this);
}

DlgSelectSaleRepType::~DlgSelectSaleRepType()
{
    delete ui;
}

void DlgSelectSaleRepType::on_btNormalReport_clicked()
{
    reject();
}

void DlgSelectSaleRepType::on_btnTotalView_clicked()
{
    accept();
}
