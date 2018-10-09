#ifndef NMENU_H
#define NMENU_H

#include "mdiwindow.h"
#include "mdibutton.h"

class CGridWidget;

class NMenu : public MdiWindow
{
    Q_OBJECT
private:
    MdiButton *fMdiButton;
    CGridWidget *fGrid;
public:
    NMenu(QWidget *parent = 0);
    ~NMenu();
};

#endif // NMENU_H
