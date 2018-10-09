#include "dlglist.h"
#include "ui_dlglist.h"

DlgList::DlgList(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgList)
{
    QFont font(qApp->font());
    font.setPointSize(20);
    setFont(font);
    ui->setupUi(this);
}

DlgList::~DlgList()
{
    delete ui;
}

void DlgList::setMenu(const QStringList &menu)
{
    ui->listWidget->addItems(menu);
}

QString DlgList::selectedItem()
{
    return ui->listWidget->selectedItems().at(0)->data(Qt::DisplayRole).toString();
}

void DlgList::on_btnCancel_clicked()
{
    reject();
}

void DlgList::on_btnOk_clicked()
{
    if (ui->listWidget->selectedItems().count()) {
        accept();
    }
}
