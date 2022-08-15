#ifndef GRMENU_H
#define GRMENU_H

#include "gridwidget.h"

class GRMenu : public GridWidget
{
    Q_OBJECT

public:
    GRMenu();
    virtual void prepare() override;
};

#endif // GRMENU_H
