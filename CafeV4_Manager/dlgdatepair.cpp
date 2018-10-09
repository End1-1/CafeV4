#include "dlgdatepair.h"
#include "ui_dlgdatepair.h"

DlgDatePair::DlgDatePair(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDatePair)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit_2->setDate(QDate::currentDate());
}

DlgDatePair::~DlgDatePair()
{
    delete ui;
}

bool DlgDatePair::getDate(QDate &d1, QDate &d2, QWidget *parent)
{
    DlgDatePair *d = new DlgDatePair(parent);
    d->ui->dateEdit->setDate(d1);
    d->ui->dateEdit_2->setDate(d2);
    if (d->exec() == QDialog::Accepted) {
        d1 = d->ui->dateEdit->date();
        d2 = d->ui->dateEdit_2->date();
        delete d;
        return true;
    }
    delete d;
    return false;
}

void DlgDatePair::on_btnCancel_clicked()
{
    reject();
}

void DlgDatePair::on_btnOK_clicked()
{
    accept();
}
