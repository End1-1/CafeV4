#include "customtablewidget.h"

CustomTableWidget::CustomTableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setItemDelegate(new CustomItemDelegate(this));
}


CustomTableWidget::CustomItemDelegate::CustomItemDelegate(QTableWidget *tableWidget) :
    QItemDelegate(tableWidget)
{
    m_tableWidget = tableWidget;
}

void CustomTableWidget::CustomItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid())
        return;

    if (index.flags() & Qt::ItemIsUserCheckable) {
        QItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();

    QPen pen(Qt::NoPen);
    pen.setColor(Qt::black);
    painter->setPen(pen);

    QString text;
    switch (index.data(Qt::DisplayRole).type()) {
    case QVariant::Double:
        text = QString::number(index.data().toDouble(), 'f', index.data(Qt::UserRole).toInt());
        break;
    default:
        text = QString(index.data().toString());
        break;
    }

    QBrush brush(Qt::white);
    if (option.state & QStyle::State_Selected) {
        brush.setColor(Qt::blue);
        pen.setColor(Qt::white);
    } 
    painter->setBrush(brush);
    painter->drawRect(option.rect);

    QRect rText = option.rect;
    rText.adjust(5, 3, -2, -2);
    pen.setStyle(Qt::SolidLine);
    painter->setPen(pen);
    painter->drawText(rText, text);

    painter->restore();
}
