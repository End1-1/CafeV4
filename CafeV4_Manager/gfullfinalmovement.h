#ifndef GFULLFINALMOVEMENT_H
#define GFULLFINALMOVEMENT_H

#include "qgrid.h"
#include <QTableWidgetItem>

class GFullFinalMovement : public QGrid
{
    Q_OBJECT

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
    explicit GFullFinalMovement(QWidget *parent = 0);
    virtual void actionBeforeSelect();
    virtual void actionCostum(int action);

signals:

public slots:

private:
    QMap<QString, int> m_dataFields;
    QMap<int, int> m_goodsRows;
    QMap<int, float> m_prices;
    void viewPrices();
    void setOptions();
    void applyOptions();
    void getAllStores();
    void fetchRow(QTableWidget *t, int col, int sign = 1);
    void checkRow(QTableWidget *t, int goodsId, const QString &goodsName);

};

#endif // GFULLFINALMOVEMENT_H
