#ifndef QSTORAGES_H
#define QSTORAGES_H

#include "qgrid.h"

class QStorages : public QGrid
{
    Q_OBJECT
public:
    explicit QStorages(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();

signals:

public slots:

};

#endif // QSTORAGES_H
