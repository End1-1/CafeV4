#include "dlggetdoublevalue.h"
#include "ui_dlggetdoublevalue.h"
#include "qsqldrv.h"

DlgGetDoubleValue::DlgGetDoubleValue(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgGetDoubleValue)
{
    ui->setupUi(this);
}

DlgGetDoubleValue::~DlgGetDoubleValue()
{
    delete ui;
}

bool DlgGetDoubleValue::getDoubleValue(double &value, const QString &caption, QWidget *parent)
{
    DlgGetDoubleValue *d = new DlgGetDoubleValue(parent);
    d->ui->leValue->setValidator(new QDoubleValidator(0, 1000000, 2));
    d->ui->leValue->setText(dts(value));
    d->ui->lbCaption->setText(caption);
    d->ui->leValue->setSelection(0, d->ui->leValue->text().length());
    if (d->exec() == QDialog::Accepted) {
        value = d->ui->leValue->text().toDouble();
        delete d;
        return true;
    }
    delete d;
    return false;
}

void DlgGetDoubleValue::on_btnCancel_clicked()
{
    reject();
}

void DlgGetDoubleValue::on_btnOK_clicked()
{
    accept();
}
