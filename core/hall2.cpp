#include "hall2.h"
#include "hall.h"
#include <QHeaderView>
#include <QPainter>
#include <QTime>
#include "connection.h"
#include "definitions.h"
#include "toolkit.h"
#include <QLayout>
#include <QPushButton>

Hall2::Hall2()
{
    init();
}

Hall2::~Hall2()
{
    m_timer.stop();
//    clearTable();
}

QStringList Hall2::namesHalls()
{
    QStringList list;
    for (QList<Hall*>::const_iterator it = m_halls.begin(); it != m_halls.end(); it++)
        list.append((*it)->toString("NAME"));
    return list;
}

void Hall2::loadData()
{
    BaseNetwork *bn = new BaseNetwork();
    connect(bn, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(continueStartElement(QString,QXmlAttributes)));
    connect(bn, SIGNAL(endOfQuery(int, QString)), SLOT(endOfLoad(int,QString)));
    bn->appendData("sysop", "HALL2");
    bn->appendData("su", "1");
    bn->appendData("cafe", "1");
    bn->appendData("host", Connection::hostName() + ":" + Connection::userName());
    bn->appendData("session", m_session);
    bn->appendData("halllist", m_avaiableHall);
    bn->post();
}

void Hall2::setSession(const QString &session)
{
    m_session = session;
}

void Hall2::setTable(QTableWidget *table)
{
    m_table = table;
    int colWidth = 150;
    int colCount = (m_table->width() - 4) / colWidth;
    int colWidthCorrect = ((m_table->width() - 4) - (colWidth * colCount)) / colCount;
    colWidth += colWidthCorrect;
    m_table->setColumnCount(colCount);
    m_table->horizontalHeader()->setDefaultSectionSize(colWidth);

    int rowHeight = 60;
    m_table->verticalHeader()->setDefaultSectionSize(rowHeight);
    m_table->setItemDelegate(new Hall2::TablesDelegate(this));
    connect(m_table, SIGNAL(clicked(QModelIndex)), SLOT(tableClicked(QModelIndex)), Qt::UniqueConnection);
}

void Hall2::setHallFilter(const QString &hallName)
{
    m_currentHall = hallName;
    filterTablesByHall();
}

void Hall2::setHallPanel(QWidget *panel)
{
    m_hallPanel = panel;
}

void Hall2::setAvailableHall(const QString &hallList)
{
    m_avaiableHall = hallList;
}

bool Hall2::getTable(const QString &id, Table &table)
{
    for (QList<Table*>::iterator it = m_tables.begin(); it != m_tables.end(); it++)
        if ((*it)->toString(TABLE_ID) == id) {
            table = **it;
            return true;
        }
    return false;
}

bool Hall2::getTable(const int &tableIndex, Hall2::Table &table)
{
    if (tableIndex > m_tables.count() - 1)
        return false;
    table = *m_tables[tableIndex];
    return true;
}

void Hall2::simpleTable(QTableWidget *table)
{
    if (!table)
        return;

    table->clearContents();
    int colWidth = 50, rowHeight = 50;
    int colCount = (table->width() - 4) / colWidth;
    int rowCount = m_tables.count() / colCount;
    if (m_tables.count() % colCount)
        rowCount++;
    int colWidthCorrect = ((table->width() - 4) - (colWidth * colCount)) / colCount;
    colWidth += colWidthCorrect;

    table->horizontalHeader()->setDefaultSectionSize(colWidth);
    table->verticalHeader()->setDefaultSectionSize(rowHeight);
    if (table->colorCount() != colCount)
        table->setColumnCount(colCount);
    if (table->rowCount() != rowCount)
        table->setRowCount(rowCount);

    int col = 0, row = 0, i = 1;
    for (QList<Table *>::const_iterator it = m_tables.begin(); it != m_tables.end(); it++) {
        QTableWidgetItem *item = new QTableWidgetItem((*it)->toString(TABLE_NAME));
        item->setData(Qt::UserRole, i++);
        table->setItem(row, col++, item);
        if (col >= colCount) {
            col = 0;
            row++;
        }
    }
}

int Hall2::pCount()
{
    return m_busyTableCount;
}

double Hall2::pAmount()
{
    return m_totalAmount;
}

void Hall2::setBusyFilter()
{
    m_onlyBusy = !m_onlyBusy;
    filterTablesByHall();
    refreshTable();
}

bool Hall2::busyFilter()
{
    return m_onlyBusy;
}

void Hall2::setCurrentHallFilter()
{
}

void Hall2::refreshTable()
{
    int rowCount = m_proxyTables.count() / m_table->columnCount();
    if (m_proxyTables.count() % m_table->columnCount())
        rowCount++;
    m_table->setRowCount(rowCount);

    for (int i = 0, cols = m_table->columnCount(); i < cols; i++)
        for (int j = 0, rows = m_table->rowCount(); j < rows; j++) {
            m_table->setItem(j, i, new QTableWidgetItem());
            m_table->item(j, i)->setData(Qt::UserRole, -1);
        }

    m_totalAmount = 0;
    m_busyTableCount = 0;
    int row = 0, col = 0, index = 0;
    for (QList<Table *>::const_iterator it = m_proxyTables.begin(); it != m_proxyTables.end(); it++) {
        if ((*it)->toInt(TABLE_ORDER_ID)) {
            m_totalAmount += (*it)->toDouble(TABLE_ORDER_AMOUNT);
            m_busyTableCount++;
        }
        m_table->item(row, col++)->setData(Qt::UserRole, index++);
        if (col > m_table->columnCount() - 1) {
             col = 0;
             row++;
        }
    }
    emit tableRefreshed();
}

void Hall2::init()
{
    m_table = 0;
    m_hallPanel = 0;
   // connect(&m_timer, SIGNAL(timeout()), this, SLOT(refreshData()));
}

void Hall2::makeHall()
{
    if (!m_hallPanel)
        return;

    QLayoutItem *child;
    while ((child = m_hallPanel->layout()->takeAt(0)))
        delete child;

    QPushButton *btn = new QPushButton("*");
    btn->setMinimumSize(QSize(60, 60));
    btn->setMaximumSize(QSize(60, 60));
    connect(btn, SIGNAL(clicked()), SLOT(btnHallFilterClick()));
    m_hallPanel->layout()->addWidget(btn);

    for (QList<Hall*>::const_iterator it = m_halls.begin(); it != m_halls.end(); it++) {
        btn = new QPushButton((*it)->toString("NAME"));
        btn->setMinimumSize(QSize(60, 60));
        btn->setMaximumSize(QSize(60, 60));
        connect(btn, SIGNAL(clicked()), SLOT(btnHallFilterClick()));
        m_hallPanel->layout()->addWidget(btn);
    }

    m_hallPanel->layout()->addItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
}

void Hall2::filterTablesByHall()
{
    m_proxyTables.clear();
    bool byHall = m_currentHall.length();

    for (QList<Table*>::const_iterator it = m_tables.begin(); it != m_tables.end(); it++) {
        bool ok = true;
        if (byHall)
            ok = ok && ((*it)->toString(TABLE_HALL_NAME) == m_currentHall);
        if (m_onlyBusy)
            ok = ok && (*it)->toInt(TABLE_ORDER_ID);
        if (ok)
            m_proxyTables.append(*it);
    }
    refreshTable();
}

void Hall2::continueStartElement(const QString &qName, const QXmlAttributes &atts)
{
    if (qName == "HALL") {
        Hall *h = new Hall();
        h->fromXml(atts);
        m_halls.append(h);
        return;
    }

    if (qName == "TABLE") {
        Table *t = new Table();
        t->fromXml(atts);
        m_tables.append(t);
        m_proxyTables.append(t);
        return;
    }
}

void Hall2::refreshData()
{
    BaseNetwork *bn = new BaseNetwork();
    connect(bn, SIGNAL(takeElement(QString, QXmlAttributes)), this, SLOT(takeRefreshData(QString, QXmlAttributes)));
    connect(bn, SIGNAL(endOfQuery(int, QString)), this, SLOT(endOfRefreshData(int, QString)));
    bn->appendData("sysop", "HALL2");
    bn->appendData("su", "1");
    bn->appendData("cafe", "1");
    bn->appendData("host", Connection::hostName() + ":" + Connection::userName());
    bn->appendData("session", m_session);
    bn->appendData("halllist", m_avaiableHall);
    bn->post();
}


void Hall2::tableClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;

    int elementIndex =  index.data(Qt::UserRole).toInt();

    if (elementIndex < 0)
        return;

    Table *t = m_proxyTables[elementIndex];
    emit tableClick(t->toString(TABLE_ID), t->toString(TABLE_NAME));
}

void Hall2::endOfLoad(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    makeHall();
    refreshData();
    m_timer.start(10000);
}

void Hall2::takeRefreshData(const QString &qName, const QXmlAttributes &attr)
{
    if (qName == "TABLE") {
        for (QList<Table *>::iterator it = m_tables.begin(); it != m_tables.end(); it++)
            if ((*it)->toString(TABLE_ID) == attr.value(TABLE_ID))
                (*it)->fromXml(attr);
    }
}

void Hall2::endOfRefreshData(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    refreshTable();
}

void Hall2::btnHallFilterClick()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    setHallFilter(btn->text() == "*" ? "" : btn->text());
}


void Hall2::TablesDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{

    if (!index.isValid())
        return;

    if (!m_hall->m_proxyTables.count())
        return;

    int elementIndex = index.data(Qt::UserRole).toInt();
    if (elementIndex < 0)
        return;

    Hall2::Table *t = m_hall->m_proxyTables[elementIndex];
    QFont font(painter->font());

    QPen pen;
    pen.setStyle(Qt::NoPen);
    painter->setPen(pen);

    QBrush brush(Qt::white);
    painter->setBrush(brush);

    QRect rect = option.rect;
    painter->drawRect(rect);


    //Table name
    QColor cellColor = Qt::white;
    if (t->toInt(TABLE_ORDER_ID))
        cellColor = QColor::fromRgb(9, 255, 7);

    if (t->toInt(TABLE_ORDER_CHECK_QTY) != 0)
        cellColor = QColor::fromRgb(255, 243, 28);

    if (t->toInt(TABLE_ORDER_ID)) {
        if (t->toString(TABLE_RESERVED).length()) {
            cellColor = QColor::fromRgb(156, 200, 226);
            if (t->toDateTime(TABLE_DATE_OPEN).addDays(1) < QDateTime::currentDateTime())
                cellColor = QColor::fromRgb(243, 203, 141);
        }

        if (t->toString(TABLE_LOCK_HOST).length())
            if (t->toString(TABLE_LOCK_HOST) != Connection::lockName())
                cellColor = QColor::fromRgb(255, 13, 13);
    }

    pen.setStyle(Qt::SolidLine);
    pen.setColor(QColor::fromRgb(192, 192, 192));
    painter->setPen(pen);

    rect.adjust(1, 1, -1, -1);
    brush.setColor(cellColor);
    painter->setBrush(brush);
    painter->drawRect(rect);

    font.setPointSize(18);
    painter->setFont(font);
    font.setBold(true);
    painter->setFont(font);
    pen.setColor(Qt::white);
    painter->setPen(pen);
    painter->drawText(rect.left() + 5, rect.top() + 22, t->toString(TABLE_NAME));
    pen.setColor(Qt::black);
    painter->setPen(pen);
    painter->drawText(rect.left() + 3, rect.top() + 20, t->toString(TABLE_NAME));

    font.setPointSize(12);
    font.setBold(false);
    painter->setFont(font);
    //Date open
    QString str;
    if (t->toString(TABLE_DATE_OPEN).length()) {
        QDateTime d = QDateTime::fromString(t->toString(TABLE_DATE_OPEN), DATETIME_FORMAT);
        int top = rect.top();
        if (d.date() < QDate::currentDate()) {
            str = d.toString("dd.MM.yyyy");
            font.setPointSize(8);
            top += QFontMetrics(font).height() - 2;
            painter->setFont(font);
            painter->drawText(rect.right() - QFontMetrics(font).width(str) - 2, top, str);
            top += QFontMetrics(font).height() + 3;
        } else
            top += QFontMetrics(font).height();
        font.setPointSize(16);
        painter->setFont(font);
        str = d.toString("HH:mm");
        painter->drawText(rect.right() - QFontMetrics(font).width(str) - 2, top, str);
    }

    //Amount
    QString amount;
    if(t->toDouble(TABLE_ORDER_AMOUNT) > 0) {
        font.setBold(false);
        painter->setFont(font);
        amount = Toolkit::formatDouble(t->toDouble(TABLE_ORDER_AMOUNT));
        painter->setFont(font);
        pen.setColor(Qt::white);
        painter->setPen(pen);
        painter->drawText(rect.right() - QFontMetrics(font).width(amount), rect.bottom() - 1, amount);
        pen.setColor(Qt::black);
        painter->setPen(pen);
        painter->drawText(rect.right() - QFontMetrics(font).width(amount) - 1, rect.bottom() - 2, amount);
    }

    //Comment
    QTextOption to;
    to.setWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    if (t->toString(TABLE_ORDER_COMMENT).length()) {
        font.setPointSize(12);
        painter->setFont(font);
        rect.setLeft(rect.left() + 2);
        rect.setTop(rect.bottom()  - QFontMetrics(font).height() + 1);
        rect.setRight(rect.right() - QFontMetrics(font).width(amount) - 10);
        painter->drawText(rect, t->toString(TABLE_ORDER_COMMENT), to);
    }

    //Client card number
    if (t->toString(TABLE_RESERVED).length()) {
        font.setPointSize(10);
        painter->setFont(font);
        rect = option.rect;
        rect.adjust(4, 20, 0, 0);
        painter->drawText(rect, t->toString(TABLE_RESERVED), to);
    } else {
        font.setPointSize(10);
        painter->setFont(font);
        rect = option.rect;
        rect.adjust(4, 25, 0, 0);
        painter->drawText(rect, t->toString(TABLE_STAFF_NAME), to);
    }
}

Hall2::TablesDelegate::TablesDelegate(Hall2 *h)
{
    m_hall = h;
}
