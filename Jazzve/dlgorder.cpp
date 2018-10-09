#include "dlgorder.h"
#include "ui_dlgorder.h"
#include "dlgtables.h"
#include "dlgorderhistory.h"
#include "dlggetpassword.h"
#include "../core/basenetwork.h"

DlgOrder::DlgOrder(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::DlgOrder)
{
    ui->setupUi(this);

    m_flagClose = false;

    QFont font(qApp->font());
    font.setPointSize(10);
    font.setBold(true);

    ui->tableOrderInfo->setFont(font);
    ui->tableOrderInfo->verticalHeader()->hide();
    ui->tableOrderInfo->setRowHeight(0, 20);
    ui->tableOrderInfo->setColumnWidth(0, 120);
    ui->tableOrderInfo->setColumnWidth(1, 120);
    ui->tableOrderInfo->setColumnWidth(2, 220);

    ui->tbOrderDishes->setFont(qApp->font());
    ui->tbOrderDishes->setColumnWidth(0, 370);
    ui->tbOrderDishes->setColumnWidth(1, 80);

    connect(this, SIGNAL(qtyText(QString&)), SLOT(slotQtyText(QString&)));

    showFullScreen();
    qApp->processEvents();
    ___menu.configureGroupTable(ui->tbGroups, ___settings->toString(SETTINGS_MENU_NAME), "");
    ___menu.configureGroupOfGroupTable(ui->tblGroupOfGroup);
}

void DlgOrder::setMain(StationAuth *staff, const QString &tableId)
{
    m_staff = staff;
    m_order = new Order(m_staff->session());
    m_order->setOrderProperty(ORDER_CURRENT_STAFF_NAME, m_staff->fullName());
    m_order->setOrderProperty(ORDER_CURRENT_STAFF_ID, m_staff->id());
    m_order->setOrderProperty(PERMISSION_PRINT_CHECK_MORE_THEN_ONE, m_staff->getWritePermission(PERMISSION_PRINT_CHECK_MORE_THEN_ONE));
    m_order->setOrderProperty(PERMISSION_APPEND_DISH_AFTER_CHECKOUT, m_staff->getWritePermission(PERMISSION_APPEND_DISH_AFTER_CHECKOUT));
    m_order->setOrderProperty(PERMISSION_PAYMENT_ALL_ORDERS, m_staff->getWritePermission(PERMISSION_PAYMENT_ALL_ORDERS));
    Hall2::Table t;
    ___hall.getTable(tableId, t);
    m_order->setOrderProperty(ORDER_DEBT, t.toString(TABLE_RESERVED));
    connect(m_order, SIGNAL(stopped(int,QString)), SLOT(orderStopped(int,QString)));    

    m_order->setAutoModSrc(___mod);

    m_order->appendWidget("table", ui->tbOrderDishes);
    m_order->appendWidget("table_info", ui->tableOrderInfo);
    m_order->appendWidget("payment_display", ui->tblSum);
    m_order->appendWidget("btn_service_print", ui->btnPrintServiceCheck);
    m_order->appendWidget("btn_discount", ui->btnDiscount);
    m_order->appendWidget("btn_checkout_print", ui->btnCheckout);
    m_order->appendWidget("btn_payment", ui->btnPayment);
    m_order->appendWidget("btn_remove_order", ui->btnRemoveWholeOrder);
    m_order->appendWidget("btn_move_order", ui->btnMoveOrder);
    m_order->appendWidget("table_dishes", ui->tbDishes);
    m_order->setOrderProperty(ORDER_TABLE_TO_LOCK, tableId);
    m_order->setOrderProperty(ORDER_TABLE_TO_UNLOCK, tableId);
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->putVoid(SLOT(setAutoMod(int,QString)));
    m_order->newOrder(0, "");
    ui->tableOrderInfo->item(0, 2)->setData(Qt::DisplayRole, staff->fullName());

    //Enabled functions
    ui->btnRemoveWholeOrder->setVisible(m_staff->getWritePermission(PERMISSION_REMOVE_ORDER));
    ui->btnMoveOrder->setVisible(m_staff->getWritePermission(PERMISSION_MOVE_ORDER));
    ui->btnMoveDish->setVisible(m_staff->getWritePermission(PERMISSION_MOVE_DISH));
    m_order->setOrderProperty(PERMISSION_PRINT_CHECK_MORE_THEN_ONE, m_staff->getWritePermission(PERMISSION_PRINT_CHECK_MORE_THEN_ONE));
    m_order->setOrderProperty(PERMISSION_APPEND_DISH_AFTER_CHECKOUT, m_staff->getWritePermission(PERMISSION_APPEND_DISH_AFTER_CHECKOUT));
}

DlgOrder::~DlgOrder()
{
    delete ui;
}

void DlgOrder::on_btnExit_clicked()
{
    close();
}

void DlgOrder::on_btnMenuDown_clicked()
{
    ui->tbGroups->verticalScrollBar()->setValue(ui->tbGroups->verticalScrollBar()->value() + 5);
}

void DlgOrder::on_btnMenuUp_clicked()
{
    ui->tbGroups->verticalScrollBar()->setValue(ui->tbGroups->verticalScrollBar()->value() - 5);
}

void DlgOrder::on_tbGroups_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    if (!index.data(Qt::DisplayRole).toString().length())
        return;

    QMap<QString, QString> filter;
    filter.insert("menu_name", ___settings->toString(SETTINGS_MENU_NAME));
    filter.insert("group_name", ui->tbGroups->item(index.row(), index.column())->data(Qt::DisplayRole).toString());
    ___menu.configureDishTable(ui->tbDishes, filter);
}

void DlgOrder::on_tblGroupOfGroup_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    if (!index.data().toString().length())
        return;
    ___menu.configureGroupTable(ui->tbGroups, ___settings->toString(SETTINGS_MENU_NAME), index.data().toString());
}

void DlgOrder::on_tbDishes_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int i = (index.row() * ui->tbDishes->columnCount()) + index.column();
    if (i > ___menu.filteredDishesCount() - 1)
        return;

    QMap<QString, QString> dish = ___menu.dish(i);
    dish[DISH_LAST_USER] = m_staff->id();
    m_order->appendDish(dish);
    const QStringList &printers = ___menu.printSchema(dish);
    m_order->appendPrinterToLastDish(printers);
    m_order->afterChange();

    int row = m_order->dishCount() - 1;
    setBtnQtyMoveEnabled(row);
}

void DlgOrder::on_pushButton_18_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_12_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_15_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_13_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_16_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_9_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_14_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_10_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_17_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_19_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_4_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::on_pushButton_20_clicked()
{
    btnQtyClicked(sender());
}

void DlgOrder::closeEvent(QCloseEvent *e)
{
    setControlEnabledForRemove(false);
    ui->widgetDishQty->setEnabled(false);

    if (m_order->dishCount())
        if (!m_order->dishCount(DISH_STATE_NORMAL)) {
            m_order->setOrderProperty(ORDER_STATE_ID, ORDER_STATE_REMOVED);
            m_order->setOrderProperty(ORDER_RELEASE_TABLE, "yes");
        }
    m_order->setOrderProperty(ORDER_DATE_CLOSE, QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    m_order->dateCash(___settings->toString(SETTINGS_CASH_DATE_CHANGE));
    m_order->putVoid(SLOT(unlockTable(int,QString)));
    m_order->putVoid(SLOT(remove(int,QString)));
    m_order->save(0, "");
    while (!m_order->removed)
        qApp->processEvents();
    m_staff->release();
    QDialog::closeEvent(e);
}

void DlgOrder::btnQtyClicked(QObject *button)
{
    QString btnText = (qobject_cast<QPushButton *>(button))->text();

    if (!btnText.compare("0") || !btnText.compare("1") || !btnText.compare("2") ||
        !btnText.compare("3") || !btnText.compare("4") || !btnText.compare("5") ||
        !btnText.compare("6") || !btnText.compare("7") || !btnText.compare("8") ||
        !btnText.compare("9")) {
        m_dishQty += btnText;
    }

    if (!btnText.compare("."))
        if (!m_dishQty.contains("."))
            m_dishQty += ".";

    if (!btnText.compare("X"))
        m_dishQty = "0";

    emit qtyText(m_dishQty);
}

void DlgOrder::slotQtyText(QString &qty)
{
    int row = Toolkit::currentRow(ui->tbOrderDishes, row);
    if (row < 0)
        return;

    m_order->changeDishQty(row, qty);
}

void DlgOrder::slotConfirmQtyDel()
{
    int row;
    if (Toolkit::currentRow(ui->tbOrderDishes, row) < 0)
        return;

    if (m_data["qty_typed"].toDouble() <= 0)
        return;

    m_order->descreaseDishQty(row, m_data["qty_typed"].toDouble(), "1", "");
    setControlEnabledForRemove(true);
}

void DlgOrder::slotConfirmQtyMovement()
{
    m_order->putVoid(SLOT(saveMovedDishes(int,QString)));
    m_order->putVoid(SLOT(newOrder(int,QString)));
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->save(0, "");
}

void DlgOrder::slotQtyRemoveOrMovement(QString &qty)
{
    if (qty.toDouble() > m_data["qty_available"].toDouble())
        qty = m_data["qty_available"].toString();
    m_data["qty_typed"] = qty;
    m_data["qty_end"] = m_data["qty_begin"].toDouble() - m_data["qty_typed"].toDouble();
    m_order->setOrderProperty(ORDER_DISH_QTY_FOR_MOVEMENT, m_data["qty_typed"]);
    ui->btnQtyAccept->setText(Toolkit::formatDouble(m_data["qty_typed"].toDouble()));
    ui->lbRemoveOrMovedQty->setText(QString("%1 --> %2, %3").arg(m_data["qty_begin"].toDouble())
            .arg(m_data["qty_end"].toDouble())
            .arg(m_order->getOrderProperty(ORDER_DST_TABLE_NAME).toString()));
}

void DlgOrder::takeTotalSale(const QString &qName, const QXmlAttributes &attr)
{
    if (qName == "totalsale") {
        DlgMessageBox::show(attr.value(0) + " - " + attr.value(1));
        return;
    }
}

void DlgOrder::endOfTotalSale(int code, const QString &message)
{
    ___dlgProgress->hide();
    BaseNetwork::removeInstance(sender());
    if (code)
        DlgMessageBox::show(message, 1);
}

void DlgOrder::orderStopped(int code, const QString &message)
{
    m_order->afterChange();
    if (m_order->getOrderProperty(ORDER_ID).toInt()) {
        if (m_order->getOrderProperty(ORDER_STAFF_ID).toInt() != m_order->getOrderProperty(ORDER_CURRENT_STAFF_ID).toInt())
            ui->tableOrderInfo->item(0, 2)->setData(Qt::BackgroundColorRole, QColor::fromRgb(255, 0, 0));
        else
            ui->tableOrderInfo->item(0, 2)->setData(Qt::BackgroundColorRole, QColor::fromRgb(255, 255, 255));
    }
    ui->tbOrderDishes->selectionModel()->clearSelection();
    ui->btnQtyDel->setEnabled(false);
    ui->btnDishComment->setEnabled(false);
    //ui->btnDishSpecialComment->setEnabled(false);
    ___dlgProgress->hide();
    if (code)
        DlgMessageBox::show(message, 1, this);
    if (code == 11 || code == 12)
        close();
    if (m_flagClose)
        close();
    if(m_data["table_mode"] == "move_dish") {
        m_data["table_mode"] = "";
        setControlEnabledForRemove(true);
    }
}

void DlgOrder::takeDiscountElement(const QString &qName, const QXmlAttributes &atts)
{
    if (qName == "mod") {
        m_order->setMod(atts);
        return;
    }
}

void DlgOrder::endOfDiscountElement(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    if (code) {
        ___dlgProgress->hide();
        DlgMessageBox::show(message, 1, this);
        return;
    }
    m_order->afterChange();
    m_order->save(0, "");
}

void DlgOrder::endChangeStaffQuery(int code, const QString &message)
{
    Q_UNUSED(message)
    ___dlgProgress->hide();
    StationAuth *newStaff = qobject_cast<StationAuth*>(sender());
    if (!code) {
        m_order->setOrderProperty(ORDER_STAFF_ID, newStaff->id());
        ui->tableOrderInfo->item(0, 2)->setText(newStaff->fullName());
        ui->btnExit->click();
    } else
        DlgMessageBox::show(tr("Unknown staff"), 1);
    newStaff->deleteLater();
}

void DlgOrder::on_btnOrderDown_clicked()
{
    ui->tbOrderDishes->verticalScrollBar()->setValue(ui->tbOrderDishes->verticalScrollBar()->value() + 5);
}

void DlgOrder::on_btnOrderUp_clicked()
{
    ui->tbOrderDishes->verticalScrollBar()->setValue(ui->tbOrderDishes->verticalScrollBar()->value() - 5);
}

void DlgOrder::on_tbOrderDishes_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn)
    Q_UNUSED(previousRow)
    Q_UNUSED(previousColumn)

    m_dishQty = "";
    ui->btnDishComment->setEnabled(false);
    ui->btnQtyDel->setEnabled(false);
    ui->btnMoveDish->setEnabled(false);
    disconnect(this, SIGNAL(qtyText(QString&)), this, SLOT(slotQtyRemoveOrMovement(QString&)));
    connect(this, SIGNAL(qtyText(QString&)), SLOT(slotQtyText(QString&)), Qt::UniqueConnection);

    if (currentRow < 0)
        return;

    if (!m_order)
        return;


}

void DlgOrder::on_btnDishComment_clicked()
{
    int row;
    if (Toolkit::currentRow(ui->tbOrderDishes, row) < 0)
        return;

    if (!___menu.dishesComments().count())
        return;

    DlgDishComment *dishComment = new DlgDishComment(___menu.dishesComments(), this);
    if (dishComment->exec() == QDialog::Accepted)
        m_order->setDishComment(row, dishComment->result());
    delete dishComment;
}

void DlgOrder::prepareForMovementOrRemoveDish(const char *slot)
{
    disconnect(this, SIGNAL(confirmQty()), this, SLOT(slotConfirmQtyMovement()));
    disconnect(this, SIGNAL(confirmQty()), this, SLOT(slotConfirmQtyDel()));

    int row;
    if (Toolkit::currentRow(ui->tbOrderDishes, row) < 0)
        return;

    double qtyToRemove = 0;
    bool removePrintedQty = m_staff->getWritePermission(PERMISSION_REMOVE_DISH);
    if (removePrintedQty)
        qtyToRemove = m_order->dishProperty(row, DISH_QTY).toDouble();
    else
        qtyToRemove = m_order->dishProperty(row, DISH_QTY).toDouble() - m_order->dishProperty(row, DISH_PRINTED_QTY).toDouble();

    if (qtyToRemove == 0) {
        DlgMessageBox::show(tr("No of available quantity"));
        return;
    }

    setControlEnabledForRemove(false);

    connect(this, SIGNAL(confirmQty()), slot);
    disconnect(this, SIGNAL(qtyText(QString&)), this, SLOT(slotQtyText(QString&)));
    connect(this, SIGNAL(qtyText(QString&)), SLOT(slotQtyRemoveOrMovement(QString&)));

    m_data["qty_begin"] = Toolkit::formatDouble(m_order->dishProperty(row, DISH_QTY).toDouble());
    m_data["qty_end"] = m_data["qty_begin"];
    m_data["qty_available"] = Toolkit::formatDouble(qtyToRemove);
    m_data["qty_typed"] = m_data["qty_available"];
    m_dishQty = m_data["qty_available"].toString();

    if (m_data["table_mode"].toString() == "move_dish")
        m_order->setOrderProperty(ORDER_DISH_QTY_FOR_MOVEMENT, m_data["qty_available"]);


    displayQtyToRemoveOrMovement(m_dishQty);
    m_dishQty = "0";
}

void DlgOrder::setBtnQtyMoveEnabled(int row)
{
    double qty = m_order->dishProperty(row, DISH_QTY).toDouble();
    double printed_qty = m_order->dishProperty(row, DISH_PRINTED_QTY).toDouble();
    ui->btnDishComment->setEnabled(printed_qty == 0);
    //ui->btnDishSpecialComment->setEnabled(printed_qty == 0 && ___menu.dishesCommentsSpecial(m_order->dish(currentRow)).count());
    ui->btnQtyDel->setEnabled((printed_qty == 0 || m_staff->getWritePermission(PERMISSION_REMOVE_DISH)) || (qty > printed_qty));
    ui->btnMoveDish->setEnabled((printed_qty == qty) && m_staff->getWritePermission(PERMISSION_MOVE_DISH));
}

void DlgOrder::displayQtyToRemoveOrMovement(const QString &qty)
{
    Q_UNUSED(qty)

    ui->btnQtyReject->setText(m_data["qty_available"].toString());
    if (m_dishQty.toDouble() > m_data["qty_available"].toDouble()) {
        m_data["qty_typed"] = m_data["qty_available"].toString();
        m_dishQty = m_data["qty_types"].toString();
    }
    else
        m_data["qty_typed"] = m_dishQty;
    m_data["qty_end"] = Toolkit::formatDouble(m_data["qty_begin"].toDouble() - m_data["qty_typed"].toDouble());
    ui->btnQtyAccept->setText(Toolkit::formatDouble(m_data["qty_typed"].toDouble()));
    ui->lbRemoveOrMovedQty->setText(QString("%1 --> %2").arg(m_data["qty_begin"].toString()).arg(m_data["qty_end"].toString()));
    if (m_data["table_mode"] == "move_dish") {
        ui->lbRemoveOrMovedQty->setText(QString("%1 --> %2, %3").arg(m_data["qty_begin"].toDouble())
                .arg(m_data["qty_end"].toDouble())
                .arg(m_order->getOrderProperty(ORDER_DST_TABLE_NAME).toString()));
    }
}

void DlgOrder::setControlEnabledForRemove(const bool &enable)
{
    ui->wdtDishesButtonsSet->setEnabled(enable);
    ui->wdtOrderButtonsSet->setEnabled(enable);
    ui->tbOrderDishes->setEnabled(enable);
    ui->wgDown->setEnabled(enable);
    ui->widgetDishQty->setEnabled(!enable);
    ui->tbOrderDishes->setEnabled(enable);

    if (enable) {
        ui->btnQtyAccept->setText("");
        ui->btnQtyReject->setText("");
        ui->lbRemoveOrMovedQty->setText("-");
    }
}

void DlgOrder::on_btnQtyDel_clicked()
{
    prepareForMovementOrRemoveDish(SLOT(slotConfirmQtyDel()));
}

void DlgOrder::on_btnQtyReject_clicked()
{
    setControlEnabledForRemove(true);
}

void DlgOrder::on_btnQtyAccept_clicked()
{
    emit confirmQty();
}

void DlgOrder::on_btnPrintServiceCheck_clicked()
{
    if (!m_order) {
        DlgMessageBox::show(tr("Nothing to print"));
        return;
    }

    ___dlgProgress->show();

    m_order->setOrderProperty(ORDER_TABLE_TO_LOCK, m_order->getOrderProperty(ORDER_TABLE_ID));
    m_order->putVoid(SLOT(newOrder(int,QString)));
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->putVoid(SLOT(setPrintedQty(int,QString)));
    m_order->putVoid(SLOT(save(int,QString)));
    m_order->save(0, "");
}

void DlgOrder::on_btnMoveOrder_clicked()
{
    Hall2::Table t;
    if (!DlgTables::getTableId(this, t))
        return;

    if (t.toString(TABLE_ID) == m_order->getOrderProperty(ORDER_TABLE_ID)) {
        DlgMessageBox::show(tr("Same table"), 1, this);
        return;
    }

    if (DlgMessageBox::show(tr("Confirm movement") + "<br>" +
                            tr("Source:") + m_order->getOrderProperty(ORDER_TABLE_NAME).toString() + "<br>" +
                            tr("Destination:") + t.toString(TABLE_NAME),
                            0, this) != QDialog::Accepted)
        return;

    ___dlgProgress->show();
    m_order->setOrderProperty(ORDER_DST_TABLE_ID, t.toString(TABLE_ID));
    m_order->setOrderProperty(ORDER_SRC_TABLE_ID, m_order->getOrderProperty(ORDER_TABLE_ID));
    m_order->setOrderProperty(ORDER_TABLE_TO_UNLOCK, m_order->getOrderProperty(ORDER_SRC_TABLE_ID));
    m_order->setOrderProperty(ORDER_TABLE_TO_LOCK, m_order->getOrderProperty(ORDER_DST_TABLE_ID));
    m_order->putVoid(SLOT(unlockTable(int,QString)));
    m_order->putVoid(SLOT(newOrder(int,QString)));
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->moveOrder(0, "");
}

void DlgOrder::on_btnPayment_clicked()
{
    if (DlgMessageBox::show(tr("Confirm closing order"), 0, this) != QDialog::Accepted)
        return;

    ___dlgProgress->show();
    m_flagClose = true;
    m_order->setOrderProperty(ORDER_DATE_CLOSE, QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    m_order->setOrderProperty(ORDER_DATE_CASH, m_order->dateCash(___settings->toString(SETTINGS_CASH_DATE_CHANGE)));
    m_order->setOrderProperty(ORDER_STATE_ID, ORDER_STATE_CLOSED);
    m_order->setOrderProperty(ORDER_RELEASE_TABLE, "yes");
    m_order->save(0, "");

    OrderStoreDoc *orderStoreDoc = new OrderStoreDoc(m_staff->session(), m_order->getOrderProperty(ORDER_ID).toString(), m_order->getOrderProperty("date_cash").toString());
    orderStoreDoc->post();
}

void DlgOrder::on_btnRemoveWholeOrder_clicked()
{
    if (m_order->getOrderProperty(ORDER_ID).toInt() == 0) {
        DlgMessageBox::show(tr("Order is empty"), 1, this);
        return;
    }

    if (DlgMessageBox::show(tr("Confirm order removal"), 0, this) != QDialog::Accepted)
        return;

    ___dlgProgress->show();
    m_flagClose = true;
    m_order->setOrderProperty(ORDER_DATE_CLOSE, QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    m_order->dateCash(___settings->toString(SETTINGS_CASH_DATE_CHANGE));
    m_order->setOrderProperty("state_id", ORDER_STATE_REMOVED);
    m_order->markNormalDishesState(DISH_STATE_REMOVED);
    m_order->setOrderProperty("release_table", "yes");
    m_order->save(0, "");
}

void DlgOrder::on_btnCheckout_clicked()
{
    if (DlgMessageBox::show(tr("Confirm checkout"), 0, this) != QDialog::Accepted)
        return;
    ___dlgProgress->show(tr("Printing"));
    m_order->setOrderProperty(ORDER_CHECKOUT_PRINTER, ___settings->toString(SETTINGS_CHECKOUT_PRINTER));
    m_order->putVoid(SLOT(save(int,QString)));
    m_order->printCheck(0, "");
}

void DlgOrder::on_btnMoveDish_clicked()
{
    int row;
    if (Toolkit::currentRow(ui->tbOrderDishes, row) < 0)
        return;

    Hall2::Table t;
    if (!DlgTables::getTableId(this, t))
        return;

    if ( t.toString(TABLE_ID) == m_order->getOrderProperty(ORDER_TABLE_ID)) {
        DlgMessageBox::show(tr("Same table"), 1, this);
        return;
    }

    if (DlgMessageBox::show(tr("Confirm movement") + "<br>" +
                            tr("Source:") + m_order->getOrderProperty(ORDER_TABLE_NAME).toString() + "<br>" +
                            tr("Destination:") + t.toString(TABLE_NAME),
                            0, this) != QDialog::Accepted)
        return;

    m_order->setOrderProperty(ORDER_DST_TABLE_ID, t.toString(TABLE_ID));
    m_order->setOrderProperty(ORDER_DST_TABLE_NAME, t.toString(TABLE_NAME));
    m_order->setOrderProperty(ORDER_DISH_REC_ID_FOR_MOVEMENT, m_order->dish(row)[DISH_ORDER_REC_ID]);

    m_data["table_mode"] = "move_dish";
    prepareForMovementOrRemoveDish(SLOT(slotConfirmQtyMovement()));
}

void DlgOrder::on_btnMenu_clicked()
{
    QStringList menu;
    menu << tr("View additional info")
         << tr("Set order comment")
         << tr("History")
         << tr("Change staff")
         << tr("Total sale");
    DlgList *d = new DlgList(this);
    d->setMenu(menu);
    if (d->exec() == QDialog::Accepted) {
        if (d->selectedItem() == menu.at(0)) {

        } else if (d->selectedItem() == menu.at(1)) {
            DlgDishComment *d = new DlgDishComment(QStringList(), this);
            if (d->exec() == QDialog::Accepted)
                m_order->setOrderProperty(ORDER_COMMENT, d->result());
            delete d;
        } else if (d->selectedItem() == menu.at(2)) {
            DlgOrderHistory *d = new DlgOrderHistory(m_order->getOrderProperty(ORDER_ID).toString(), m_staff->session(), this);
            d->exec();
            delete d;
        } else if (d->selectedItem() == menu.at(3)) {
            if (!m_staff->getWritePermission(PERMISSION_CHANGE_STAFF)) {
                DlgMessageBox::show(tr("Access denied"), 1);
                return;
            }
            DlgGetPassword *dlg = new DlgGetPassword(this);
            QString password;
            if (dlg->exec() == QDialog::Accepted)
                password = dlg->property("password").toString();
            else
                password = "nopasswordforchanges";
            delete dlg;

            ___dlgProgress->show();
            StationAuth *newStaff = new StationAuth();
            connect(newStaff, SIGNAL(endOfQuery(int,QString)), SLOT(endChangeStaffQuery(int, QString)));
            newStaff->auth2(password);
        } else if (d->selectedItem() == menu.at(4)) {
            ___dlgProgress->show();
            BaseNetwork *bn = new BaseNetwork();
            bn->appendData("sysop", "TOTALSALE");
            bn->appendData("cafe", "1");
            bn->appendData("session", m_staff->session());
            bn->appendData("staff", m_staff->id());
            bn->appendData("date", m_order->dateCash(___settings->toString(SETTINGS_CASH_DATE_CHANGE)));
            connect(bn, SIGNAL(takeElement(QString, QXmlAttributes)), this, SLOT(takeTotalSale(QString, QXmlAttributes)));
            connect(bn, SIGNAL(endOfQuery(int, QString)), this, SLOT(endOfTotalSale(int, QString)));
            bn->post();
        }
    }
    delete d;
}

void DlgOrder::on_btnDiscount_clicked()
{
    QString code;
    DlgInput *d = new DlgInput(this);
    if (d->exec() == QDialog::Accepted)
        code = d->text();
    delete d;
    if (!code.length())
        return;
    ___dlgProgress->show(tr("Inserting discount"));
    BaseNetwork *b = new BaseNetwork();
    b->appendData("sysop", "DISCOUNT");
    b->appendData("cafe", "1");
    b->appendData("order_id", m_order->getOrderProperty(ORDER_ID).toString());
    b->appendData("code", code);
    b->appendData("session", m_staff->session());
    connect(b, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(takeDiscountElement(QString,QXmlAttributes)));
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfDiscountElement(int,QString)));
    b->post();
}


void DlgOrder::on_btnDishListDown_clicked()
{
    ui->tbDishes->verticalScrollBar()->setValue(ui->tbDishes->verticalScrollBar()->value() + 4);
}

void DlgOrder::on_btnDishListUp_clicked()
{
    ui->tbDishes->verticalScrollBar()->setValue(ui->tbDishes->verticalScrollBar()->value() - 4);
}

void DlgOrder::on_tbOrderDishes_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    setBtnQtyMoveEnabled(index.row());
}
