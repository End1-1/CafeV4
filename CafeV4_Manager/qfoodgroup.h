#ifndef QFOODGROUP_H
#define QFOODGROUP_H

#include "qgrid.h"

class QFoodGroup : public QGrid
{
    Q_OBJECT
public:
    explicit QFoodGroup(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();

signals:

public slots:

};

#endif // QFOODGROUP_H
