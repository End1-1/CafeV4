#ifndef GSETTINGS_H
#define GSETTINGS_H

#include "qgrid.h"

class GSettings : public QGrid
{
    Q_OBJECT

private:
    void showSalesSettings();

public:
    explicit GSettings(QWidget *parent = 0);
    virtual void actionBeforeSelect();
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionCostum(int action);

};

#endif // GSETTINGS_H
