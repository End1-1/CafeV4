#ifndef GDISCOUNTCARD_H
#define GDISCOUNTCARD_H

#include "qgrid.h"

class GDiscountCard : public QGrid
{
    Q_OBJECT

public:
    GDiscountCard(QWidget *parent = 0);
    virtual void actionBeforeSelect();
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionCostum(int a);
};

#endif // GDISCOUNTCARD_H
