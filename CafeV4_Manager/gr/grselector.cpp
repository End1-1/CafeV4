#include "grselector.h"
#include "ui_grselector.h"

GrSelector::GrSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrSelector)
{
    ui->setupUi(this);
}

GrSelector::~GrSelector()
{
    delete ui;
}
