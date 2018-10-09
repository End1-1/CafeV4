#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include <QTableWidget>
#include <QItemDelegate>
#include <QPainter>

class CustomTableWidget : public QTableWidget
{
    Q_OBJECT

    class CustomItemDelegate : public QItemDelegate {
        QTableWidget *m_tableWidget;
    public:
        CustomItemDelegate(QTableWidget *tableWidget);
    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

public:
    CustomTableWidget(QWidget *parent = 0);
};

#endif // CUSTOMTABLEWIDGET_H
