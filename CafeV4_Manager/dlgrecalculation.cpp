#include "dlgrecalculation.h"
#include "ui_dlgrecalculation.h"
#include "core.h"

DlgRecalculation::DlgRecalculation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgRecalculation)
{
    ui->setupUi(this);
    QDate d(QDate::currentDate().addMonths(-1));
    ui->deStart->setDate(d.addDays(-1 * (d.day() - 1)));
    ui->deEnd->setDate(d.addDays(d.daysInMonth() - d.day()));
    m_sqlDrv = new QSqlDrv(___ff_user->fullName, "clientdb");
}

DlgRecalculation::~DlgRecalculation()
{
    delete ui;
}

void DlgRecalculation::on_btnStart_clicked()
{
    start();
}

void DlgRecalculation::start()
{
    ui->lbProgress->setText(tr("Get output of recipes"));
    qApp->processEvents();
    /* Count output of recipes */
    m_sqlDrv->prepare("select o.store_id, r.goods_id, sum(r.qty*o.qty) as qty "
                     "from o_dishes o, me_recipes r, o_order oo  "
                     "where o.dish_id=r.dish_id and oo.id=o.order_id  "
                     " and o.state_id=1 and oo.state_id=2 "
                     " and oo.date_cash between :date1 and :date2 "
                     "group by 1, 2 "
                     "order by 1 ");
    m_sqlDrv->bind(":date1", ui->deStart->date());
    m_sqlDrv->bind(":date2", ui->deEnd->date());
    m_sqlDrv->execSQL();

    QStringList sqlQueries;
    int currStore = 0, currDoc = m_sqlDrv->genId("GEN_ST_DOCUMENTS_ID");
    while (m_sqlDrv->next()) {
        int store = m_sqlDrv->val().toInt();
        int goods = m_sqlDrv->val().toInt();
        float qty = m_sqlDrv->val().toFloat();

        if (!currStore)
            currStore = store;

        if (currStore != store) {
            sqlQueries.append(QString("insert into st_documents (id, action_id, state_id, payment_method, doc_date,"
                                      "store_input, store_output, editor_id, passed_id, admited_id, provider_id,"
                                      "amount, comment) values (%1, 4, 1, 1, '%2', 0, %3, %4, %4, %4, 0, 0, '')")
                              .arg(currDoc).arg(ui->deEnd->date().toString(DATE_FORMAT)).arg(currStore)
                              .arg(___ff_user->id));
            currStore = store;
            currDoc = m_sqlDrv->genId("GEN_ST_DOCUMENTS_ID");
        }

        sqlQueries.append(QString("insert into st_draft (doc_id, goods_id, qty, amount) "
                          "values (%1, %2, %3, 0)")
                          .arg(currDoc).arg(goods).arg(qty));
        sqlQueries.append(QString("insert into st_store (doc_date, doc_id, store_id, goods_id, qty, amount, op) "
                        "values ('%1', %2, %3, %4, %5, 0, -1)")
                          .arg(ui->deEnd->date().toString(DATE_FORMAT))
                          .arg(currDoc).arg(currStore).arg(goods).arg(qty));
    }
    if (currStore) {
        sqlQueries.append(QString("insert into st_documents (id, action_id, state_id, payment_method, doc_date,"
                                  "store_input, store_output, editor_id, passed_id, admited_id, provider_id,"
                                  "amount, comment) values (%1, 4, 1, 1, '%2', 0, %3, %4, %4, %4, 0, 0, '')")
                          .arg(currDoc).arg(ui->deEnd->date().toString(DATE_FORMAT)).arg(currStore)
                          .arg(___ff_user->id));
    }
    m_sqlDrv->close();

    ui->lbProgress->setText(tr("Insert result"));
    qApp->processEvents();

    m_sqlDrv->m_db.transaction();
    sqlQueries.append(QString("delete from st_documents where doc_date between '%1' and '%2' and action_id=4")
                      .arg(ui->deStart->date().toString(DATE_FORMAT))
                      .arg(ui->deEnd->date().toString(DATE_FORMAT)));
    std::reverse(sqlQueries.begin(), sqlQueries.end());
    for (QStringList::const_iterator it = sqlQueries.begin(); it != sqlQueries.end(); it++)
        m_sqlDrv->execSQL(*it);
    m_sqlDrv->close();    
    /* End of count output of recipes */


    /* Calculate prices */
    ui->lbProgress->setText(tr("Get prices from last snapshot of storages"));
    qApp->processEvents();
    QMap<int, QMap<int, Goods> > pricesMap;
    QDate dateSnapshot = ui->deStart->date().addDays(-1);
    m_sqlDrv->prepare("select store_id, goods_id, sum(sdd.amount), sum(qty) "
                      "from st_inv_documents sd, st_inv_draft sdd "
                      "where sd.id=sdd.doc_id and sd.doc_date=:doc_date "
                      "group by 1, 2 "
                      "order by 1, 2 ");
    m_sqlDrv->bind(":doc_date", dateSnapshot);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        int store = m_sqlDrv->val().toInt();
        int goods = m_sqlDrv->val().toInt();
        if (!pricesMap.contains(store))
            pricesMap[store] = QMap<int, Goods>();
        Goods g;
        g.price = m_sqlDrv->val().toFloat();
        g.qty = m_sqlDrv->val().toFloat();
        pricesMap[store][goods] = g;
    }
    ui->lbProgress->setText(tr("Get prices from store inputs"));
    qApp->processEvents();
    m_sqlDrv->prepare("select store_input, goods_id, sum(sdd.amount), sum(qty) "
                      "from st_documents sd, st_draft sdd "
                      "where sd.id=sdd.doc_id and sd.doc_date between :date1 and :date2 "
                      "and sd.action_id in (1, 5) "
                      "group by 1, 2 "
                      "order by 1, 2 ");
    m_sqlDrv->bind(":date1", ui->deStart->date());
    m_sqlDrv->bind(":date2", ui->deEnd->date());
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        int store = m_sqlDrv->val().toInt();
        int goods = m_sqlDrv->val().toInt();
        if (!pricesMap.contains(store))
            pricesMap[store] = QMap<int, Goods>();
        if (pricesMap[store].contains(goods)) {
            Goods &g = pricesMap[store][goods];
            g.price += m_sqlDrv->val().toFloat();
            g.qty += m_sqlDrv->val().toFloat();
        } else {
            Goods g;
            g.price = m_sqlDrv->val().toFloat();
            g.qty = m_sqlDrv->val().toFloat();
            pricesMap[store][goods] = g;
        }
    }
    m_sqlDrv->close();
    ui->lbProgress->setText(tr("Counting average prices"));
    qApp->processEvents();
    for (QMap<int, QMap<int, Goods> >::iterator s = pricesMap.begin(); s != pricesMap.end(); s++)
        for (QMap<int, Goods>::iterator g = s.value().begin(); g != s.value().end(); g++) {
            if (g.value().qty != 0)
                g.value().price /= g.value().qty;
            else
                g.value().price = 0;
        }

    ui->lbProgress->setText(tr("Updating output prices"));
    qApp->processEvents();
    m_sqlDrv->prepare("update st_draft set amount = :price * qty where goods_id=:goods_id and doc_id in ("
                      "select id from st_documents where doc_date between :date1 and :date2 "
                      "and action_id in (2, 4, 6))");
    for (QMap<int, QMap<int, Goods> >::const_iterator s = pricesMap.begin(); s != pricesMap.end(); s++) {
        int i = 1, count = s.value().count();
        for (QMap<int, Goods>::const_iterator g = s.value().begin(); g != s.value().end(); g++) {
            m_sqlDrv->bind(":price", g.value().price);
            m_sqlDrv->bind(":goods_id", g.key());
            m_sqlDrv->bind(":date1", ui->deStart->date());
            m_sqlDrv->bind(":date2", ui->deEnd->date());
            m_sqlDrv->execSQL();
            ui->lbProgress->setText(QString("%1: %2, %3/%4").arg(tr("Store")).arg(s.key()).arg(i++).arg(count));
        }
    }
    /* End of calculate prices */

    ui->lbProgress->setText(tr("Idle"));
    QMessageBox::information(this, "", tr("Done"));
}

void DlgRecalculation::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Escape:
    case Qt::Key_Return:
        return;
    }
    QDialog::keyPressEvent(e);
}

