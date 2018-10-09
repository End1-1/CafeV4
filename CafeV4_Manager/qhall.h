#ifndef QHALL_H
#define QHALL_H

#include "qgrid.h"

class QHall : public QGrid
{
    Q_OBJECT
public:
    explicit QHall(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();

signals:

public slots:

};

#endif // QHALL_H
