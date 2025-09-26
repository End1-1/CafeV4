#ifndef MONTHLYSELFCOST_H
#define MONTHLYSELFCOST_H

#include "qgrid.h"

class MonthlySelfcost : public QGrid
{
    class ItemDelegate : public QItemDelegate {
    protected:
        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

    class IntTableWidgetItem : public QTableWidgetItem {
    public:
        IntTableWidgetItem(int value) : QTableWidgetItem(){setData(Qt::DisplayRole, value);}
        bool operator < (const QTableWidgetItem &item) const {
            return text().toInt() < item.text().toInt();
        }
    };

    class DoubleTableWidgetItem : public QTableWidgetItem {
    public:
        DoubleTableWidgetItem(double value) : QTableWidgetItem(){setData(Qt::DisplayRole, value);}
        bool operator < (const QTableWidgetItem &item) const {
            return text().toFloat() < item.text().toFloat();
        }
    };

public:
    MonthlySelfcost();
    virtual void actionBeforeSelect() override;
private:
    QMap<QString, int> m_dataFields;
    QMap<int, int> m_goodsRows;
    QMap<int, float> m_prices;
    void fetchRow(QTableWidget *t, int col, int sign = 1);
    void checkRow(QTableWidget *t, int goodsId, const QString &goodsName);
};

#endif // MONTHLYSELFCOST_H
