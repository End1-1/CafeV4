#ifndef QPRINTSCHEMA_H
#define QPRINTSCHEMA_H

#include "qgrid.h"

class QPrintSchema : public QGrid
{
    Q_OBJECT
public:
    explicit QPrintSchema(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionCostum(int action);

private:
    void editPrintersList();

};

#endif // QPRINTSCHEMA_H
