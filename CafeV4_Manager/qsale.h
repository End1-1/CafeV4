#ifndef QSALE_H
#define QSALE_H

#include "qgrid.h"

class QSale : public QGrid
{
    Q_OBJECT
public:
    explicit QSale(QWidget *parent = 0);
    virtual void actionEdit();
    virtual void actionBeforeSelect();
    virtual void actionDelete();
    virtual void actionCostum(int action);
    virtual bool actionFilter();

protected:
    virtual void getPrintData(XmlPrintMaker &x);

private:
    void correctStore();
    void viewDishParams();

private slots:
    void slotContextMenu(const QPoint &pos);
    void slotCopyContextMenuToClipboard();

};

#endif // QSALE_H
