#include "dlgemployee.h"
#include "ui_dlgemployee.h"
#include "core.h"

DlgEmployee::DlgEmployee(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgEmployee)
{
    ui->setupUi(this);
    QSettings s("Jazzve", "Cafe4");
    ui->comboBox->setIndexOfData(s.value("dlgemployee_combo", 0).toInt());
    ui->comboBox_2->setIndexOfData(s.value("dlgemployee_combo2", 0).toInt());
}

DlgEmployee::~DlgEmployee()
{
    delete ui;
}

void DlgEmployee::on_comboBox_2_currentIndexChanged(int index)
{
    QSettings s("Jazzve", "Cafe4");
    s.setValue("dlgemployee_combo2", ui->comboBox_2->itemData(index));
}

void DlgEmployee::on_comboBox_currentIndexChanged(int index)
{
    QSettings s("Jazzve", "Cafe4");
    s.setValue("dlgemployee_combo", ui->comboBox->itemData(index));
}
