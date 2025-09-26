#include "monthlyselfcost.h"

MonthlySelfcost::MonthlySelfcost() {
    m_actions <<  "actionRefresh"
              << "actionFilter"
              << "actionPrint"
              << "actionPrint_preview"
              << "actionExport_to_Excel"
              << "actionView_prices"
              << "actionSearch"
              << "actionSales_settings";

    setWindowTitle(tr("Goods movement"));
    QFont f(font());
    f.setPointSize(f.pointSize() - 2);
    setFont(f);

    QStringList captions;
    captions << tr("Goods code")
             << tr("Goods")
             << tr("Till")
             << tr("In")
             << tr("Movement in")
             << tr("Sale")
             << tr("Movement out")
             << tr("Out")
             << tr("Counted")
             << tr("Fact")
             << tr("Price") ;
    QList<int> widths;
    widths << 0 << 250
           << 65 << 65 << 65 << 65
           << 65 << 65 << 65 << 65
           << 65 << 65 << 65 ;
    setCaptions(captions, widths);
    setHeaderHeight(60);

    int i = 0;
    m_dataFields["goods_id"] = i++;
    m_dataFields["goods_name"] = i++;
    m_dataFields["q_till"] = i++;
    m_dataFields["q_in"] = i++;
    m_dataFields["q_move_in"] = i++;
    m_dataFields["q_sale_out"] = i++;
    m_dataFields["q_move_out"] = i++;
    m_dataFields["q_out"] = i++;
    m_dataFields["q_counted"] = i++;
    m_dataFields["q_fact"] = i++;
    m_dataFields["price"] = i++;

    QDate d = QDate::currentDate().addMonths(-1);

    SqlField *fDate1 = new SqlField("DATE1", tr("Date start"), 0, 0, QVariant::Date, 0, false);
    fDate1->addFilter(Filter::ftDate, false, "");
    fDate1->m_filter->listValues = QString("01.%1.%2").arg(d.month(), 2, 10, QChar('0')).arg(d.year());
    m_fields.append(fDate1);

    SqlField *fDate2 = new SqlField("DATE2", tr("Date end"), 0, 0, QVariant::Date, 0, false);
    fDate2->addFilter(Filter::ftDate, false, "");
    fDate2->m_filter->listValues = QString("%1.%2.%3").arg(d.daysInMonth()).arg(d.month(), 2, 10, QChar('0')).arg(d.year());
    m_fields.append(fDate2);

    SqlField *fStoreId = new SqlField("STORE_ID", tr("Store id"), 0, 0, QVariant::Int, 0, false);
    fStoreId->addFilter(Filter::ftEdit, true, "ST_STORAGES");
    m_fields.append(fStoreId);

    SqlField *fGoodsId = new SqlField("GOODS_ID", tr("Goods id"), 0, 0, QVariant::Int, 0, false);
    fGoodsId->addFilter(Filter::ftEdit, true, "FOOD_NAMES");
    m_fields.append(fGoodsId);

    tableWidget(0)->setItemDelegate(new ItemDelegate());
}

void MonthlySelfcost::actionBeforeSelect()
{
    SqlField *f = fieldByName("STORE_ID");
    QStringList stores = f->m_filter->listValues.split(",");
    QStringList storeNames = f->m_filter->listNames.split(",");
    if (!stores.count()) {
        QMessageBox::warning(this, tr("Warning"), tr("Store is not selected"));
        return;
    }

    for (int si = 0; si < stores.count(); si++) {
        QString goodsCond;
        f = fieldByName("GOODS_ID");
        if (f->m_filter->listValues.length())
            goodsCond = " and sd.goods_id in (" + f->m_filter->listValues + ") ";

        f = fieldByName("DATE1");
        QDate d1 = QDate::fromString(f->m_filter->listValues, DATE_FORMAT);
        f = fieldByName("DATE2");
        QDate d2 = QDate::fromString(f->m_filter->listValues, DATE_FORMAT);


        /* Get prices */
        m_sqlDriver->execSQL("select id, price from food_names");
        while (m_sqlDriver->next())
            m_prices[m_sqlDriver->val().toInt()] = m_sqlDriver->val().toFloat();

        m_goodsRows.clear();
        QTableWidget *t1 = tableWidget(0);
        t1->clearContents();
        t1->setSortingEnabled(false);
        t1->setRowCount(0);
        t1->setColumnCount(m_dataFields.count());
        setColCaption(1, QSystem::objectName() + "/" + storeNames.at(si) + "/"
                             + d1.toString(DATE_FORMAT) + "-" + d2.toString(DATE_FORMAT));

        /* Snapshots */
        m_sqlDriver->prepare("select sd.goods_id, fn.name, sum(sd.qty) as qty "
                             "from st_documents st, st_draft sd, food_names fn "
                             "where st.id=sd.doc_id and sd.goods_id=fn.id " + goodsCond +
                             "and st.doc_date = :date and st.store_input=:store_id "
                             "and st.action_id=:action_id "
                             "group by 1, 2");
        /* First storage snapshot */
        m_sqlDriver->bind(":date", d1.addDays(-1));
        m_sqlDriver->bind(":store_id", stores.at(si).toInt());
        m_sqlDriver->bind(":action_id", DOC_INVERNTORIZATION);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            fetchRow(t1, m_dataFields["q_till"]);
        /* Last  storage snapshot */
        m_sqlDriver->bind(":date", d2);
        m_sqlDriver->bind(":store_id", stores.at(si).toInt());
        m_sqlDriver->bind(":action_id", DOC_INVERNTORIZATION);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            fetchRow(t1, m_dataFields["q_fact"]);

        /* Input */
        m_sqlDriver->prepare("select sd.goods_id, fn.name, sum(sd.qty) as qty, sum(sd.amount) as amount "
                             "from st_documents st, st_draft sd, food_names fn "
                             "where st.id=sd.doc_id and sd.goods_id=fn.id " + goodsCond +
                             "and st.doc_date between :date1 and :date2 and store_input=:store_id "
                             "and st.action_id=:action_id "
                             "group by 1, 2");

        /* Input */
        m_sqlDriver->bind(":date1", d1);
        m_sqlDriver->bind(":date2", d2);
        m_sqlDriver->bind(":store_id", stores.at(si).toInt());
        m_sqlDriver->bind(":action_id", DOC_INPUT);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            fetchRow(t1, m_dataFields["q_in"]);

        /* Movement, in */
        m_sqlDriver->bind(":date1", d1);
        m_sqlDriver->bind(":date2", d2);
        m_sqlDriver->bind(":store_id", stores.at(si).toInt());
        m_sqlDriver->bind(":action_id", DOC_MOVEMENT);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            fetchRow(t1, m_dataFields["q_move_in"]);

        /* Output */
        m_sqlDriver->prepare("select sd.goods_id, fn.name, sum(sd.qty) as qty, sum(sd.amount) as amount "
                             "from st_documents st, st_draft sd, food_names fn "
                             "where st.id=sd.doc_id and sd.goods_id=fn.id " + goodsCond +
                             "and st.doc_date between :date1 and :date2 and store_output=:store_id "
                             "and st.action_id=:action_id and st.comment not like 'Անձնակազմի%'"
                             "group by 1, 2");

        /* Sale */
        m_sqlDriver->bind(":date1", d1);
        m_sqlDriver->bind(":date2", d2);
        m_sqlDriver->bind(":store_id", stores.at(si).toInt());
        m_sqlDriver->bind(":action_id", DOC_SALE);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            fetchRow(t1, m_dataFields["q_sale_out"], -1);
        /* Out */
        m_sqlDriver->bind(":date1", d1);
        m_sqlDriver->bind(":date2", d2);
        m_sqlDriver->bind(":store_id", stores.at(si).toInt());
        m_sqlDriver->bind(":action_id", DOC_OUTPUT);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            fetchRow(t1, m_dataFields["q_out"], -1);
        /* Movement, out */
        m_sqlDriver->bind(":date1", d1);
        m_sqlDriver->bind(":date2", d2);
        m_sqlDriver->bind(":store_id", stores.at(si).toInt());
        m_sqlDriver->bind(":action_id", DOC_MOVEMENT);
        m_sqlDriver->execSQL();
        while (m_sqlDriver->next())
            fetchRow(t1, m_dataFields["q_move_out"], -1);


        /* Get autoout goods list */
        QMap<int, double> autooutQty;
        QMap<int, double> autooutAmount;
        m_sqlDriver->execSQL("select id from food_names where auto_out=1");
        while (m_sqlDriver->next()) {
            int g = m_sqlDriver->val().toInt();
            autooutQty[g] = 0;
            autooutAmount[g] = 0;
        }
        bool autoout = false;

        /* And final */
        for (int i = 0; i < tableWidget(0)->rowCount(); i++) {
            int goodsId = cellValue(i, m_dataFields["goods_id"]).toInt();
            float qty = 0.0, amount = 0.0, avg = 0.0;
            qty += cellValue(i, m_dataFields["q_till"]).toFloat();
            amount += cellValue(i, m_dataFields["a_till"]).toFloat();
            qty += cellValue(i, m_dataFields["q_in"]).toFloat();
            amount += cellValue(i, m_dataFields["a_in"]).toFloat();
            qty += cellValue(i, m_dataFields["q_move_in"]).toFloat();
            amount += cellValue(i, m_dataFields["a_move_in"]).toFloat();

            if (qty > 0)
                avg = amount / qty;

            float pr = m_prices[cellValue(i, m_dataFields["goods_id"]).toInt()];
            if (pr < 0.1) {
                pr = avg;
            }

            if (cellValue(i, m_dataFields["a_fact"]).toFloat() < 1)
                setCellValue(i, m_dataFields["a_fact"],
                             cellValue(i, m_dataFields["q_fact"]).toFloat() * pr);

            qty += cellValue(i, m_dataFields["q_sale_out"]).toFloat();
            setCellValue(i, m_dataFields["a_sale_out"], avg * cellValue(i, m_dataFields["q_sale_out"]).toFloat());


            qty += cellValue(i, m_dataFields["q_out"]).toFloat();
            setCellValue(i, m_dataFields["a_out"], avg * cellValue(i, m_dataFields["q_out"]).toFloat());


            qty += cellValue(i, m_dataFields["q_move_out"]).toFloat();
            setCellValue(i, m_dataFields["a_move_out"], avg * cellValue(i, m_dataFields["q_move_out"]).toFloat());


            setCellValue(i, m_dataFields["q_counted"], qty);
            setCellValue(i, m_dataFields["a_counted"], avg * cellValue(i, m_dataFields["q_counted"]).toFloat());

            setCellValue(i, m_dataFields["q_diff"], (float) cellValue(i, m_dataFields["q_fact"]).toFloat() - qty);
            setCellValue(i, m_dataFields["a_diff"], (float) avg * cellValue(i, m_dataFields["q_diff"]).toFloat());

            /* Count autoout */
            if (autooutQty.contains(goodsId)) {
                if (cellValue(i, m_dataFields["q_diff"]).toDouble() < 0) {
                    autooutQty[goodsId] = cellValue(i, m_dataFields["q_diff"]).toDouble();
                    autooutAmount[goodsId] = avg * autooutQty[goodsId];
                    setCellValue(i, m_dataFields["q_sale_out"], cellValue(i, m_dataFields["q_sale_out"]).toDouble() + autooutQty[goodsId]);
                    setCellValue(i, m_dataFields["q_counted"], cellValue(i, m_dataFields["q_counted"]).toDouble() + autooutQty[goodsId]);
                    setCellValue(i, m_dataFields["q_diff"], 0);
                    autoout = true;
                }
            }

            setCellValue(i, m_dataFields["price"], m_prices[goodsId]);
        }

        for (int i = 0; i < tableWidget(0)->rowCount(); i++) {
            float correctZero = m_prices[cellValue(i, m_dataFields["goods_id"]).toInt()];

            if (cellValue(i, m_dataFields["q_diff"]).toFloat() > 0.0009)
                if (cellValue(i, m_dataFields["a_diff"]).toFloat() < 1)
                    setCellValue(i, m_dataFields["a_diff"], cellValue(i, m_dataFields["q_diff"]).toFloat() * correctZero);

            if (cellValue(i, m_dataFields["q_counted"]).toFloat() > 0.0009 || cellValue(i, m_dataFields["q_counted"]).toFloat() < -0.0009)
                if (abs(cellValue(i, m_dataFields["a_counted"]).toFloat()) < 1)
                    setCellValue(i, m_dataFields["a_counted"], cellValue(i, m_dataFields["q_counted"]).toFloat() * correctZero);

            if (cellValue(i, m_dataFields["q_sale_out"]).toFloat() < -0.0009)
                if (cellValue(i, m_dataFields["a_sale_out"]).toFloat() > -1)
                    setCellValue(i, m_dataFields["a_sale_out"], cellValue(i, m_dataFields["q_sale_out"]).toFloat() * correctZero);
        }

        /* Insert autoout */
        if (autoout) {
            int docId = m_sqlDriver->genId("GEN_ST_DOCUMENTS_ID");
            m_sqlDriver->m_db.transaction();
            m_sqlDriver->prepare("insert into st_documents (id, state_id, action_id, payment_method, doc_date, store_input, store_output,"
                                 "editor_id, passed_id, admited_id, provider_id, amount, comment) values "
                                 "(:id, :state_id, :action_id, :payment_method, :doc_date, :store_input, :store_output,"
                                 ":editor_id, :passed_id, :admited_id, :provider_id, :amount, :comment)");
            m_sqlDriver->bind(":id", docId);
            m_sqlDriver->bind(":state_id", 1);
            m_sqlDriver->bind(":action_id", DOC_SALE);
            m_sqlDriver->bind(":payment_method", 1);
            m_sqlDriver->bind(":doc_date", d2);
            m_sqlDriver->bind(":store_input", 0);
            m_sqlDriver->bind(":store_output", stores.at(si).toInt());
            m_sqlDriver->bind(":editor_id", 1);
            m_sqlDriver->bind(":passed_id", 1);
            m_sqlDriver->bind(":admited_id", 1);
            m_sqlDriver->bind(":provider_id", 0);
            m_sqlDriver->bind(":amount", 0);
            m_sqlDriver->bind(":comment", "AUTOOUT");
            m_sqlDriver->execSQL();

            m_sqlDriver->prepare("insert into st_draft (doc_id, goods_id, qty, amount) values (:doc_id, :goods_id, :qty, :amount)");
            for (QMap<int, double>::const_iterator it = autooutQty.begin(); it != autooutQty.end(); it++) {
                if (it.value() * 10 < -1) {
                    m_sqlDriver->bind(":doc_id", docId);
                    m_sqlDriver->bind(":goods_id", it.key());
                    m_sqlDriver->bind(":qty", it.value() * -1);
                    m_sqlDriver->bind(":amount", autooutAmount[it.key()] * -1);
                    m_sqlDriver->execSQL();
                }
            }
            m_sqlDriver->m_db.commit();
        }



        m_sqlDriver->close();

        t1->setSortingEnabled(true);
        t1->sortByColumn(m_dataFields["q_diff"], Qt::AscendingOrder);
        //t1->sortByColumn(m_dataFields["q_diff"]);

        QStringList vl;
        vl.append(QString::number(t1->rowCount()));
        tableWidget(1)->setVerticalHeaderLabels(vl);

        /* Count total */
        QTableWidget *t = tableWidget(0);
        for (int i = 2; i < t->columnCount(); i++) {
            float sum = 0;
            for (int j = 0; j < t->rowCount(); j++) {
                sum += t->item(j, i)->text().toDouble();
            }
            tableWidget(1)->setItem(0, i, new QTableWidgetItem(dts(sum)));
        }

        if (t1->rowCount() > 0)
            actionExcel(QString("%1\\selfcost.%2.%3.%4.xlsx")
                            .arg(QSystem::repDefaultPath())
                            .arg(QSystem::objectName())
                            .arg(QSystem::shortMonth(d1))
                            .arg(storeNames.at(si)));
    }
}

void MonthlySelfcost::fetchRow(QTableWidget *t, int col, int sign)
{
    int goodsId = m_sqlDriver->val().toInt();
    QString goodsName = m_sqlDriver->val().toString();
    double qty = m_sqlDriver->val().toFloat() * sign;
    double amount = m_sqlDriver->val().toFloat() * sign;

    if (!m_goodsRows.contains(goodsId))
        checkRow(t, goodsId, goodsName);
    int row = m_goodsRows[goodsId];
    t->item(row, col)->setData(Qt::DisplayRole, qty);
}

void MonthlySelfcost::checkRow(QTableWidget *t, int goodsId, const QString &goodsName)
{
    int newRow = t->rowCount();
    m_goodsRows[goodsId] = newRow;
    t->setRowCount(newRow + 1);
    for (int i = 2; i < t->columnCount(); i++)
        t->setItem(newRow, i, new DoubleTableWidgetItem(0.0));
    t->setItem(newRow, 0, new IntTableWidgetItem(goodsId));
    t->setItem(newRow, 1, new QTableWidgetItem(goodsName));
}

void MonthlySelfcost::ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    painter->save();

    QString text;
    if (index.column() > 1)
        text = QString::number(index.data(Qt::DisplayRole).toDouble(), 'f', 2);
    else
        text = index.data(Qt::DisplayRole).toString();

    QPen pen(Qt::NoPen);
    painter->setPen(pen);
    QBrush brush(Qt::SolidPattern);
    brush.setColor(Qt::white);
    if (option.state & QStyle::State_Selected) {
        brush.setColor(Qt::blue);
        pen.setColor(Qt::white);
    }
    painter->setBrush(brush);
    painter->drawRect(option.rect);

    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    QRect r = option.rect;
    r.adjust(4, 4, -3, -3);
    painter->drawText(r, text);

    painter->restore();
}
