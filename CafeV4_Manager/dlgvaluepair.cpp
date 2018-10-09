#include "dlgvaluepair.h"
#include "ui_dlgvaluepair.h"

DlgValuePair::DlgValuePair(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgValuePair)
{
    ui->setupUi(this);
}

DlgValuePair::~DlgValuePair()
{
    delete ui;
}

bool DlgValuePair::getValuePair(QString &v1, QString &v2, QWidget *parent)
{
    bool result = false;
    DlgValuePair *d = new DlgValuePair(parent);
    d->ui->leAS->setText(v1);
    d->ui->leCafe4->setText(v2);
    if (d->exec() == QDialog::Accepted) {
        v1 = d->ui->leAS->text();
        v2 = d->ui->leCafe4->text();
        result = true;
    }
    delete d;
    return result;
}
