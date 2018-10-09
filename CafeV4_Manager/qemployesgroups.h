#ifndef QEMPLOYESGROUPS_H
#define QEMPLOYESGROUPS_H

#include "qgrid.h"

class QEmployesGroups : public QGrid
{
    Q_OBJECT
public:
    explicit QEmployesGroups(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionBeforeSelect();
    virtual void actionCostum(int action);

private:
    void accessControl();
    void salary();

};

#endif // QEMPLOYESGROUPS_H
