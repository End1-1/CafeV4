#ifndef GEMPLOYES_H
#define GEMPLOYES_H

#include "qgrid.h"

class GEmployes : public QGrid
{
    Q_OBJECT
public:
    explicit GEmployes(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionCostum(int action);
    virtual void actionBeforeSelect();

private:
    void setPassword();
    void clearPassword();

};

#endif // GEMPLOYES_H
