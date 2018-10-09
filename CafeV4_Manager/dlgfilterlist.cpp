#include "dlgfilterlist.h"
#include "ui_dlgfilterlist.h"

DlgFilterList::DlgFilterList(const QStringList &names, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgFilterList)
{
    ui->setupUi(this);
    ui->lstNames->addItems(names);
    for (int i = 0, count = names.count(); i < count; i++)
        ui->lstNames->item(i)->setCheckState(Qt::Unchecked);

    singleSelection = false;
}

DlgFilterList::~DlgFilterList()
{
    delete ui;
}

QList<int> &DlgFilterList::getCheckedIndexes(QList<int> &currentList, const QStringList &names, QWidget *parent)
{
    DlgFilterList *dlgFilterList = new DlgFilterList(names, parent);
    for (int i = 0, count = dlgFilterList->ui->lstNames->count(); i < count; i++)
        dlgFilterList->ui->lstNames->item(i)->setCheckState(Qt::Unchecked);
    for (QList<int>::const_iterator it = currentList.begin(); it != currentList.end(); it++)
        dlgFilterList->ui->lstNames->item(*it)->setCheckState(Qt::Checked);
    if (dlgFilterList->exec() == QDialog::Accepted)
        dlgFilterList->checkedIndexes(currentList);

    delete dlgFilterList;
    return currentList;
}

void DlgFilterList::checkedIndexes(QList<int> &list)
{
    list.clear();
    for (int i = 0, count = ui->lstNames->count(); i < count; i++)
        if(ui->lstNames->item(i)->checkState() == Qt::Checked)
            list.append(i);
}

void DlgFilterList::setSingleSelection()
{
    ui->lstNames->setSelectionMode(QAbstractItemView::SingleSelection);
    singleSelection = true;
}

void DlgFilterList::on_btnCancel_clicked()
{
    reject();
}

void DlgFilterList::on_btnOk_clicked()
{
    accept();
}

void DlgFilterList::on_btnClear_clicked()
{
    for (int i = 0, count = ui->lstNames->count(); i < count; i++)
        ui->lstNames->item(i)->setCheckState(Qt::Unchecked);
}

void DlgFilterList::on_lstNames_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    ui->lstNames->item(index.row())->setCheckState(ui->lstNames->item(index.row())->checkState() == Qt::Unchecked ? Qt::Checked : Qt::Unchecked);
    if (!singleSelection)
        return;
    for (int i = 0, count = ui->lstNames->count(); i < count; i++)
        ui->lstNames->item(i)->setCheckState(Qt::Unchecked);
    ui->lstNames->item(index.row())->setCheckState(Qt::Checked);
}
