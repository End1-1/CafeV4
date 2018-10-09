#include "dlgcalculator.h"
#include "ui_dlgcalculator.h"

DlgCalculator::DlgCalculator(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgCalculator)
{
    ui->setupUi(this);
}

DlgCalculator::~DlgCalculator()
{
    delete ui;
}

void DlgCalculator::on_btnOk_clicked()
{
    accept();
}

void DlgCalculator::on_btnCancel_clicked()
{
    reject();
}

void DlgCalculator::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        ui->widget->calculate();
        return;
    case Qt::Key_Escape:
        ui->widget->clear();
        return;
    }
    QDialog::keyPressEvent(e);
}
