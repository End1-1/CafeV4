#include "magicmenu.h"
#include "ui_magicmenu.h"
#include "core.h"
#include <QColorDialog>

MagicMenu::MagicMenu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MagicMenu)
{
    ui->setupUi(this);

    m_sqlDrv =  new QSqlDrv(___ff_user->fullName, "clientdb");
    m_sqlDrv->execSQL("select id, name from me_menus");
    while (m_sqlDrv->m_query->next()) {
        QListWidgetItem *i = new QListWidgetItem(ui->lstMenu);
        i->setText(m_sqlDrv->valStr("NAME"));
        i->setData(Qt::UserRole, m_sqlDrv->valInt("ID"));
        i->setFlags(i->flags() | Qt::ItemIsUserCheckable);
        i->setCheckState(Qt::Unchecked);
        ui->lstMenu->addItem(i);
    }
    m_sqlDrv->close();
    ui->cbState->addItem(tr("Active"), 1);
    ui->cbState->addItem(tr("Inactive"), 0);
}

MagicMenu::~MagicMenu()
{
    delete m_sqlDrv;
    delete ui;
}

QMap<QString, QVariant> MagicMenu::values()
{
    QMap<QString, QVariant> values;
    values["applyInSelectedMenu"] = ui->chApplyInSelectedMenu->isChecked();
    values["deleteFromMenu"] = ui->chDeleteFromMenu->isChecked();
    values["color"] = ui->leColor->text().toInt();
    values["editColor"] = ui->chEditColor->isChecked();
    values["editStore"] = ui->chEditStore->isChecked();
    values["store"] = ui->cbStore->currentItemData();
    values["editPrintSchema"] = ui->chEditPrintSchema->isChecked();
    values["printSchema"] = ui->cbPrintSchema->currentItemData();
    values["chEditRemind"] = ui->chEditRemind->isChecked();
    values["remind"] = static_cast<int>(ui->chRemind->isChecked());
    values["chEditPayment"] = ui->chEditPayment->isChecked();
    values["payment"] = ui->cbPaymentMod->currentItemData();
    values["chState"] = ui->chState->isChecked();
    values["state"] = ui->cbState->currentData();

    QString menuList = "";
    bool first = true;
    for (int i = 0; i < ui->lstMenu->count(); i++) {
        QListWidgetItem *item = ui->lstMenu->item(i);
        if (item->checkState() == Qt::Checked) {
            if (first)
                first = false;
            else
                menuList += ",";
            menuList += QString::number(item->data(Qt::UserRole).toInt());
        }
    }
    values["menuList"] = menuList;
    return values;
}

void MagicMenu::on_pushButton_clicked()
{
    QColor c = QColorDialog::getColor(QColor::fromRgb(ui->leColor->text().toInt()));
    QPalette p(ui->leColor->palette());
    p.setColor(ui->leColor->backgroundRole(), c);
    ui->leColor->setPalette(p);
    ui->leColor->setText(QString::number((int)c.rgb()));
}

void MagicMenu::on_Cancel_clicked()
{
    reject();
}

void MagicMenu::on_btnOK_clicked()
{
    accept();
}
