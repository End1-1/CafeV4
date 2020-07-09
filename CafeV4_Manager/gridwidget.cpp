#include "gridwidget.h"
#include "ui_gridwidget.h"

GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GridWidget)
{
    ui->setupUi(this);
}

GridWidget::~GridWidget()
{
    delete ui;
}
