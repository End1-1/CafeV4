#include "dlgmenucopy.h"
#include "ui_dlgmenucopy.h"
#include "qsqldrv.h"
#include "qbasesqlwindow.h"

DlgMenuCopy::DlgMenuCopy(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgMenuCopy)
{
    ui->setupUi(this);
}

DlgMenuCopy::~DlgMenuCopy()
{
    delete ui;
}

void DlgMenuCopy::on_btnClose_clicked()
{
    accept();
}

void DlgMenuCopy::on_btnStart_clicked()
{
    if (ui->cbFrom->currentItemData() == ui->cbTo->currentItemData()) {
        QMessageBox::warning(this, tr("Warning"), tr("Cannot copy itself to itself"));
        return;
    }

    QSqlDrv *qname = new QSqlDrv(QBaseSqlWindow::m_username, "clientdb");
    QSqlDrv *temp = new QSqlDrv(QBaseSqlWindow::m_username, "clientdb1");
    QSqlDrv *ins = new QSqlDrv(QBaseSqlWindow::m_username, "clientdb2");
    if (!qname->openDB()) {
        QMessageBox::critical(this, tr("Sql error"), tr("Cannot connect to database"));
        return;
    }
    if (!temp->openDB()) {
        QMessageBox::critical(this, tr("Sql error"), tr("Cannot connect to database"));
        return;
    }
    if (!ins->openDB()) {
        QMessageBox::critical(this, tr("Sql error"), tr("Cannot connect to database"));
        return;
    }
    //GetNames
    qname->prepare("select * from me_dishes where id in (select dish_id from me_dishes_menu where menu_id=:menu_id and state_id=1)");
    qname->bind(":menu_id", ui->cbFrom->currentItemData());
    qname->execSQL();
    QMap<QString, QVariant> bindValues;
    //ins->m_db.transaction();
    ui->progressBar->setMaximum(qname->m_query->size());
    while (qname->next()) {
        //name
        bindValues.clear();
        for (int i = 0;i < qname->m_query->record().count(); i++) {
            bindValues[qname->m_query->record().fieldName(i)] = qname->m_query->value(i);
        }
        int oldid = bindValues["ID"].toInt();
        int newid = temp->genId("gen_me_dishes_id");
        bindValues["ID"] = newid;
        ins->insert("me_dishes", bindValues);

        //menu
        temp->prepare("select * from me_dishes_menu where dish_id=:dish_id and menu_id=:menu_id");
        temp->bind(":dish_id", oldid);
        temp->bind(":menu_id", ui->cbFrom->currentItemData());
        temp->execSQL();
        while (temp->next()) {
            bindValues.clear();
            for (int i = 0;i < temp->m_query->record().count(); i++) {
                bindValues[temp->m_query->record().fieldName(i)] = temp->m_query->value(i);
            }
            bindValues.remove("ID");
            bindValues["DISH_ID"] = newid;
            bindValues["MENU_ID"] = ui->cbTo->currentItemData();
            ins->insert("me_dishes_menu", bindValues);
        }

        //recipe
        temp->prepare("select * from me_recipes where dish_id=:dish_id");
        temp->bind(":dish_id", oldid);
        temp->execSQL();
        while (temp->next()) {
            bindValues.clear();
            for (int i = 0;i < temp->m_query->record().count(); i++) {
                bindValues[temp->m_query->record().fieldName(i)] = temp->m_query->value(i);
            }
            bindValues.remove("ID");
            bindValues["DISH_ID"] = newid;
            ins->insert("me_recipes", bindValues);
        }
        ui->progressBar->setValue(ui->progressBar->value() + 1);
    }
    //ins->m_db.commit();
    QMessageBox::information(this, "Copy", "Menu copied");
}
