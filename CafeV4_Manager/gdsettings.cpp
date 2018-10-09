#include "gdsettings.h"
#include "ui_gdsettings.h"

GDSettings::GDSettings(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::GDSettings)
{
    m_tableName = "SYS_SETTINGS";
    ui->setupUi(this);
}

GDSettings::~GDSettings()
{
    delete ui;
}
