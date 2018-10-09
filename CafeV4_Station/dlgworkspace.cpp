#include "dlgworkspace.h"
#include "ui_dlgworkspace.h"
#include <QPainter>
#include <QPushButton>
#include <QScrollBar>
#include <QGraphicsBlurEffect>
#include "dlgdishcomment.h"
#include "dlgchange.h"
#include "core.h"
#include "../core_gui/dlglist.h"
#include "../core_gui/dlginput.h"
#include "../jazzve/dlggetpassword.h"

#define LOADED_MODULES_COUNT 3

#define ROW_HEIGHT_TABLE 45
#define ROW_HEIGHT_DISHES_TYPES 60
#define ROW_HEIGHT_DISHES 60

#define COLUMN_WIDTH_TABLE 88
#define COLUMN_WIDTH_DISHES_TYPES 176
#define COLUMN_WIDTH_DISHES 264

#define ORDER_COUNTED_SUM 0
#define ORDER_SERVICE_SUM 1
#define ORDER_DISCOUNT_SUM 2
#define ORDER_TOTAL_SUM 3

DlgWorkspace::DlgWorkspace(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::dlgWorkspace)
{
    ui->setupUi(this);
    setDishModelEnable(false);
    ui->wdtDishQty->setEnabled(false);
    ui->tblTable->setEnabled(true);
    ui->wdtOrderButtonsSet->setEnabled(true);
    QObjectList btn = ui->wdtOrderButtonsSet->children();
    for (QObjectList::iterator it = btn.begin(); it != btn.end(); it++) {
        QPushButton *btn = qobject_cast<QPushButton*>(*it);
        if (btn)
            btn->setEnabled(false);
    }
    ui->btnExit->setEnabled(true);
    ui->btnAdditional->setEnabled(true);

    if (!___settings) {
        ___settings = new Settings(Connection::waiterSettings());
    }

    if (!___mod)
        ___mod = new OrderModifierList(___staff.session());

    m_loadedModulesCount = 0;
    m_hallLoaded = false;
    m_menuLoaded = false;
    m_modLoaded = false;
    m_order = 0;

    connect(&___hall, SIGNAL(endOfQuery(int,QString)), SLOT(slotHallLoaded(int,QString)));
    connect(&___menu, SIGNAL(endOfQuery(int,QString)), SLOT(slotMenuLoaded(int,QString)));
    connect(___mod, SIGNAL(endOfQuery(int,QString)), SLOT(slotModLoaded(int,QString)));
    connect(___settings, SIGNAL(endWithError(QString)), this, SLOT(settingsError(QString)));
    connect(___settings, SIGNAL(newVersion()), this, SLOT(newSettingsVersion()));
    connect(___settings, SIGNAL(endOfSettingsQuery(int,QString)), SLOT(endOfSettingsQuery(int,QString)));

    //Order info
    QFont font(qApp->font());
    font.setPointSize(10);
    ui->tableOrderInfo->verticalHeader()->hide();
    ui->tableOrderInfo->horizontalHeader()->setFont(font);
    ui->tableOrderInfo->setRowHeight(0, 40);
    ui->tableOrderInfo->setColumnWidth(0, 100);
    ui->tableOrderInfo->setColumnWidth(1, 100);
    ui->tableOrderInfo->setColumnWidth(2, 200);
    for (int i = 0; i < 3; i++)
        ui->tableOrderInfo->setItem(0, i, new QTableWidgetItem());
    ui->tableOrderInfo->item(0, 2)->setData(Qt::DisplayRole, ___staff.fullName());

    //Dishes table
    ui->tableDishes->setFont(font);
    ui->tableDishes->setColumnWidth(0, 370);
    ui->tableDishes->setColumnWidth(1, 80);

    ui->btnPrintServiceCheck->setEnabled(false);
    ui->btnDishComment->setEnabled(false);
    ui->btnQtyDel->setEnabled(false);
    ui->btnMoveDish->setEnabled(false);
    ui->btnCheckout->setEnabled(false);
    ui->btnPayment->setEnabled(false);
    ui->btnDiscount->setEnabled(false);

    //Payment info
    ui->tableSums->setItemDelegate(new TableColDelegate());
    ui->tableSums->setColumnWidth(0, 300);

    //Menu table
    ui->tableOrderInfo->setItemDelegate(new TableHeaderItemDelegate());

    m_dishQty = "";
    m_data["table_mode"] = tmTable;

    //Enabled functions
    ui->btnRemoveWholeOrder->setVisible(___staff.getWritePermission(PERMISSION_REMOVE_ORDER));
    ui->btnMoveOrder->setVisible(___staff.getWritePermission(PERMISSION_MOVE_ORDER));
    ui->btnMoveDish->setVisible(___staff.getWritePermission(PERMISSION_MOVE_DISH));
    connect(this, SIGNAL(qtyText(QString&)), SLOT(slotQtyText(QString&)), Qt::UniqueConnection);

    showFullScreen();
    ___progressDialog->show();
    ___settings->loadSettings(___staff.session(), "1");

    connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    m_timer.start(60000);
}

DlgWorkspace::~DlgWorkspace()
{
    m_timer.disconnect(this, SLOT(slotTimeout()));
    delete ui;
}

void DlgWorkspace::checkLoadedModulesCount()
{
    if (m_loadedModulesCount < LOADED_MODULES_COUNT)
        return;

    bool allSuccess = m_hallLoaded && m_menuLoaded && m_modLoaded;
    ___progressDialog->hide();
    if (!allSuccess) {
        DlgMessageBox::show(tr("Some modules not loaded. Order editing is not possible."), 1, this);
        close();
        return;
    }
}

void DlgWorkspace::updateTableStatus(Order *order)
{
    const QModelIndex &tableIndex = order->tableModelIndex();
    QString orderId = order->getOrderProperty(ORDER_ID).toString();
    int tableId = order->getOrderProperty("table_id").toInt();
    ___hall.setTableValue(tableId, TABLE_ORDER_ID, orderId);
    ui->tblTable->update(tableIndex);
}

void DlgWorkspace::prepareToRemoveOrMovementDish(const char *confirmSlot)
{
    disconnect(this, SIGNAL(confirmQty(double)), this, SLOT(slotConfirmQtyForMovement(double)));
    disconnect(this, SIGNAL(confirmQty(double)), this, SLOT(slotConfirmQtyForRemove(double)));
    int row;
    if (Toolkit::currentRow(ui->tableDishes, row) < 0)
        return;

    double qtyToRemove = 0;
    bool removePrintedQty = ___staff.getWritePermission(PERMISSION_REMOVE_DISH);
    if (removePrintedQty)
        qtyToRemove = m_order->dishProperty(row, DISH_QTY).toDouble();
    else
        qtyToRemove = m_order->dishProperty(row, DISH_QTY).toDouble() - m_order->dishProperty(row, DISH_PRINTED_QTY).toDouble();

    if (qtyToRemove == 0) {
        DlgMessageBox::show(tr("No of available quantity"));
        return;
    }

    setDishModelEnable(false);
    connect(this, SIGNAL(confirmQty(double)), confirmSlot);
    m_data["qty_begin"] = Toolkit::formatDouble(m_order->dishProperty(row, DISH_QTY).toDouble());
    m_data["qty_end"] = m_data["qty_begin"];
    m_data["qty_available"] = Toolkit::formatDouble(qtyToRemove);
    m_data["qty_typed"] = m_data["qty_available"];
    m_dishQty = m_data["qty_available"].toString();

    disconnect(this, SIGNAL(qtyText(QString&)), this, SLOT(slotQtyText(QString&)));
    connect(this, SIGNAL(qtyText(QString&)), SLOT(slotQtyRemoveOrMovemenet(QString&)));
    displayQtyToRemoveOrMovement(m_dishQty);
    m_dishQty = "0";
}

void DlgWorkspace::displayQtyToRemoveOrMovement(const QString &qty)
{
    Q_UNUSED(qty)

    ui->btnQtyReject->setText(m_data["qty_available"].toString());
    if (m_dishQty.toDouble() > m_data["qty_available"].toDouble()) {
        m_data["qty_typed"] = m_data["qty_available"].toString();
        m_dishQty = m_data["qty_typed"].toString();
    } else
        m_data["qty_typed"] = m_dishQty;

    m_data["qty_end"] = Toolkit::formatDouble(m_data["qty_begin"].toDouble() - m_data["qty_typed"].toDouble());
    ui->btnQtyAccept->setText(Toolkit::formatDouble(m_data["qty_typed"].toDouble()));
    ui->lbRemoveOrMovedQty->setText(QString("%1 --> %2").arg(m_data["qty_begin"].toString()).arg(m_data["qty_end"].toString()));
}

void DlgWorkspace::setDishModelEnable(const bool &visible)
{
    ui->tblGroupOfGroup->setEnabled(visible);
    ui->wdtDish->setEnabled(visible);
    ui->wdtGroup->setEnabled(visible);
    ui->tblTable->setEnabled(visible);
    ui->wdtDishesButtonsSet->setEnabled(visible);
    ui->wdtOrderButtonsSet->setEnabled(visible);
    ui->tableDishes->setEnabled(visible);
    ui->wdtDishQty->setEnabled(!visible);

    if (visible) {
        ui->btnQtyAccept->setText("");
        ui->btnQtyReject->setText("");
        ui->lbRemoveOrMovedQty->setText("-");
        m_data["table_mode"] = tmTable;
    }
}

void DlgWorkspace::configureTables()
{
    m_hallFilter[TABLE_HALL_NAME] = ___settings->toString(SETTINGS_HALL_ID);
    ___hall.configureTableWidget(ui->tblTable, m_hallFilter);
    ___menu.configureGroupOfGroupTable(ui->tblGroupOfGroup);
    ___menu.configureGroupTable(ui->tblGroup, ___settings->toString(SETTINGS_MENU_NAME), "");
    ___progressDialog->hide();
}

QModelIndex DlgWorkspace::getTableIndex(const QVariant &tableId)
{
    for (int i = 0, rowCount = ui->tblTable->rowCount(); i < rowCount; i++)
        for (int j = 0, colCount = ui->tblTable->columnCount(); j < colCount; j++) {
            QTableWidgetItem *item = ui->tblTable->item(i, j);
            if (!item)
                return QModelIndex();
            if (item->data(Qt::UserRole).toInt() == tableId.toInt())
                return ui->tblTable->model()->index(i, j);
        }
    return QModelIndex();
}

void DlgWorkspace::closeEvent(QCloseEvent *e)
{
    disconnect(___settings, SIGNAL(endWithError(QString)), this, SLOT(settingsError(QString)));
    disconnect(___settings, SIGNAL(newVersion()), this, SLOT(newSettingsVersion()));

    if (m_order) {
        m_order->removed = false;
        m_order->setOrderProperty(ORDER_DATE_CLOSE, QDateTime::currentDateTime().toString(DATETIME_FORMAT));
        m_order->setOrderProperty(ORDER_DATE_CASH, m_order->dateCash(___settings->toString(SETTINGS_CASH_DATE_CHANGE)));
        m_order->setOrderProperty(ORDER_TABLE_TO_UNLOCK, m_order->getOrderProperty(ORDER_TABLE_ID));
        m_order->putVoid(SLOT(unlockTable(int,QString)));
        m_order->putVoid(SLOT(remove(int,QString)));
        m_order->save(0, "");
        while (!m_order->removed)
            qApp->processEvents();
        delete m_order;
    }

    for (int i = 0; i < 1000; i++)
        qApp->processEvents();

    for (int i = 0; i < 1000; i++)
        qApp->processEvents();

    for (int i = 0; i < 1000; i++)
        qApp->processEvents();

    ___staff.release();

    for (int i = 0; i < 1000; i++)
        qApp->processEvents();

    QDialog::closeEvent(e);
}

void DlgWorkspace::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Escape) {
        close();
        return;
    }

    QDialog::keyPressEvent(e);
}

void DlgWorkspace::slotTimeout()
{
    ___hall.post(___staff.session());
}

void DlgWorkspace::slotUpdateTableStatus(int code, const QString &message)
{
    if (code)
        DlgMessageBox::show(message, 1, this);

    Order *order = qobject_cast<Order*>(sender());
    updateTableStatus(order);
}

void DlgWorkspace::slotHallLoaded(int code, const QString &message)
{
    m_hallFilter[TABLE_HALL_NAME] = ___settings->toString(SETTINGS_HALL_ID);
    ___hall.configureTableWidget(ui->tblTable, m_hallFilter);

    if (m_data["table_mode"].toInt() == tmMoveTable) {
        m_data["table_mode"] = tmTable;
        QModelIndex index = getTableIndex(m_order->getOrderProperty(ORDER_DST_TABLE_ID));
        ui->tblTable->setCurrentIndex(index);
        return;
    }

    if (m_data["update_after_movement"].toString() == "1") {
        m_data["update_after_movement"] = "0";
        ___progressDialog->hide();
        return;
    }

    for (int i = 0, cols = ui->tblTable->columnCount(); i < cols; i++)
        for (int j = 0, rows = ui->tblTable->rowCount(); j < rows; j++)
            ui->tblTable->update(ui->tblTable->model()->index(j, i));

    Q_UNUSED(message)
    m_hallLoaded = code == 0;
    m_loadedModulesCount++;
    checkLoadedModulesCount();
}

void DlgWorkspace::slotMenuLoaded(int code, const QString &message)
{
    if (!code) {
        ___menu.configureGroupOfGroupTable(ui->tblGroupOfGroup);
        ___menu.configureGroupTable(ui->tblGroup, ___settings->toString(SETTINGS_MENU_NAME), "");
    }

    Q_UNUSED(message)
    m_menuLoaded = code == 0;
    m_loadedModulesCount++;
    checkLoadedModulesCount();
}

void DlgWorkspace::slotModLoaded(int code, const QString &message)
{
    Q_UNUSED(code)
    Q_UNUSED(message)
    m_modLoaded = true;
    m_loadedModulesCount++;
    checkLoadedModulesCount();
}

void DlgWorkspace::orderStopped(int code, const QString &message)
{
    ui->btnCangeOrderOwner->setVisible(___staff.getWritePermission(PERMISSION_CHANGE_STAFF));
    ui->btnCangeOrderOwner->setEnabled(___staff.getWritePermission(PERMISSION_CHANGE_STAFF));
    ui->tableDishes->selectionModel()->clearSelection();
    ui->btnQtyDel->setEnabled(false);
    ui->btnDishComment->setEnabled(false);
    ui->btnDishSpecialComment->setEnabled(false);
    m_dishQty = "";
    if (m_data["update_after_movement"].toString() == "1") {
        if (code)
            DlgMessageBox::show(message, 1, this);
        ___hall.post(___staff.session());
        return;
    }

    if (m_data["table_mode"].toInt() == tmMoveTable) {
        ___hall.post(___staff.session());
        return;
    }

    ___progressDialog->hide();
    if (code) {
        DlgMessageBox::show(message);
        ui->tblTable->setEnabled(true);
        ui->wdtOrderButtonsSet->setEnabled(true);
        QObjectList btn = ui->wdtOrderButtonsSet->children();
        for (QObjectList::iterator it = btn.begin(); it != btn.end(); it++) {
            QPushButton *btn = qobject_cast<QPushButton*>(*it);
            if (btn)
                btn->setEnabled(false);
        }
        ui->btnExit->setEnabled(true);
        ui->btnAdditional->setEnabled(true);
    }
    else
        setDishModelEnable(true);
    m_order->afterChange();
}

void DlgWorkspace::slotQtyText(QString &qty)
{
    int row = ui->tableDishes->currentRow();
    if (row < 0) {
        qty = "";
        return;
    }

    m_order->changeDishQty(row, qty);
}

void DlgWorkspace::slotQtyRemoveOrMovemenet(QString &qty)
{
    displayQtyToRemoveOrMovement(qty);
}

void DlgWorkspace::slotConfirmQtyForRemove(const double &qty)
{
    int row;
    if (Toolkit::currentRow(ui->tableDishes, row) < 0)
        return;

    Order::Dish &dish = m_order->dish(row);
    QString reason;
    if (dish[DISH_PRINTED_QTY].toDouble() > dish[DISH_QTY].toDouble() - qty) {
        DlgDishComment *d = new DlgDishComment(___menu.rmReasons(), this);
        d->setSingleSelection();
        if (d->exec() == QDialog::Accepted)
            reason = d->result();
        delete d;
    }
    m_order->descreaseDishQty(row, qty, "1", reason);
    setDishModelEnable(true);
}

void DlgWorkspace::slotConfirmQtyForMovement(const double &qty)
{
    int row;
    if (Toolkit::currentRow(ui->tableDishes, row) < 0)
        return;

    QModelIndexList lstIndex = ui->tblTable->selectionModel()->selectedIndexes();
    if (lstIndex.count() == 0)
        return;

    QTableWidgetItem *item = ui->tblTable->item(lstIndex.at(0).row(), lstIndex.at(0).column());
    int dstTable = item->data(Qt::UserRole).toInt();

    if (!dstTable) {
        DlgMessageBox::show(tr("No table is selected"), 1, this);
        return;
    }

    dstTable = ___hall.tableValue(dstTable, TABLE_ID).toInt();
    if (m_order->getOrderProperty("table_id").toInt() == dstTable) {
        DlgMessageBox::show(tr("Same table"), 1, this);
        return;
    }

    ___progressDialog->show();
    setDishModelEnable(true);
    m_data["update_after_movement"] = "1";

    m_order->setOrderProperty(ORDER_DST_TABLE_ID, QString::number(dstTable));
    m_order->setOrderProperty(ORDER_DISH_REC_ID_FOR_MOVEMENT, m_order->dish(row)[DISH_ORDER_REC_ID]);
    m_order->setOrderProperty(ORDER_DISH_QTY_FOR_MOVEMENT, qty);
    m_order->putVoid(SLOT(saveMovedDishes(int,QString)));
    m_order->putVoid(SLOT(newOrder(int,QString)));
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->save(0, "");
}

void DlgWorkspace::slotPayment()
{
    close();
}

void DlgWorkspace::slotClose()
{
    close();
}

void DlgWorkspace::TableHeaderItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QFont font(qApp->font());
    font.setPointSize(12);
    font.setBold(true);
    painter->setFont(font);

    QRect rect = option.rect;

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);

    QPen pen(Qt::blue);
    painter->setPen(pen);

    painter->drawText(rect, index.data().toString(), textOption);
}

void DlgWorkspace::on_btnOrderDown_clicked()
{
    ui->tableDishes->verticalScrollBar()->setValue(ui->tableDishes->verticalScrollBar()->value() + 7);
}

void DlgWorkspace::on_btnOrderUp_clicked()
{
    ui->tableDishes->verticalScrollBar()->setValue(ui->tableDishes->verticalScrollBar()->value() - 7);
}

void DlgWorkspace::TableColDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QFont font(qApp->font());
    font.setPointSize(13);
    font.setBold(true);
    painter->setFont(font);

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignVCenter);
    QRect rect = option.rect;
    rect.adjust(4, 0, 0, 0);
    painter->drawText(rect, index.data().toString(), textOption);
}

void DlgWorkspace::on_btnPrintServiceCheck_clicked()
{
    if (!m_order) {
        DlgMessageBox::show(tr("Nothing to print"));
        return;
    }

    ___progressDialog->show();

    m_order->setOrderProperty(ORDER_TABLE_TO_LOCK, m_order->getOrderProperty(ORDER_TABLE_ID));
    m_order->putVoid(SLOT(newOrder(int,QString)));
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->putVoid(SLOT(setPrintedQty(int,QString)));
    m_order->putVoid(SLOT(save(int,QString)));
    m_order->save(0, "");
}

void DlgWorkspace::on_btnExit_clicked()
{
    close();
}

void DlgWorkspace::on_pushButton_12_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_18_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_15_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_13_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_16_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_9_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_14_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_10_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_17_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_19_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_4_clicked()
{
    btnQtyClicked(sender());
}

void DlgWorkspace::on_pushButton_20_clicked()
{
    if (!m_order)
        return;

    DlgChange *d = new DlgChange(m_order->getOrderProperty(ORDER_AMOUNT).toString(), this);
    d->exec();
    delete d;
}

void DlgWorkspace::btnQtyClicked(QObject *button)
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

void DlgWorkspace::on_btnQtyDel_clicked()
{
    prepareToRemoveOrMovementDish(SLOT(slotConfirmQtyForRemove(double)));
}

void DlgWorkspace::on_btnPayment_clicked()
{

    if (DlgMessageBox::show(tr("Confirm closing order"), 0, this) != QDialog::Accepted)
        return;

    ___progressDialog->show();
    m_order->setOrderProperty(ORDER_DATE_CLOSE, QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    m_order->setOrderProperty(ORDER_DATE_CASH, m_order->dateCash(___settings->toString(SETTINGS_CASH_DATE_CHANGE)));
    m_order->setOrderProperty(ORDER_STATE_ID, ORDER_STATE_CLOSED);
    m_order->setOrderProperty(ORDER_RELEASE_TABLE, "yes");
    m_order->putVoid(SLOT(updateTableItem(int,QString)));
    m_order->putVoid(SLOT(newOrder(int,QString)));
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->save(0, "");

    OrderStoreDoc *orderStoreDoc = new OrderStoreDoc(___staff.session(), m_order->getOrderProperty(ORDER_ID).toString(), m_order->getOrderProperty(ORDER_DATE_CASH).toString());
    orderStoreDoc->post();
    on_tblTable_currentItemChanged(0, ui->tblTable->item(m_order->tableModelIndex().row(), m_order->tableModelIndex().column()));
}

void DlgWorkspace::slotEndTableMovement(int code)
{
    switch (code) {
    case MOVEMENT_RESULT_OK:
        DlgMessageBox::show(tr("Movement successfull"), 0, this);
        break;
    case MOVEMENT_RESULT_SRC_TABLE_NOT_DEFINED:
        DlgMessageBox::show(tr("Source table is not defined"), 1, this);
        break;
    case MOVEMENT_RESULT_DST_TABLE_NOT_DEFINED:
        DlgMessageBox::show(tr("Destination table is not define"), 1, this);
        break;
    case MOVEMENT_RESULT_CANNOT_LOCK_TABLE:
        DlgMessageBox::show(tr("The one of the tables is editing"), 1, this);
        break;
    case MOVEMENT_RESULT_NETWORK_ERROR:
        DlgMessageBox::show(tr("Network error"), 1, this);
        break;
    default:
        break;
    }
}

void DlgWorkspace::settingsError(const QString &message)
{
    ___progressDialog->hide();
    DlgMessageBox::show(message, 1, this);
}

void DlgWorkspace::newSettingsVersion()
{
    ___settings->uploadPrinters(___staff.session());
    ___hall.post(___staff.session());
    ___menu.post(___staff.session());
    m_availableHalls = ___settings->toString(SETTINGS_AVAILABLE_HALLS).split(",");
}

void DlgWorkspace::endOfSettingsQuery(int code, const QString &message)
{
    if (code) {
        DlgMessageBox::show(message, 1, this);
        return;
    }

    ___settings->uploadPrinters(___staff.session());
    m_availableHalls = ___settings->toString(SETTINGS_AVAILABLE_HALLS).split(",");
    configureTables();
}

void DlgWorkspace::on_tableDishes_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    Q_UNUSED(currentColumn)
    Q_UNUSED(previousRow)
    Q_UNUSED(previousColumn)

    m_dishQty = "";
    ui->btnDishComment->setEnabled(false);
    ui->btnDishSpecialComment->setEnabled(false);
    ui->btnQtyDel->setEnabled(false);
    ui->btnMoveDish->setEnabled(false);
    disconnect(this, SIGNAL(qtyText(QString&)), this, SLOT(slotQtyRemoveOrMovemenet(QString&)));
    connect(this, SIGNAL(qtyText(QString&)), SLOT(slotQtyText(QString&)), Qt::UniqueConnection);

    if (currentRow < 0)
        return;

    if (!m_order)
        return;

    double qty = m_order->dishProperty(currentRow, DISH_QTY).toDouble();
    double printed_qty = m_order->dishProperty(currentRow, DISH_PRINTED_QTY).toDouble();
    ui->btnDishComment->setEnabled(printed_qty == 0);
    ui->btnDishSpecialComment->setEnabled(printed_qty == 0 && ___menu.dishesCommentsSpecial(m_order->dish(currentRow)).count());
    ui->btnQtyDel->setEnabled(printed_qty == 0 || ___staff.getWritePermission(PERMISSION_REMOVE_DISH));
    ui->btnMoveDish->setEnabled((printed_qty == qty) && ___staff.getWritePermission(PERMISSION_MOVE_DISH));
}

void DlgWorkspace::on_btnQtyAccept_clicked()
{
    double qty = m_data["qty_typed"].toDouble();
    if (qty <= 0) {
        DlgMessageBox::show(tr("Invalid quantity"));
        return;
    }

    disconnect(this, SIGNAL(qtyText(QString&)), this, SLOT(slotQtyRemoveOrMovemenet(QString&)));
    connect(this, SIGNAL(qtyText(QString&)), SLOT(slotQtyText(QString&)), Qt::UniqueConnection);
    emit confirmQty(qty);
}

void DlgWorkspace::on_btnDishComment_clicked()
{
    int row;
    if (Toolkit::currentRow(ui->tableDishes, row) < 0)
        return;

    DlgDishComment *dishComment = new DlgDishComment(___menu.dishesComments(), this);
    if (dishComment->exec() == QDialog::Accepted)
        m_order->setDishComment(row, dishComment->result());
    delete dishComment;
}

void DlgWorkspace::on_btnMoveDish_clicked()
{
    m_data["table_mode"] = tmMoveDish;
    prepareToRemoveOrMovementDish(SLOT(slotConfirmQtyForMovement(double)));
    ui->tblTable->setEnabled(true);
}

void DlgWorkspace::on_btnCheckout_clicked()
{
    ___progressDialog->show(tr("Printing"));
    m_order->setOrderProperty(ORDER_CHECKOUT_PRINTER, ___settings->toString(SETTINGS_CHECKOUT_PRINTER));
    m_order->putVoid(SLOT(save(int,QString)));
    m_order->printCheckJ(0, "");
}

void DlgWorkspace::on_btnRemoveWholeOrder_clicked()
{
    if (m_order->getOrderProperty(ORDER_ID).toInt() == 0) {
        DlgMessageBox::show(tr("Order is empty"), 1, this);
        return;
    }

    if (DlgMessageBox::show(tr("Confirm order removal"), 0, this) != QDialog::Accepted)
        return;

    DlgDishComment *d = new DlgDishComment(___menu.rmReasons(), this);
    d->setSingleSelection();
    QString reason;
    if (d->exec() == QDialog::Accepted)
        reason = d->result();
    delete d;
    for (int i = 0, count = m_order->dishCount(); i < count; i++) {
        Order::Dish &dish = m_order->dish(i);
        if (dish[DISH_STATE_ID].toInt() == DISH_STATE_NORMAL)
            dish[DISH_REMOVE_REASON] = reason;
    }

    ___progressDialog->show();
    m_order->setOrderProperty(ORDER_DATE_CLOSE, QDateTime::currentDateTime().toString(DATETIME_FORMAT));
    m_order->setOrderProperty(ORDER_RELEASE_TABLE, "yes");
    m_order->setOrderProperty(ORDER_DATE_CASH, m_order->dateCash(___settings->toString(SETTINGS_CASH_DATE_CHANGE)));
    m_order->setOrderProperty(ORDER_STATE_ID, ORDER_STATE_REMOVED);
    m_order->markNormalDishesState(DISH_STATE_REMOVED);
    m_order->putVoid(SLOT(updateTableItem(int,QString)));
    m_order->putVoid(SLOT(newOrder(int,QString)));
    m_order->putVoid(SLOT(lockTable(int,QString)));
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->save(0, "");
    on_tblTable_currentItemChanged(0, ui->tblTable->item(m_order->tableModelIndex().row(), m_order->tableModelIndex().column()));
}

void DlgWorkspace::on_tblGroup_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QString groupName = index.data().toString();
    if (!groupName.length())
        return;

    m_dishFilter[DISH_MENU_NAME] = ___settings->toString(SETTINGS_MENU_NAME);
    m_dishFilter[DISH_GROUP_NAME] = groupName;
    ___menu.configureDishTable(ui->tblDish, m_dishFilter);
}

void DlgWorkspace::on_tblTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous)
    if (m_data["table_mode"] == tmMoveDish)
        return;

    if (m_data["table_mode"] == tmMoveTable) {
        if (current) {
            if (DlgMessageBox::show(tr("Confirm movement") + "<br>" +
                                    tr("Source:") + m_order->getOrderProperty(ORDER_TABLE_NAME).toString() + "<br>" +
                                    tr("Destination:") + ___hall.tableValue(current->data(Qt::UserRole).toInt(), TABLE_NAME).toString(),
                                    0, this) != QDialog::Accepted)
                return;
            m_order->setOrderProperty(ORDER_DST_TABLE_ID, ___hall.tableValue(current->data(Qt::UserRole).toInt(), TABLE_ID));
            m_order->setOrderProperty(ORDER_SRC_TABLE_ID, m_order->getOrderProperty(ORDER_TABLE_ID));
            m_order->putVoid(SLOT(updateTableItem(int,QString)));
            m_order->putVoid(SLOT(unlockTable(int,QString)));
            m_order->putVoid(SLOT(newOrder(int,QString)));
            m_order->putVoid(SLOT(lockTable(int,QString)));
            m_order->putVoid(SLOT(read(int,QString)));
            m_order->moveOrder(0, "");
        }
        return;
    }

    if (current) {
        if (!current->data(Qt::UserRole).toInt())
            return;

        ___progressDialog->show();
        setDishModelEnable(false);
        ui->wdtDishQty->setEnabled(false);
        if (!m_order) {
            m_order = new Order(___staff.session());
            connect(m_order, SIGNAL(stopped(int,QString)), SLOT(orderStopped(int,QString)));
            connect(m_order, SIGNAL(setTableState(int,QString,int)), &___hall, SLOT(setTableState(int,QString,int)));
            m_order->appendWidget("table", ui->tableDishes);
            m_order->appendWidget("table_info", ui->tableOrderInfo);
            m_order->appendWidget("payment_display", ui->tableSums);
            m_order->appendWidget("btn_service_print", ui->btnPrintServiceCheck);
            m_order->appendWidget("btn_checkout_print", ui->btnCheckout);
            m_order->appendWidget("btn_payment", ui->btnPayment);
            m_order->appendWidget("table_tables", ui->tblTable);
            m_order->appendWidget("btn_remove_order", ui->btnRemoveWholeOrder);
            m_order->appendWidget("btn_move_order", ui->btnMoveOrder);
            m_order->appendWidget("table_dishes", ui->tblDish);
            m_order->appendWidget("btn_discount", ui->btnDiscount);
            m_order->setOrderProperty(ORDER_CURRENT_STAFF_NAME, ___staff.fullName());
            m_order->setOrderProperty(PERMISSION_PRINT_CHECK_MORE_THEN_ONE, ___staff.getWritePermission(PERMISSION_PRINT_CHECK_MORE_THEN_ONE));
            m_order->setOrderProperty(PERMISSION_APPEND_DISH_AFTER_CHECKOUT, ___staff.getWritePermission(PERMISSION_APPEND_DISH_AFTER_CHECKOUT));
        }

        m_order->setAutoModSrc(___mod);
        m_order->setOrderProperty(PERMISSION_PAYMENT_ALL_ORDERS, ___staff.getWritePermission(PERMISSION_PAYMENT_ALL_ORDERS));
        m_order->setOrderProperty(ORDER_CURRENT_STAFF_ID, ___staff.id());
        m_order->setOrderProperty(ORDER_TABLE_TO_UNLOCK, m_order->getOrderProperty(ORDER_TABLE_ID));
        m_order->setOrderProperty(ORDER_TABLE_TO_LOCK, ___hall.tableValue(current->data(Qt::UserRole).toInt(), TABLE_ID));
        m_order->putVoid(SLOT(read(int,QString)));
        m_order->putVoid(SLOT(updateTableItem(int,QString)));
        m_order->putVoid(SLOT(unlockTable(int,QString)));
        m_order->putVoid(SLOT(newOrder(int,QString)));
        m_order->putVoid(SLOT(lockTable(int,QString)));
        m_order->putVoid(SLOT(clearMod(int,QString)));
        m_order->putVoid(SLOT(read(int,QString)));
        m_order->putVoid(SLOT(setAutoMod(int,QString)));
        if (m_order->getOrderProperty(ORDER_ADVANCE_CHECK).toInt())
            m_order->nop(0, "");
        else
            m_order->save(0, "");
    }
}

void DlgWorkspace::on_pushButton_6_clicked()
{
    bool btnVisible = true;
    if (ui->wdtTables->height() < 60) {
        ui->wdtTables->setMinimumHeight(255);
        ui->wdtTables->setMaximumHeight(255);
    }
    else {
        btnVisible = false;
        ui->wdtTables->setMinimumHeight(55);
        ui->wdtTables->setMaximumHeight(55);
    }
    ui->btnUpTables->setVisible(btnVisible);
    ui->btnUpUpTables->setVisible(btnVisible);
    ui->btnDownDownTables->setVisible(btnVisible);
    ui->btnDownTables->setVisible(btnVisible);
}

void DlgWorkspace::on_tblDish_clicked(const QModelIndex &index)
{
    if (m_data["table_mode"] != tmTable) {
        DlgMessageBox::show(tr("Complete removing or moving dish first"), 1, this);
        return;
    }

    if (!index.isValid())
        return;

    try {
        QMap<QString, QString> dish = ___menu.dish((index.row() * ui->tblDish->columnCount()) + index.column());
        if (dish[DISH_PRICE].toDouble() == 0) {
            double value;
            if (DlgInput::getDouble(value, tr("Price"), 100000, this))
                dish[DISH_PRICE] = QString("%1").arg(value);
            else
                return;
        }
        dish[DISH_LAST_USER] = ___staff.id();
        m_order->appendDish(dish);
        const QStringList &printers = ___menu.printSchema(dish);
        m_order->appendPrinterToLastDish(printers);
        m_order->afterChange();

        if (___menu.dishesCommentsSpecial(dish).count())
            ui->btnDishSpecialComment->click();
    }
    catch (...) {
        return;
    }
}

void DlgWorkspace::on_btnQtyReject_clicked()
{
    setDishModelEnable(true);
}

void DlgWorkspace::on_btnMenuUp_2_clicked()
{
    if (!ui->tblGroup->rowCount())
        return;
    ui->tblGroup->verticalScrollBar()->setValue(ui->tblGroup->verticalScrollBar()->value() - (ui->tblGroup->height() / ui->tblGroup->rowHeight(0)));
}

void DlgWorkspace::on_btnMenuDown_2_clicked()
{
    if (!ui->tblGroup->rowCount())
        return;
    ui->tblGroup->verticalScrollBar()->setValue(ui->tblGroup->verticalScrollBar()->value() + (ui->tblGroup->height() / ui->tblGroup->rowHeight(0)));
}

void DlgWorkspace::on_btnMenuUp_clicked()
{
    if (!ui->tblDish->rowCount())
        return;
    ui->tblDish->verticalScrollBar()->setValue(ui->tblDish->verticalScrollBar()->value() - (ui->tblDish->height() / ui->tblDish->rowHeight(0)));
}

void DlgWorkspace::on_btnMenuDown_clicked()
{
    if (!ui->tblDish->rowCount())
        return;
    ui->tblDish->verticalScrollBar()->setValue(ui->tblDish->verticalScrollBar()->value() + (ui->tblDish->height() / ui->tblDish->rowHeight(0)));
}

void DlgWorkspace::on_btnUpTables_clicked()
{
    if (!ui->tblTable->rowCount())
        return;
    ui->tblTable->verticalScrollBar()->setValue(ui->tblTable->verticalScrollBar()->value() - (ui->tblTable->height() / ui->tblTable->rowHeight(0)));
}

void DlgWorkspace::on_btnDownTables_clicked()
{
    if (!ui->tblTable->rowCount())
        return;
    ui->tblTable->verticalScrollBar()->setValue(ui->tblTable->verticalScrollBar()->value() + (ui->tblTable->height() / ui->tblTable->rowHeight(0)));
}

void DlgWorkspace::on_btnUpUpTables_clicked()
{
    QString hallName = m_hallFilter[TABLE_HALL_NAME];
    int index = m_availableHalls.indexOf(hallName);

    if (!m_availableHalls.count()) {
        DlgMessageBox::show(tr("No halls defined"), 1, this);
        return;
    }

    if (index < 0) {
        DlgMessageBox::show(tr("No such hall"), 1, this);
        return;
    }
    if (index > 0)
        hallName = m_availableHalls.at(index - 1);
    else
        hallName = m_availableHalls.at(m_availableHalls.count() - 1);

    m_hallFilter[TABLE_HALL_NAME] = hallName;
    ___hall.configureTableWidget(ui->tblTable, m_hallFilter);
}

void DlgWorkspace::on_btnDownDownTables_clicked()
{
    QString hallName = m_hallFilter[TABLE_HALL_NAME];
    int index = m_availableHalls.indexOf(hallName);
    if (!m_availableHalls.count()) {
        DlgMessageBox::show(tr("No halls defined"), 1, this);
        return;
    }

    if (index < 0) {
        DlgMessageBox::show(tr("No such hall"), 1, this);
        return;
    }

    if (index < m_availableHalls.count() - 1)
        hallName = m_availableHalls.at(index + 1);
    else
        hallName = m_availableHalls.at(0);

    m_hallFilter[TABLE_HALL_NAME] = hallName;
    ___hall.configureTableWidget(ui->tblTable, m_hallFilter);
}

void DlgWorkspace::on_tblGroupOfGroup_clicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    QTableWidgetItem *item = ui->tblGroupOfGroup->item(index.row(), index.column());
    if (!item)
        return;

    QString groupFilter = item->data(Qt::DisplayRole).toString();
    ___menu.configureGroupTable(ui->tblGroup, ___settings->toString(SETTINGS_MENU_NAME), groupFilter);
}

void DlgWorkspace::on_btnDishSpecialComment_clicked()
{
    int row;
    if (Toolkit::currentRow(ui->tableDishes, row) < 0)
        return;

    QStringList specialComments = ___menu.dishesCommentsSpecial(m_order->dish(ui->tableDishes->currentRow()));
    if (!specialComments.count())
        return;

    DlgDishComment *dishComment = new DlgDishComment(specialComments, this);
    if (dishComment->exec() == QDialog::Accepted)
        m_order->setDishComment(row, dishComment->result());
    delete dishComment;
}

void DlgWorkspace::on_btnMoveOrder_clicked()
{
    m_data["table_mode"] = tmMoveTable;
    setDishModelEnable(false);
    ui->tblTable->setEnabled(true);
    ui->wdtDishQty->setEnabled(false);
}

void DlgWorkspace::on_btnAdditional_clicked()
{
    QStringList menu;
    menu << tr("Print advance check");
    DlgList *d = new DlgList(this);
    d->setMenu(menu);
    if (d->exec() == QDialog::Accepted) {
        if (d->selectedItem() == menu.at(0)) {
            if (!m_order)
                return;
            ___progressDialog->show(tr("Printing"));
            m_order->setOrderProperty(ORDER_CHECKOUT_PRINTER, ___settings->toString(SETTINGS_CHECKOUT_PRINTER));
            m_order->setOrderProperty(ORDER_ADVANCE_CHECK, 1);
            m_order->printCheck(0, "");
        }
    }
    delete d;
}

void DlgWorkspace::takeDiscountElement(const QString &qName, const QXmlAttributes &atts)
{
    QXmlAttributes a = atts;
    if (qName == "mod") {
        //Если значение скидки или надбавки установлено в 0, вводим вручную
        if (a.value("VALUE_TYPE").toInt() == OM_VALUE_TYPE_AMOUNT)
            if (a.value("AMOUNT").toDouble() == 0) {
                double value = 0;
                if (DlgInput::getDouble(value, tr("Cost"), 1000000, this))
                    a.append("USER", "", "", QString("%1").arg(value));
                else
                    return;
            }
        if (a.value("VALUE_TYPE").toInt() == OM_VALUE_TYPE_PERCENT)
            if (a.value("PERCENT").toDouble() == 0) {
                double value = 0;
                if (DlgInput::getDouble(value, tr("Cost"), 1000000, this))
                    a.append("USER", "", "", QString("%1").arg(value));
                else
                    return;
            }

        m_order->setMod(a);
        return;
    }
}

void DlgWorkspace::endOfDiscountElement(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    if (code) {
        ___progressDialog->hide();
        DlgMessageBox::show(message, 1, this);
        return;
    }
    m_order->afterChange();
    m_order->save(0, "");
}

void DlgWorkspace::endChangeStaffQuery(int code, const QString &message)
{
    ___progressDialog->hide();
    if (!code) {
        ___staff = *(qobject_cast<StationAuth*>(sender()));
        m_order->setOrderProperty(ORDER_STAFF_ID, ___staff.id());
        m_order->setOrderProperty(ORDER_STAFF_NAME, ___staff.fullName());
        m_order->setOrderProperty(ORDER_CURRENT_STAFF_ID, ___staff.id());
        m_order->setOrderProperty(ORDER_CURRENT_STAFF_NAME, ___staff.fullName());
        m_order->afterChange();
        ui->tableOrderInfo->item(0, 2)->setData(Qt::DisplayRole, ___staff.fullName());
        return;
    }
    DlgMessageBox::show(message);
}

void DlgWorkspace::on_btnDiscount_clicked()
{
    QString code;
    DlgInput *d = new DlgInput(this);
    if (d->exec() == QDialog::Accepted)
        code = d->text();
    delete d;
    if (!code.length())
        return;
    ___progressDialog->show(tr("Inserting discount"));
    BaseNetwork *b = new BaseNetwork();
    b->appendData("sysop", "DISCOUNT");
    b->appendData("cafe", "1");
    b->appendData("order_id", m_order->getOrderProperty(ORDER_ID).toString());
    b->appendData("code", code);
    b->appendData("session", ___staff.session());
    connect(b, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(takeDiscountElement(QString,QXmlAttributes)));
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfDiscountElement(int,QString)));
    b->post();
}

void DlgWorkspace::on_tableDishes_clicked(const QModelIndex &index)
{
   on_tableDishes_currentCellChanged(index.row(), index.column(), -1, -1);
}

void DlgWorkspace::on_btnCangeOrderOwner_clicked()
{
    if (!m_order->getOrderProperty(ORDER_ID).toInt())
        return;
    if (!___staff.getWritePermission(PERMISSION_CHANGE_STAFF)) {
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

    ___progressDialog->show();
    StationAuth *newStaff = new StationAuth();
    connect(newStaff, SIGNAL(endOfQuery(int,QString)), SLOT(endChangeStaffQuery(int, QString)));
    newStaff->auth2(password, DBID);
}
