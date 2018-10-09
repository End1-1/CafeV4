#include "dlgorderremovereason.h"
#include "ui_dlgorderremovereason.h"

DlgOrderRemoveReason::DlgOrderRemoveReason(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgOrderRemoveReason)
{
    ui->setupUi(this);
}

DlgOrderRemoveReason::~DlgOrderRemoveReason()
{
    delete ui;
}

bool DlgOrderRemoveReason::reason(QString &reason, QWidget *parent)
{
    bool result = false;
    DlgOrderRemoveReason *d = new DlgOrderRemoveReason(parent);
    if (d->exec() == QDialog::Accepted) {
        reason = d->ui->leReason->text();
        result = true;
    }
    delete d;
    return result;
}

void DlgOrderRemoveReason::on_btnCancel_clicked()
{
    reject();
}

void DlgOrderRemoveReason::on_btnOk_clicked()
{
    if (ui->leReason->text().isEmpty()) {
        return;
    }
    accept();
}
