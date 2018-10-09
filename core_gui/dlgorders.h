#ifndef DLGORDERS_H
#define DLGORDERS_H

#include "core.h"

namespace Ui {
class DlgOrders;
}

class DlgOrders : public QDialog
{
    Q_OBJECT

    class OrdersPaintDelegate : public QItemDelegate {
    private:
        QList<BaseObject> &m_orders;

    public:
        OrdersPaintDelegate(QList<BaseObject> &orders);

    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
    
public:
    explicit DlgOrders(StationAuth &user, QWidget *parent = 0);
    ~DlgOrders();
    
private slots:
    void takeOrder(const QString &qName, const QXmlAttributes &atts);
    void endOfOrder(int code, const QString &message);
    void endOfReadOrder(int code, const QString &message);
    void on_pushButton_clicked();
    void on_tblOrders_clicked(const QModelIndex &index);
    void on_btnCheckout_clicked();

private:
    StationAuth &m_user;
    QList<BaseObject> m_orders;
    Order *m_order;
    Ui::DlgOrders *ui;
    void getOrders();
    void setTableCell(int &col, int &row, QTableWidgetItem *item);
    int getTableIndex(const QString &id);
};

#endif // DLGORDERS_H
