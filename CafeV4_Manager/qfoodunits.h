#ifndef QFOODUNITS_H
#define QFOODUNITS_H

#include "qgrid.h"

class QFoodUnits : public QGrid
{
    Q_OBJECT
public:
    explicit QFoodUnits(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();

signals:

public slots:

};

#endif // QFOODUNITS_H
