#include "imenuitem.h"
#include "ui_imenuitem.h"
#include <QCheckBox>

IMenuItem::IMenuItem(QWidget *parent) :
    GridEditorWidget(parent),
    ui(new Ui::IMenuItem)
{
    ui->setupUi(this);
    ui->tblMenu->setColumnWidth(0, 30);
    ui->tblMenu->setColumnWidth(1, 150);
    ui->tblMenu->setColumnWidth(2, 80);
    ui->tblMenu->setColumnWidth(3, 150);
    ui->tblMenu->setColumnWidth(4, 100);
    ui->tblMenu->setColumnWidth(5, 100);
    ui->tblMenu->setColumnWidth(6, 30);
    ui->tblMenu->setColumnWidth(7, 30);
}

IMenuItem::~IMenuItem()
{
    delete ui;
}

void IMenuItem::load(int id)
{
    SqlThread *st = new SqlThread(_gr_host_, _gr_path_, _gr_user_, _gr_pass_, "lc_ctype=utf8");
    connect(st, SIGNAL(finished()), this, SLOT(sqlFinished()));
    connect(st, SIGNAL(error(QString)), this, SLOT(saveError(QString)));
    st->bind(":id", id);
    st->setSqlQuery("select md.id, md.type_id, mt.name as type_name, md.color, md.queue, md.remind, "
                    "md.descr, md.name "
                    "from me_dishes md "
                    "inner join me_types mt on mt.id=md.type_id "
                    "where md.id=:id");

    st->start();
}


void IMenuItem::save()
{
    GridEditorWidget::save();
}

QString IMenuItem::title() const
{
    return tr("Menu item");
}

void IMenuItem::setTableName()
{
    mTableName = "me_dishes";
}

void IMenuItem::sqlFinished()
{
    GridEditorWidget::sqlFinished();
    SqlThread *st = new SqlThread(_gr_host_, _gr_path_, _gr_user_, _gr_pass_, "lc_ctype=utf8");
    connect(st, SIGNAL(finished()), this, SLOT(menuFinished()));
    connect(st, SIGNAL(error(QString)), this, SLOT(saveError(QString)));
    st->bind(":dish_id", mCodeLineEdit->text().toInt());
    st->setSqlQuery("select mdm.state_id, mm.name as menu_name, mdm.price, mdm.store_id, st.name as store_name, mdm.print1, mdm.print2 "
                    "from me_menus mm "
                    "inner join me_dishes_menu mdm on mdm.menu_id=mm.id "
                    "inner join st_storages st on st.id=mdm.store_id "
                    "where mdm.dish_id=:dish_id "
                    "order by mm.id ");

    st->start();
}

void IMenuItem::menuFinished()
{
    SqlThread *st = static_cast<SqlThread*>(sender());
    ui->tblMenu->setRowCount(st->rowCount());
    int row = 0;
    while (st->nextRow()) {
        ui->tblMenu->createCheckbox(row, 0)->setChecked(st->getInteger("state_id") == 1);
        ui->tblMenu->setString(row, 1, st->getString("menu_name"));
        ui->tblMenu->createLineEdit(row, 2)->setText(st->getString("price"));
        QLineEdit *l = ui->tblMenu->createLineEdit(row, 3);
        l->setText(st->getString("store_name"));
        l->setProperty("id", st->getInteger("store_id"));
        l = ui->tblMenu->createLineEdit(row, 4);
        l->setText(st->getString("print1"));
        l = ui->tblMenu->createLineEdit(row, 5);
        l->setText(st->getString("print2"));
        row++;
    }
    st->deleteLater();
    st = new SqlThread(_gr_host_, _gr_path_, _gr_user_, _gr_pass_, "lc_ctype=utf8");
    connect(st, SIGNAL(finished()), this, SLOT(recipeFinished()));
    connect(st, SIGNAL(error(QString)), this, SLOT(saveError(QString)));
    st->bind(":id", mCodeLineEdit->text().toInt());
    st->setSqlQuery("select md.id, md.type_id, mt.name as type_name, md.color, md.queue, md.remind, "
                    "md.descr, md.name "
                    "from me_dishes md "
                    "inner join me_types mt on mt.id=md.type_id "
                    "where md.id=:id");

    st->start();
}

void IMenuItem::recipeFinished()
{
    SqlThread *st = static_cast<SqlThread*>(sender());
    st->deleteLater();
}
