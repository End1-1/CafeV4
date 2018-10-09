#include "dlgdishremovereason.h"
#include "ui_dlgdishremovereason.h"

DlgDishRemoveReason::DlgDishRemoveReason(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgDishRemoveReason)
{
    ui->setupUi(this);
}

DlgDishRemoveReason::~DlgDishRemoveReason()
{
    delete ui;
}
