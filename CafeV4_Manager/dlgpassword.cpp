#include "dlgpassword.h"
#include "ui_dlgpassword.h"

DlgPassword::DlgPassword(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgPassword)
{
    ui->setupUi(this);
}

DlgPassword::~DlgPassword()
{
    delete ui;
}

bool DlgPassword::GetPasswords(QString &p1, QString &p2, QWidget *parent)
{
    bool result = true;
    DlgPassword *d = new DlgPassword(parent);
    if (d->exec() == QDialog::Accepted) {
        p1 = d->ui->lePwd1->text();
        p2 = d->ui->lePwd2->text();
        result = true;
    }
    delete d;
    return result;
}

void DlgPassword::on_pushButton_clicked()
{
    reject();
}

void DlgPassword::on_pushButton_2_clicked()
{
    accept();
}
