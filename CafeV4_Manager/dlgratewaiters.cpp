#include "dlgratewaiters.h"
#include "ui_dlgratewaiters.h"

DlgRateWaiters::DlgRateWaiters(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRateWaiters)
{
    ui->setupUi(this);
    QList<int> colw;
    colw.append(300);
    colw.append(30);
    colw.append(30);
    colw.append(30);
    colw.append(30);
    colw.append(30);
    for (int i = 0; i < colw.count(); i++) {
        ui->tbl->setColumnWidth(i, colw.at(i));
    }
}

DlgRateWaiters::~DlgRateWaiters()
{
    delete ui;
}

void DlgRateWaiters::getRates(const QStringList &names, QList<int> &rates)
{
    DlgRateWaiters d;
    d.ui->tbl->setRowCount(names.count());
    d.disconnect(d.ui->tbl, SIGNAL(cellChanged(int,int)), &d, SLOT(on_tbl_cellChanged(int,int)));
    for (int i = 0; i < names.count(); i++) {
        d.ui->tbl->setItem(i, 0, new QTableWidgetItem(names.at(i)));
        for (int j = 1; j < d.ui->tbl->columnCount(); j++) {
            QTableWidgetItem *item = new QTableWidgetItem();
            item->setCheckState(Qt::Unchecked);
            d.ui->tbl->setItem(i, j, item);
        }
    }
    d.connect(d.ui->tbl, SIGNAL(cellChanged(int,int)), &d, SLOT(on_tbl_cellChanged(int,int)));
    bool ok = false;
    do {
        d.exec();
        for (int i = 0; i < d.ui->tbl->rowCount(); i++) {
            for (int j = 1; j < d.ui->tbl->columnCount(); j++) {
                if (d.ui->tbl->item(i, j)->checkState() == Qt::Checked) {
                    rates[i] = j;
                    break;
                }
            }
        }
        ok  = true;
        for (int r: rates) {
            if (r == 0) {
                ok = false;
                break;
            }
        }
    } while (!ok);
}

void DlgRateWaiters::on_tbl_cellChanged(int row, int column)
{
    disconnect(ui->tbl, SIGNAL(cellChanged(int,int)), this, SLOT(on_tbl_cellChanged(int,int)));
    if (column > 0) {
        for (int i = 1; i < ui->tbl->columnCount(); i++) {
            ui->tbl->item(row, i)->setCheckState(Qt::Unchecked);
        }
        ui->tbl->item(row, column)->setCheckState(Qt::Checked);
    }
    connect(ui->tbl, SIGNAL(cellChanged(int,int)), this, SLOT(on_tbl_cellChanged(int,int)));
}
