#include "cfilterdialog.h"
#include "ui_cfilterdialog.h"

CFilterDialog::CFilterDialog(const QString &title, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CFilterDialog)
{
    ui->setupUi(this);
    setWindowTitle(title);
}

CFilterDialog::~CFilterDialog()
{
    delete ui;
}

bool CFilterDialog::isEmpty()
{
    return ui->tblData->rowCount() == 0;
}

void CFilterDialog::clearValues()
{
    ui->tblData->clearContents();
    ui->tblData->setRowCount(0);
}

void CFilterDialog::setFilterValues(const QList<QStringList> &values)
{
    if (values.count() == 0) {
        return;
    }
    int rowCount = values.at(0).count();
    int columnCount = values.count() + 1;
    ui->tblData->setColumnCount(columnCount);
    ui->tblData->setRowCount(rowCount);
    ui->tblData->setColumnWidth(0, 30);
    if (columnCount == 2) {
        ui->tblData->setColumnWidth(1, 300);
    }
    for (int i = 0; i < rowCount; i++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setCheckState(Qt::Unchecked);
        ui->tblData->setItem(i, 0, item);
        for (int j = 1; j < columnCount; j++) {
            ui->tblData->setItem(i, j, new QTableWidgetItem(values.at(j - 1).at(i)));
        }
    }
}

bool CFilterDialog::getFilter()
{
    if (exec() == QDialog::Accepted) {
        return true;
    }
    return false;
}

int CFilterDialog::exec()
{
    ui->leSearch->setFocus();
    return QDialog::exec();
}

void CFilterDialog::on_tblData_cellDoubleClicked(int row, int column)
{
    if (row < 0 || column < 0) {
        return;
    }
    ui->tblData->item(row, 0)->setCheckState(Qt::Checked);
    getResult();
    accept();
}

void CFilterDialog::getResult()
{
    fResult.clear();
    for (int i = 0; i < ui->tblData->rowCount(); i++) {
        if (ui->tblData->item(i, 0)->checkState() == Qt::Checked) {
            fResult.append(ui->tblData->item(i, 1)->text());
        }
    }
}

void CFilterDialog::on_leSearch_textChanged(const QString &arg1)
{
    for (int row = 0, rowCount = ui->tblData->rowCount(); row < rowCount; row++) {
        ui->tblData->setRowHidden(row, true);
        for (int col = 1, colCount = ui->tblData->columnCount(); col < colCount; col++) {
            if (ui->tblData->item(row, col)->text().contains(arg1, Qt::CaseInsensitive)) {
                ui->tblData->setRowHidden(row, false);
            }
        }
    }
}

void CFilterDialog::on_btnUncheck_clicked()
{
    ui->leSearch->clear();
    for (int i = 0; i < ui->tblData->rowCount(); i++) {
        ui->tblData->item(i, 0)->setCheckState(Qt::Unchecked);
    }
}

void CFilterDialog::on_btnApply_clicked()
{
    getResult();
    accept();
}

void CFilterDialog::on_btnRefresh_clicked()
{

}
