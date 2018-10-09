#ifndef NRECIPES_H
#define NRECIPES_H

#include "mdiwindow.h"
#include "mdibutton.h"

class CGridWidget;

class nrecipes : public MdiWindow
{
    Q_OBJECT
    MdiButton *fMdiButton;
    CGridWidget *fGrid;
public:
    nrecipes(QWidget *parent = 0);
};

#endif // NRECIPES_H
