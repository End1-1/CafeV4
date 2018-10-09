#ifndef HALL_H
#define HALL_H

#include "basenetwork.h"
#include <QItemDelegate>
#include <QTableWidget>
/*  Properties of table
    id
    hallId
    hallName
    name
    state
    orderId
    lockHost

    Filter names
    hall_id
    state */

#define TABLE_ID "TABLE_ID"
#define TABLE_NAME "TABLE_NAME"
#define TABLE_HALL_ID "HALL_ID"
#define TABLE_HALL_NAME "HALL_NAME"
#define TABLE_DATE_OPEN "DATE_OPEN"
#define TABLE_ORDER_ID "ORDER_ID"
#define TABLE_LOCK_HOST "LOCK_HOST"
#define TABLE_FLAG_FILTER "TABLE_FLAG_FILTER"
#define TABLE_STATE "STATE"
#define TABLE_QUEUE "QUEUE"
#define TABLE_PRINTED_QTY "PRINT_QTY"

#define TABLE_STATE_ALL ""
#define TABLE_STATE_EMPTY "0"
#define TABLE_STATE_OPENED "1"
#define TABLE_STATE_LOCKED "2"

class Hall : public BaseNetwork
{
    Q_OBJECT

public:
    typedef QMap<int, QMap<QString, QVariant> >::iterator Tables;
    typedef QMap<QString, QVariant> Table;
    typedef QStringList Halls;

    class TableItemDelegate : public QItemDelegate
    {
    private:
        const QMap<int, QMap<QString, QVariant> > &m_tables;
    public:
        TableItemDelegate(const QMap<int, QMap<QString, QVariant> > &tables, QObject *parent);
    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

private:
    QMap<QString, QString> m_filter;
    QMap<QString, QString> m_listHall;
    QMap<int, QMap<QString, QVariant> > m_tables;
    Tables m_table;
    QTableWidget *m_tablesWidget;
    bool checkFilter(Table &table);

protected:
    virtual bool continueStartElement(const QString &qName, const QXmlAttributes &atts);

public:
    explicit Hall();
    ~Hall();
    void post(const QString &session);
    int setFilter(const QMap<QString, QString> &filter);
    bool next();
    Table &table() const;
    Table &table(const int &queue);
    Halls halls();

    //Функция необходима для получения ключа в контейнере на основе идентификатора стола.
    int index(const QString &tableId);

    /* tableId на самом деле является номер очереди, для получения реального номера необходимо
     * сделать некоторые телодвижения. */
    QVariant tableValue(const int &tableId, const QString &valueName);
    void setTableValue(const int &tableId, const QString &valueName, const QString &value);
    void configureTableWidget(QTableWidget *tableWidget, const QMap<QString, QString> &filter);
    
signals:
    
public slots:
    void setTableState(const int &tableId, const QString &orderId, const int &printed);
};

#endif // HALL_H
