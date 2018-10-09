#ifndef QMENUNAMES_H
#define QMENUNAMES_H

#include "qgrid.h"

class QMenuNames : public QGrid
{
    Q_OBJECT
public:
    explicit QMenuNames(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionBeforeSelect();
    virtual void actionCostum(int action);

private:
    void copyMenu();

signals:

public slots:

};

#endif // QMENUNAMES_H
