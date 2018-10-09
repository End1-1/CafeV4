#ifndef QFOODS_H
#define QFOODS_H

#include "qgrid.h"

class QFoods : public QGrid
{
    Q_OBJECT
public:
    explicit QFoods(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionBeforeSelect();
    virtual void actionCostum(int action);

private:
    void updatePrices();
    QDate date1;
    QDate date2;

};

#endif // QFOODS_H
