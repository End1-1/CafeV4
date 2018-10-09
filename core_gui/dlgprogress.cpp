#include "dlgprogress.h"
#include "ui_dlgprogress.h"

DlgProgress::DlgProgress(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgProgress)
{
    ui->setupUi(this);
}

DlgProgress::~DlgProgress()
{
    delete ui;
}

void DlgProgress::show(const QString &message)
{
    ui->lbText->setText(message);
    QDialog::show();
}

void DlgProgress::slotHide()
{
    hide();
}
