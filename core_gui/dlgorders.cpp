#include "dlgorders.h"
#include "ui_dlgorders.h"

DlgOrders::DlgOrders(StationAuth &user, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgOrders),
    m_user(user)
{
    ui->setupUi(this);
    showFullScreen();
    ui->tblOrders->setColumnCount((ui->tblOrders->width() - 5) / ui->tblOrders->horizontalHeader()->defaultSectionSize());
    ui->tblOrders->setItemDelegate(new OrdersPaintDelegate(m_orders));
    getOrders();
    m_order = 0;
    ui->tblOrder->setColumnWidth(0, 370);
    ui->tblOrder->setColumnWidth(1, 80);
}

DlgOrders::~DlgOrders()
{
    delete ui;
}

void DlgOrders::takeOrder(const QString &qName, const QXmlAttributes &atts)
{
    if (qName == "order") {
        BaseObject o;
        for (int i = 0, count = atts.count(); i < count; i++)
            o.fromXml(atts);
        m_orders.append(o);
    }
}

void DlgOrders::on_pushButton_clicked()
{
    close();
}

void DlgOrders::getOrders()
{
    ___dlgProgress->show();
    BaseNetwork *b = new BaseNetwork();
    b->appendData("sysop", "ORDERS");
    b->appendData("su", "1");
    b->appendData("cafe_reports", "1");
    b->appendData("date", ___settings->cashDate());
    b->appendData("session", m_user.session());
    connect(b, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(takeOrder(QString,QXmlAttributes)));
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfOrder(int,QString)));
    b->post();
}

void DlgOrders::setTableCell(int &col, int &row, QTableWidgetItem *item)
{
    ui->tblOrders->setItem(row, col++, item);
    if (col >= ui->tblOrders->columnCount()) {
        col = 0;
        row++;
    }
}

int DlgOrders::getTableIndex(const QString &id)
{
    for (int i = 0, count = m_orders.count(); i < count; i++)
        if (m_orders[i].toString("ID") == id)
            return i;
    return -1;
}


void DlgOrders::endOfOrder(int code, const QString &message)
{
    ___dlgProgress->hide();
    BaseNetwork::removeInstance(sender());
    if (code) {
        DlgMessageBox(message, 1, this);
        return;
    }

    int rowCount = m_orders.count() / ui->tblOrders->columnCount();
    if (m_orders.count() % ui->tblOrders->columnCount())
        rowCount++;
    ui->tblOrders->setRowCount(rowCount);

    QMultiMap<int, BaseObject> o;
    for (QList<BaseObject>::const_iterator it = m_orders.begin(); it != m_orders.end(); it++)
        o.insert((*it).toString("QUEUE").toInt(), *it);

    int col = 0, row = 0;
    for (QMultiMap<int, BaseObject>::const_iterator it = o.begin(); it != o.end(); it++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::UserRole, getTableIndex(it.value().toString("ID")) + 1);
        ui->tblOrders->setItem(row, col++, item);
        if (col >= ui->tblOrders->columnCount()) {
            col = 0;
            row++;
        }
    }
}

void DlgOrders::endOfReadOrder(int code, const QString &message)
{
    if (code) {
        DlgMessageBox::show(message, 1, this);
        return;
    }
    m_order->afterChange();
}

void DlgOrders::on_tblOrders_clicked(const QModelIndex &index)
{
    if (!index.isValid() || !index.data(Qt::UserRole).toInt()) {
        ui->tblOrder->setItemDelegate(new QItemDelegate());
        ui->tblTotal->setItemDelegate(new QItemDelegate());
        ui->tblOrder->setRowCount(0);
        ui->tblTotal->setRowCount(0);
        ui->tblTotal->clearContents();
        ui->tblOrder->clearContents();
        delete m_order;
        m_order = 0;
        return;
    }

    if (!m_order) {
        m_order = new Order(m_user.session());
        m_order->appendWidget("table", ui->tblOrder);
        m_order->appendWidget("btn_checkout_print", ui->btnCheckout);
        m_order->appendWidget("payment_display", ui->tblTotal);
        connect(m_order, SIGNAL(endOfQuery(int,QString)), SLOT(endOfReadOrder(int,QString)));
    }

    int orderId = m_orders[index.data(Qt::UserRole).toInt() - 1].toInt("ID");

    m_order->setOrderProperty(PERMISSION_PRINT_CHECK_MORE_THEN_ONE, m_user.getWritePermission(PERMISSION_PRINT_CHECK_MORE_THEN_ONE));
    m_order->setOrderProperty(ORDER_NO_CLEAR_ID, 1);
    m_order->setOrderProperty(ORDER_ID, orderId);
    m_order->putVoid(SLOT(read(int,QString)));
    m_order->newOrder(0, "");
}

void DlgOrders::on_btnCheckout_clicked()
{
    if (m_order) {
        m_order->setOrderProperty(ORDER_CHECKOUT_PRINTER, ___settings->toString(SETTINGS_CHECKOUT_PRINTER));
        m_order->printCheck(0, "");
    }
}


DlgOrders::OrdersPaintDelegate::OrdersPaintDelegate(QList<BaseObject> &orders) :
    m_orders(orders)
{
}

void DlgOrders::OrdersPaintDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    int tableIndex = index.data(Qt::UserRole).toInt() - 1;
    if (tableIndex < 0)
        return;

    if (option.state & QStyle::State_Selected) {
        QBrush brush(QColor::fromRgb(213, 255, 255));
        painter->setBrush(brush);
        painter->drawRect(option.rect);
    }

    BaseObject o = m_orders[tableIndex];

    QTextOption to;
    QFont font(APP_FONT_NAME, 12);
    font.setBold(true);
    painter->setFont(font);

    QRect rect = option.rect;
    rect.adjust(1, 1, -80, -30);
    painter->drawText(rect, o.toString("TABLE_NAME"));

    font.setBold(false);
    font.setPointSize(10);
    painter->setFont(font);
    rect = option.rect;
    rect.adjust(40, 1, -1, -30);
    QDateTime time = o.toDateTime("DATE_CLOSE");
    to.setAlignment(Qt::AlignRight);
    painter->drawText(rect, time.toString("HH:mm"), to);

    font.setPointSize(12);
    font.setBold(true);
    painter->setFont(font);
    rect = option.rect;
    rect.adjust(1, 18, -1, -1);
    painter->drawText(rect, Toolkit::formatDouble(o.toDouble("AMOUNT")));

    font.setPointSize(9);
    font.setBold(false);
    painter->setFont(font);
    rect = option.rect;
    rect.adjust(1, 42, -1, -1);
    painter->drawText(rect, o.toString("STAFF_NAME"));
}
