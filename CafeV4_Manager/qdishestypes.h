#ifndef QDISHESTYPES_H
#define QDISHESTYPES_H

#include "qgrid.h"

class QDishesTypes : public QGrid
{
    Q_OBJECT
public:
    explicit QDishesTypes(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionBeforeSelect();
    virtual void actionCostum(int action);

private:
    void editImage();

};

#endif // QDISHESTYPES_H
