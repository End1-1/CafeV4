#ifndef GTABLETCONTROL_H
#define GTABLETCONTROL_H

#include "qbasesqlwindow.h"

namespace Ui {
class GTabletControl;
}

class GTabletControl : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit GTabletControl(QWidget *parent = 0);
    ~GTabletControl();

private:
    Ui::GTabletControl *ui;
};

#endif // GTABLETCONTROL_H
