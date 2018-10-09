#include "dlgsettotalmovementaddoption.h"
#include "ui_dlgsettotalmovementaddoption.h"
#include "../../common/qsystem.h"

DlgSetTotalMovementAddOption::DlgSetTotalMovementAddOption(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSetTotalMovementAddOption)
{
    ui->setupUi(this);
}

DlgSetTotalMovementAddOption::~DlgSetTotalMovementAddOption()
{
    delete ui;
}

int DlgSetTotalMovementAddOption::getResult(QList<bool> &result, QWidget *parent)
{
    DlgSetTotalMovementAddOption *d = new DlgSetTotalMovementAddOption(parent);
    d->ui->chViewComplect->setChecked(result[0]);
    d->ui->chViewPrices->setChecked(result[1]);
    d->ui->chGetAllCafeReports->setChecked(result[2] && QSystem::isMain());
    d->ui->chGetAllCafeReports->setVisible(QSystem::isMain());
    int r = d->exec();
    result[0] = d->ui->chViewComplect->isChecked();
    result[1] = d->ui->chViewPrices->isChecked();
    result[2] = d->ui->chGetAllCafeReports->isChecked() && QSystem::isMain();
    delete d;
    return r;
}

void DlgSetTotalMovementAddOption::on_btnCancel_clicked()
{
    reject();
}

void DlgSetTotalMovementAddOption::on_btnOk_clicked()
{
    accept();
}
