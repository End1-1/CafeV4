#ifndef NMENUNAMES_H
#define NMENUNAMES_H

#include "mdiwindow.h"
#include "mdibutton.h"

class CGridWidget;

class NMenuNames : public MdiWindow
{
    Q_OBJECT
    MdiButton *fMdiButton;
    CGridWidget *fGrid;
public:
    NMenuNames(QWidget *parent = 0);
};

#endif // NMENUNAMES_H
