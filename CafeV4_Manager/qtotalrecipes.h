#ifndef QTOTALRECIPES_H
#define QTOTALRECIPES_H

#include "qgrid.h"

class QTotalRecipes : public QGrid
{
    Q_OBJECT
public:
    explicit QTotalRecipes(QWidget *parent = 0);
    virtual void actionEdit();
    virtual void actionBeforeSelect();
    virtual void actionSelect();
    virtual void actionCostum(int action);
    virtual void actionDelete();

private slots:
    virtual void slotGridHeaderClicked(int index);

private:
    void addRow(QTableWidget *t, int &r);
    void updateGoodsPrice();
    bool getRowTopBottom(int curr, int &top, int &bottom);
    void updateSelfCost(int t, int b);

};

#endif // QTOTALRECIPES_H
