#include "gtabletcontrol.h"
#include "ui_gtabletcontrol.h"

GTabletControl::GTabletControl(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::GTabletControl)
{
    ui->setupUi(this);
}

GTabletControl::~GTabletControl()
{
    delete ui;
}
