#include "dlgaddsalaryfunction.h"
#include "ui_dlgaddsalaryfunction.h"
#include "dlgsalaryformula.h"

DlgAddSalaryFunction::DlgAddSalaryFunction(const QMap<int, Function> &functions, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAddSalaryFunction),
    m_functions(functions)
{
    ui->setupUi(this);
    for (QMap<int, Function>::const_iterator i = m_functions.begin(); i != m_functions.end(); i++)
        ui->cbFunction->addItem(i.value().name, i.key());
    ui->cbFunction->setCurrentIndex(-1);
}

DlgAddSalaryFunction::~DlgAddSalaryFunction()
{
    delete ui;
}

void DlgAddSalaryFunction::setData(const Function &f)
{
    ui->cbFunction->setCurrentIndex(ui->cbFunction->findData(f.id));
    ui->txtDescription->setText(f.desc);
    ui->leParams->setText(f.params);
    ui->leCell->setText(f.cell);
}

void DlgAddSalaryFunction::getData(Function &f)
{
    int index = ui->cbFunction->currentIndex();
    if (index < 0)
        return;
    int id = ui->cbFunction->itemData(index).toInt();
    const Function &fs = m_functions[id];

    f = fs;
    f.params = ui->leParams->text();
    f.cell = ui->leCell->text();
}

void DlgAddSalaryFunction::on_btnCancel_clicked()
{
    reject();
}

void DlgAddSalaryFunction::on_btnOk_clicked()
{
    if (ui->cbFunction->currentIndex() < 0)
        return;
    accept();
}

void DlgAddSalaryFunction::on_cbFunction_currentIndexChanged(int index)
{
    ui->txtDescription->clear();
    ui->leCell->clear();
    ui->leParams->clear();
    if (index < 0)
        return;
    int id = ui->cbFunction->itemData(index).toInt();
    const Function &f = m_functions[id];
    ui->txtDescription->setText(f.desc);
}
