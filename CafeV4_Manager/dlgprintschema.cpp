#include "dlgprintschema.h"
#include "ui_dlgprintschema.h"

DlgPrintSchema::DlgPrintSchema(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgPrintSchema)
{
    ui->setupUi(this);
}

DlgPrintSchema::~DlgPrintSchema()
{
    delete ui;
}
