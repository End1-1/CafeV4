#include "imenuitem.h"
#include "ui_imenuitem.h"

IMenuItem::IMenuItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IMenuItem)
{
    ui->setupUi(this);
}

IMenuItem::~IMenuItem()
{
    delete ui;
}
