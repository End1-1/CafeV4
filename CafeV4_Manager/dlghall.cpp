#include "dlghall.h"
#include "ui_dlghall.h"

DlgHall::DlgHall(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgHall)
{
    ui->setupUi(this);
}

DlgHall::~DlgHall()
{
    delete ui;
}
