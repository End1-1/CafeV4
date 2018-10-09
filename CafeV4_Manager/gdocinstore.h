#ifndef GDOCINSTORE_H
#define GDOCINSTORE_H

#include "qgrid.h"

class GDocInStore : public QGrid
{
    Q_OBJECT
public:
    explicit GDocInStore(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionBeforeSelect();
    virtual void actionEdit();
    virtual void actionDelete();

signals:

public slots:

};

#endif // GDOCINSTORE_H
