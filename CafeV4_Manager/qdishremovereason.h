#ifndef QDISHREMOVEREASON_H
#define QDISHREMOVEREASON_H

#include "qgrid.h"


class QDishRemoveReason : public QGrid
{

public:
    QDishRemoveReason(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
};

#endif // QDISHREMOVEREASON_H
