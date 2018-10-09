#include "hall.h"
#include "connection.h"
#include "definitions.h"
#include <QTextOption>
#include <QDateTime>
#include <QPainter>

Hall::Hall() :
    BaseNetwork()
{
}

Hall::~Hall()
{
}

void Hall::post(const QString &session)
{
    m_listHall.clear();
    m_tables.clear();
    m_table = 0;
    m_tablesWidget = 0;

    appendData("sysop", "HALL");
    appendData("cafe", "1");
    appendData("host", Connection::hostName() + ":" + Connection::userName());
    appendData("session", session);

    BaseNetwork::post();
}

int Hall::setFilter(const QMap<QString, QString> &filter)
{
    m_filter = filter;

    int count = 0;
    Tables table = m_tables.begin();
    m_table = m_tables.end();
    for (; table != m_tables.end(); table++) {
        table.value()[TABLE_FLAG_FILTER] = 0;
        if (!checkFilter(table.value()))
            continue;
        table.value()[TABLE_FLAG_FILTER] = 1;
        if (m_table == m_tables.end())
            m_table = table;
        count++;
    }
    return count;
}

bool Hall::next()
{
    m_table++;
    for (; m_table != m_tables.end(); m_table++)
        if (m_table.value()[TABLE_FLAG_FILTER] == 1)
            return true;

    return false;
}

Hall::Table &Hall::table() const
{
    return m_table.value();
}

Hall::Table &Hall::table(const int &queue)
{
    return m_tables[queue];
}

Hall::Halls Hall::halls()
{
    return m_listHall.values();
}

int Hall::index(const QString &tableId)
{
    Tables tables = m_tables.begin();
    for (; tables != m_tables.end(); tables++)
        if (tables.value()[TABLE_ID] == tableId)
            return tables.key();
    return 0;
}

QVariant Hall::tableValue(const int &tableId, const QString &valueName)
{
    if (!m_tables.contains(tableId))
        return "0";

    return m_tables[tableId][valueName];
}

void Hall::setTableValue(const int &tableId, const QString &valueName, const QString &value)
{
    m_tables[tableId][valueName] = value;
}

bool Hall::continueStartElement(const QString &qName, const QXmlAttributes &atts)
{
    if (!qName.compare("hall")) {
        m_listHall[atts.value("id")] = atts.value("name");
        return true;
    }

    if (!qName.compare("tables")) {
        QMap<QString, QVariant> table;
        int tableQueue = atts.value(TABLE_QUEUE).toInt();
        if (m_tables.keys().contains(tableQueue))
            table = m_tables[tableQueue];
        for (int i = 0; i < atts.count(); i++)
            table[atts.qName(i)] = atts.value(i);
        m_tables[atts.value(TABLE_QUEUE).toInt()] = table;
        return true;
    }

    return true;
}


Hall::TableItemDelegate::TableItemDelegate(const QMap<int, QMap<QString, QVariant> > &tables, QObject *parent) :
    QItemDelegate(parent),
    m_tables(tables)
{
}

void Hall::TableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;
    if (index.data(Qt::UserRole).toInt() < 1)
        return;

    Hall::Table table = m_tables[index.data(Qt::UserRole).toInt()];
    QRect rect = option.rect;
    rect.adjust(2, 2, 2, 2);

    QFont font(painter->font());

    QPen pen;
    pen.setColor(Qt::black);
    painter->setPen(pen);

    QLinearGradient gradient(rect.left(), rect.top(), rect.left(), rect.bottom());
    QColor startColor;
    startColor = startColor.fromRgb(255, 255, 255);
    QColor endColor;
    endColor = endColor.fromRgb(255, 242, 249);


    //Busy table
    if (table[TABLE_ORDER_ID] != "0") {
        startColor = startColor.fromRgb(183, 253, 193);
        endColor = endColor.fromRgb(133, 191, 96);
    }

    //Printed
    if (table[TABLE_PRINTED_QTY].toInt() > 0) {
        startColor = startColor.fromRgb(255, 255, 0);
        endColor = endColor.fromRgb(255, 255, 255);
    }

    //Locked table
    if (table[TABLE_STATE] == TABLE_STATE_LOCKED) {
        startColor = startColor.fromRgb(255, 200, 200);
        endColor = endColor.fromRgb(255, 90, 90);
    }

    if (option.state & QStyle::State_Selected) {
        startColor = startColor.fromRgb(252, 253, 198);
        endColor = endColor.fromRgb(253, 191, 196);
    }

    gradient.setColorAt(0, startColor);
    gradient.setColorAt(1, endColor);

    QBrush brush(gradient);
    painter->setBrush(brush);
    painter->drawRect(option.rect);

    QTextOption textOption;
    textOption.setAlignment(Qt::AlignCenter);
    font.setPointSize(16);
    textOption.setWrapMode(QTextOption::WordWrap);

    painter->setFont(font);
    painter->drawText(rect, index.data(Qt::DisplayRole).toString(), textOption);
    if (!index.data(Qt::UserRole).toInt())
        return;

    painter->drawText(option.rect, table[TABLE_NAME].toString(), textOption);

    if (table[TABLE_ORDER_ID] != "0") {
        QDateTime now = QDateTime::currentDateTime();
        QDateTime open = QDateTime::fromString(table[TABLE_DATE_OPEN].toString(), DATETIME_FORMAT);
        QTime t = QTime::fromString("00:00:00", TIME_FORMAT);
        int seconds = open.secsTo(now);
        t = t.addSecs(seconds);
        rect.adjust(0, QFontMetrics(font).height() + 1, 0, 0);
        font.setPointSize(10);
        painter->setFont(font);
        pen.setColor(Qt::white);
        painter->setPen(pen);
        QString totalTime = QString("%1:%2").arg((seconds / 60) / 60).arg((seconds / 60) % 60);
        painter->drawText(rect, totalTime, textOption);
        pen.setColor(Qt::black);
        painter->setPen(pen);
        rect.adjust(-2, -1, 0, 0);
        painter->drawText(rect, totalTime, textOption);
    }

}

void Hall::configureTableWidget(QTableWidget *tableWidget, const QMap<QString, QString> &filter)
{
    tableWidget->clear();
    int tablesCount = setFilter(filter);

    int colWidth = 80;
    int colCount = tableWidget->width() / colWidth;
    int colWidthMod = (tableWidget->width() - 10) - (colWidth * colCount);
    colWidth += (colWidthMod / colCount);

    int rowHeight = 50;
    int rowCount = tablesCount / colCount;
    if (tablesCount % colCount)
        rowCount++;

    tableWidget->setRowCount(rowCount);
    tableWidget->setColumnCount(colCount);
    if (!tablesCount)
        return;

    for (int i = 0; i < tableWidget->columnCount(); i++)
        tableWidget->setColumnWidth(i, colWidth);

    for (int i = 0; i < tableWidget->rowCount(); i++)
        tableWidget->setRowHeight(i, rowHeight);

    int col = 0, row = 0;
    do {
        QTableWidgetItem *i = new QTableWidgetItem();
        i->setFlags(i->flags() ^ Qt::ItemIsEditable);
        //Поиск нужного стола в списке осуществляется по индексу,
        //записанному в data(Qt::UserRole).Это order_id стола в таблице h_table.
        i->setData(Qt::UserRole, table()[TABLE_QUEUE].toInt());
        tableWidget->setItem(row, col, i);
        col++;
        if (col >= colCount) {
            col = 0;
            row++;
        }
    } while (next());

    tableWidget->setItemDelegate(new TableItemDelegate(m_tables, tableWidget));
    m_tablesWidget = tableWidget;
}

void Hall::setTableState(const int &tableId, const QString &orderId, const int &printed)
{
    if (!m_tablesWidget)
        return;

    int tableQueue = 0;
    for (QMap<int, QMap<QString, QVariant> >::iterator it = m_tables.begin(); it != m_tables.end(); it++)
        if (tableValue(it.key(), TABLE_ID).toInt() == tableId) {
            it.value()[TABLE_ORDER_ID] = orderId;
            it.value()[TABLE_PRINTED_QTY] = printed;
            tableQueue = it.key();
        }

    if (!tableQueue)
        return;

    for (int i = 0, rowcount = m_tablesWidget->rowCount(); i < rowcount; i++)
        for (int j = 0, colcount = m_tablesWidget->columnCount(); j < colcount; j++) {
            QTableWidgetItem *item = m_tablesWidget->item(i, j);
            if (!item)
                return;
            if (item->data(Qt::UserRole) == tableQueue) {
                m_tablesWidget->update(m_tablesWidget->model()->index(i, j));
                return;
            }
        }
}


bool Hall::checkFilter(Hall::Table &table)
{
    QMap<QString, QString>::const_iterator filter;
    for (filter = m_filter.begin(); filter != m_filter.end(); filter++)
        if ((filter.value() != "") && (table[filter.key()] != filter.value()))
            return false;
    return true;
}
