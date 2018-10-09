#include "dlgtables.h"
#include "ui_dlgtables.h"

DlgTables::DlgTables(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgTables)
{
    ui->setupUi(this);

    showMaximized();
    QMap<QString, QString> filter;
    filter.insert(TABLE_HALL_NAME, ___settings->toString(SETTINGS_HALL_ID));
    ___hall.simpleTable(ui->tblTable);

}

DlgTables::~DlgTables()
{
    delete ui;
}

bool DlgTables::getTableId(QWidget *parent, Hall2::Table &table)
{

    DlgTables *d = new DlgTables(parent);
    if (d->exec() == QDialog::Accepted) {
        QModelIndexList list = d->ui->tblTable->selectionModel()->selectedIndexes();
        if (!list.count())
            return false;
        int tableIndex = list.at(0).data(Qt::UserRole).toInt() - 1;
        if (___hall.getTable(tableIndex, table))
            return true;
    }
    delete d;

    return false;
}

void DlgTables::on_btnCancel_clicked()
{
    reject();
}

void DlgTables::on_tblTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous)
    ui->btnOk->setEnabled(current && current->data(Qt::UserRole).toString().length());
}

void DlgTables::on_btnOk_clicked()
{
    accept();
}

void DlgTables::on_tblTable_clicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        ui->btnOk->setEnabled(false);
        return;
    }

    if (!index.data(Qt::UserRole).toInt()) {
        ui->btnOk->setEnabled(false);
        return;
    }

    ui->btnOk->setEnabled(true);
}
