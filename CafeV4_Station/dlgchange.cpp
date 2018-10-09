#include "dlgchange.h"
#include "ui_dlgchange.h"

DlgChange::DlgChange(const QString &amount, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgChange)
{
    ui->setupUi(this);
    ui->leAmount->setText(amount);
}

DlgChange::~DlgChange()
{
    delete ui;
}

void DlgChange::on_pushButton_clicked()
{
    accept();
}

void DlgChange::btnClick(QObject *o)
{
    QPushButton *b = qobject_cast<QPushButton*>(o);
    if (b) {
        QString str = b->text();
        if (str == ".") {
            if (!ui->leDisplay->text().contains("."))
                ui->leDisplay->setText(ui->leDisplay->text() + ".");
        } else
            ui->leDisplay->setText(ui->leDisplay->text() + str);
    }
}

void DlgChange::on_btn1_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn2_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn3_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn4_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn5_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn6_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn7_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn8_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn9_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btn0_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btnPoint_clicked()
{
    btnClick(sender());
}

void DlgChange::on_btnClear_clicked()
{
    ui->leDisplay->clear();
}

void DlgChange::on_leDisplay_textChanged(const QString &arg1)
{
    ui->leResult->setText(Toolkit::formatDouble(arg1.toDouble() - ui->leAmount->text().toDouble()));
}

void DlgChange::on_leDisplay_returnPressed()
{
    accept();
}
