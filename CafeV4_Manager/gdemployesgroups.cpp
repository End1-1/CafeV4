#include "gdemployesgroups.h"
#include "ui_gdemployesgroups.h"

GDEmployesGroups::GDEmployesGroups(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::GDEmployesGroups)
{
    ui->setupUi(this);
}

GDEmployesGroups::~GDEmployesGroups()
{
    delete ui;
}
