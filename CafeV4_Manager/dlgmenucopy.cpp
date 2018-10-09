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

    QSqlDrv *q = new QSqlDrv(QBaseSqlWindow::m_username, "clientdb");
    if (!q->openDB()) {
        QMessageBox::critical(this, tr("Sql error"), tr("Cannot connect to database"));
        return;
    }

    q->prepare("delete from me_dishes_menu where menu_id=:menu_id");
    q->bind(":menu_id", ui->cbTo->currentItemData());
    q->execSQL();
    q->prepare("select dish_id, price, store_id, print_schema from me_dishes_menu where menu_id=:menu_id and state_id=1");
    q->bind(":menu_id", ui->cbFrom->currentItemData());
    q->execSQL();
    QStringList sqlLines;
    while (q->next()) {
        sqlLines.append(QString("insert into me_dishes_menu (menu_id, dish_id, price, store_id, print_schema, state_id)"
                                " values (%1, %2, %3, %4, %5, 1)")
                        .arg(ui->cbTo->currentItemData())
                        .arg(q->val().toInt())
                        .arg(q->val().toFloat())
                        .arg(q->val().toInt())
                        .arg(q->val().toInt()));
    }

    int i = 0, c = sqlLines.count();
    ui->progressBar->setMaximum(c);
    for (QStringList::const_iterator it = sqlLines.begin(); it != sqlLines.end(); it++) {
        i++;
        ui->progressBar->setValue(i);
        q->execSQL(*it);
        qApp->processEvents();
    }
    q->close();
}
