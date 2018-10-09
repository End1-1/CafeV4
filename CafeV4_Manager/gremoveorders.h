#ifndef GREMOVEORDERS_H
#define GREMOVEORDERS_H

#include"qgrid.h"

class gremoveorders : public QGrid
{
    Q_OBJECT
public:
    explicit gremoveorders(QWidget *parent = 0);
    virtual void actionBeforeSelect();
    virtual bool actionFilter();
    virtual void actionEdit();

private:
    int addRow();

};

#endif // GREMOVEORDERS_H
