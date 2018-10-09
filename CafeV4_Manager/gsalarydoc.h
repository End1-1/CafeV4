#ifndef GSALARYDOC_H
#define GSALARYDOC_H

#include "qgrid.h"

class GSalaryDoc : public QGrid
{
    Q_OBJECT
public:
    explicit GSalaryDoc(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionBeforeSelect();

signals:

public slots:

};

#endif // GSALARYDOC_H
