#ifndef GGROUPQUERIES_H
#define GGROUPQUERIES_H

#include "qgrid.h"

class GGroupQueries : public QGrid
{

public:
    GGroupQueries(QWidget *parent = 0);
    virtual void actionBeforeSelect();
    virtual void actionNew();
    virtual void actionEdit();

};

#endif // GGROUPQUERIES_H
