#include "stafffood.h"
#include "ui_stafffood.h"
#include "core.h"
#include <QLineEdit>

StaffFood::StaffFood(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::StaffFood)
{
    ui->setupUi(this);
    m_actions << "actionSave";
    ui->tbl1->setColumnWidth(0, 0);
    ui->tbl1->setColumnWidth(1, 200);
    ui->tbl1->setColumnWidth(2, 200);
    ui->tbl1->setColumnWidth(3, 80);
    ui->docDate->setDate(QDate::currentDate());
    m_sqlDrv->prepare("select d.id, t.name as typename, d.name as dishname "
                        "from me_dishes d "
                        "left join me_types t on t.id=d.type_id "
                        "where d.type_id=:type_id");
    m_sqlDrv->bind(":type_id", 170);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        int r = ui->tbl1->rowCount();
        ui->tbl1->setRowCount(r + 1);
        ui->tbl1->setItem(r, 0, new QTableWidgetItem(m_sqlDrv->valStr("ID")));
        ui->tbl1->setItem(r, 1, new QTableWidgetItem(m_sqlDrv->valStr("TYPENAME")));
        ui->tbl1->setItem(r, 2, new QTableWidgetItem(m_sqlDrv->valStr("DISHNAME")));
        ui->tbl1->setCellWidget(r, 3, new QLineEdit());
        QLineEdit *e = static_cast<QLineEdit*>(ui->tbl1->cellWidget(r, 3));
        e->setValidator(new QDoubleValidator(0, 99999, 2));
        e->setFrame(false);
        connect(e, &QLineEdit::textChanged, this, &StaffFood::qtyTextChanged);
    }
    ui->tbl2->setColumnWidth(0, 0);
    ui->tbl2->setColumnWidth(1, 0);
    ui->tbl2->setColumnWidth(2, 200);
    ui->tbl2->setColumnWidth(3, 200);
    ui->tbl2->setColumnWidth(4, 80);
    ui->tbl2->setColumnWidth(5, 80);
    m_sqlDrv->prepare("select r.dish_id, r.goods_id, r.qty, d.name as dishname, f.name as foodname "
                    "from me_recipes r "
                    "left join me_dishes d on d.id=r.dish_id "
                    "left join food_names f on f.id=r.goods_id "
                    "where d.type_id=:type_id "
                    "order by d.id ");
    m_sqlDrv->bind(":type_id", 170);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        int r = ui->tbl2->rowCount();
        ui->tbl2->setRowCount(r + 1);
        ui->tbl2->setItem(r, 0, new QTableWidgetItem(m_sqlDrv->valStr("GOODS_ID")));
        ui->tbl2->setItem(r, 1, new QTableWidgetItem(m_sqlDrv->valStr("DISH_ID")));
        ui->tbl2->setItem(r, 2, new QTableWidgetItem(m_sqlDrv->valStr("DISHNAME")));
        ui->tbl2->setItem(r, 3, new QTableWidgetItem(m_sqlDrv->valStr("FOODNAME")));
        ui->tbl2->setItem(r, 4, new QTableWidgetItem(m_sqlDrv->valStr("QTY")));
        ui->tbl2->setItem(r, 5, new QTableWidgetItem("0"));
    }
}

StaffFood::~StaffFood()
{
    delete ui;
}

void StaffFood::actionSave()
{
    if (ui->leDocNum->text().toInt() == 0) {
        ui->leDocNum->setText(its(m_sqlDrv->genId("GEN_ST_DOCUMENTS_ID")));
    }
    if (ui->leDocOutput->text().toInt() == 0) {
        ui->leDocOutput->setText(its(m_sqlDrv->genId("GEN_ST_DOCUMENTS_ID")));
    }

    m_sqlDrv->prepare("delete from st_documents where id=:id");
    m_sqlDrv->bind(":id", ui->leDocNum->text().toInt());
    m_sqlDrv->execSQL();
    m_sqlDrv->prepare("delete from st_documents where id=:id");
    m_sqlDrv->bind(":id", ui->leDocOutput->text().toInt());
    m_sqlDrv->execSQL();

    m_sqlDrv->prepare("insert into st_documents (id, state_id, action_id, doc_date, "
                      "payment_method, store_input, store_output, editor_id,"
                      "admited_id, passed_id, provider_id, amount, comment) "
                      "values (:id, :state_id, :action_id, :doc_date,"
                      ":payment_method, :store_input, :store_output, :editor_id,"
                      ":admited_id,:passed_id,:provider_id,:amount,:comment)");
    m_sqlDrv->bind(":id", ui->leDocNum->text().toInt());
    m_sqlDrv->bind(":state_id", 1);
    m_sqlDrv->bind(":action_id", 9);
    m_sqlDrv->bind(":doc_date", ui->docDate->date());
    m_sqlDrv->bind(":payment_method", 0);
    m_sqlDrv->bind(":store_input", 0);
    m_sqlDrv->bind(":store_output", 3);
    m_sqlDrv->bind(":editor_id", ___ff_user->id);
    m_sqlDrv->bind(":admited_id", ___ff_user->id);
    m_sqlDrv->bind(":passed_id", ___ff_user->id);
    m_sqlDrv->bind(":provider_id", 0);
    m_sqlDrv->bind(":amount", 0);
    m_sqlDrv->bind(":comment", tr("Staff launch"));
    m_sqlDrv->execSQL();

    m_sqlDrv->prepare("insert into st_documents (id, state_id, action_id, doc_date, "
                      "payment_method, store_input, store_output, editor_id,"
                      "admited_id, passed_id, provider_id, amount, comment) "
                      "values (:id, :state_id, :action_id, :doc_date,"
                      ":payment_method, :store_input, :store_output, :editor_id,"
                      ":admited_id,:passed_id,:provider_id,:amount,:comment)");
    m_sqlDrv->bind(":id", ui->leDocOutput->text().toInt());
    m_sqlDrv->bind(":state_id", 1);
    m_sqlDrv->bind(":action_id", 2);
    m_sqlDrv->bind(":doc_date", ui->docDate->date());
    m_sqlDrv->bind(":payment_method", 0);
    m_sqlDrv->bind(":store_input", 0);
    m_sqlDrv->bind(":store_output", 3);
    m_sqlDrv->bind(":editor_id", ___ff_user->id);
    m_sqlDrv->bind(":admited_id", ___ff_user->id);
    m_sqlDrv->bind(":passed_id", ___ff_user->id);
    m_sqlDrv->bind(":provider_id", 0);
    m_sqlDrv->bind(":amount", 0);
    m_sqlDrv->bind(":comment", QString("%1 %2").arg(tr("Staff launch"), ui->leDocNum->text()));
    m_sqlDrv->execSQL();

    m_sqlDrv->prepare("delete from st_draft where doc_id=:doc_id");
    m_sqlDrv->bind(":doc_id", ui->leDocOutput->text().toInt());
    m_sqlDrv->execSQL();
    m_sqlDrv->prepare("delete from st_stafflaunch where doc_id=:doc_id");
    m_sqlDrv->bind(":doc_id", ui->leDocNum->text().toInt());
    m_sqlDrv->execSQL();
    m_sqlDrv->prepare("delete from st_stafflaunch2 where doc_id=:doc_id");
    m_sqlDrv->bind(":doc_id", ui->leDocNum->text().toInt());
    m_sqlDrv->execSQL();
    for (int i = 0; i < ui->tbl2->rowCount(); i++) {
        if (ui->tbl2->item(i, 5)->text().toDouble() < 0.001) {
            continue;
        }
        m_sqlDrv->prepare("insert into st_draft (doc_id, goods_id, qty, amount) "
                          "values (:doc_id, :goods_id, :qty, :amount)");
        m_sqlDrv->bind(":doc_id", ui->leDocOutput->text().toInt());
        m_sqlDrv->bind(":goods_id", ui->tbl2->item(i, 0)->text().toInt());
        m_sqlDrv->bind(":qty", ui->tbl2->item(i, 5)->text().toDouble());
        m_sqlDrv->bind(":amount", 0);
        m_sqlDrv->execSQL();

        m_sqlDrv->prepare("insert into st_stafflaunch (doc_id, dish_id, food_id, base_qty, total_qty)"
                            "values (:doc_id, :dish_id, :food_id, :base_qty, :total_qty)");
        m_sqlDrv->bind(":doc_id", ui->leDocNum->text().toInt());
        m_sqlDrv->bind(":dish_id", ui->tbl2->item(i, 1)->text().toInt());
        m_sqlDrv->bind(":food_id", ui->tbl2->item(i, 0)->text().toInt());
        m_sqlDrv->bind(":base_qty", ui->tbl2->item(i, 4)->text().toDouble());
        m_sqlDrv->bind(":total_qty", ui->tbl2->item(i, 5)->text().toDouble());
        m_sqlDrv->execSQL();
    }

    for (int i = 0; i < ui->tbl1->rowCount(); i++) {
        if (static_cast<QLineEdit*>(ui->tbl1->cellWidget(i, 3))->text().toDouble() < 0.001) {
            continue;
        }
        m_sqlDrv->prepare("insert into st_stafflaunch2 (doc_id, dish_id, qty) "
                          "values (:doc_id, :dish_id, :qty)");
        m_sqlDrv->bind(":doc_id", ui->leDocNum->text().toInt());
        m_sqlDrv->bind(":dish_id", ui->tbl1->item(i, 0)->text().toInt());
        m_sqlDrv->bind(":qty", static_cast<QLineEdit*>(ui->tbl1->cellWidget(i, 3))->text().toDouble());
        m_sqlDrv->execSQL();
    }

    m_sqlDrv->close();
    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void StaffFood::qtyTextChanged(const QString &v)
{
    QLineEdit *e = static_cast<QLineEdit*>(sender());
    int r = -1;
    for (int i = 0; i < ui->tbl1->rowCount(); i++) {
        if (e == ui->tbl1->cellWidget(i, 3)) {
            r = i;
            break;
        }
    }
    if (r < 0) {
        return;
    }
    for (int i = 0; i < ui->tbl2->rowCount(); i++) {
        if (ui->tbl2->item(i, 1)->text() == ui->tbl1->item(r, 0)->text()){
            ui->tbl2->setItem(i, 5, new QTableWidgetItem(QString::number(v.toDouble() * ui->tbl2->item(i, 4)->text().toDouble())));
        }
    }
}
