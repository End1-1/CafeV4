#ifndef HALL2_H
#define HALL2_H

#include "basenetwork.h"
#include "baseobject.h"
#include <QTableWidget>
#include <QItemDelegate>
#include <QTimer>

#define TABLE_ORDER_CHECK_QTY "PRINT_QTY"
#define TABLE_ORDER_AMOUNT "AMOUNT"
#define TABLE_ORDER_COMMENT "COMMENT"
#define HALL_NAME "NAME"
#define TABLE_RESERVED "RESERVED"
#define TABLE_STAFF_NAME "STAFF_NAME"

class Hall2 : public QObject
{
    Q_OBJECT

    class TablesDelegate : public QItemDelegate {
    private:
        Hall2 *m_hall;
        friend class Hall2;
    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    public:
        explicit TablesDelegate(Hall2 *h);
    };

public:

    typedef BaseObject Table;
    typedef BaseObject Hall;

    Hall2();
    ~Hall2();

    QStringList namesHalls();
    void loadData();
    void setSession(const QString &session);
    void setTable(QTableWidget *table);
    void setHallFilter(const QString &hallName);
    void setHallPanel(QWidget *panel);
    void setAvailableHall(const QString &hallList);
    bool getTable(const QString &id, Table &table);
    bool getTable(const int &tableIndex, Table &table);
    void simpleTable(QTableWidget *table);
    int pCount();
    double pAmount();
    void setBusyFilter();
    bool busyFilter();
    void setCurrentHallFilter();
    void refreshTable();

private:
    QTableWidget *m_table;
    QWidget *m_hallPanel;
    QString m_currentHall;
    QString m_avaiableHall;
    QTimer m_timer;
    bool m_onlyBusy;
    QString m_session;
    QList<Hall *> m_halls;
    QList<Table *> m_tables;
    QList<Table *> m_proxyTables;
    void init();
    void makeHall();
    void filterTablesByHall();
    double m_totalAmount;
    int m_busyTableCount;

private slots:
    void tableClicked(const QModelIndex &index);
    void endOfLoad(int code, const QString &message);
    void takeRefreshData(const QString &qName, const QXmlAttributes &attr);
    void endOfRefreshData(int code, const QString &message);
    void btnHallFilterClick();
    void continueStartElement(const QString &qName, const QXmlAttributes &atts);

public slots:
    void refreshData();

signals:
    void tableClick(const QString &tableCode, const QString &tableName);
    void tableRefreshed();
};

#endif // HALL2_H
