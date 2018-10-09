#include "dlgabout.h"
#include "ui_dlgabout.h"
#include "../../common/qsystem.h"

DlgAbout::DlgAbout(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgAbout)
{
    ui->setupUi(this);

    ui->lbVersion->setText(getVersionString(qApp->applicationFilePath()));
    ui->lbImage->setPixmap(QPixmap(":/img/app.ico"));
}

DlgAbout::~DlgAbout()
{
    delete ui;
}

void DlgAbout::on_btnClose_clicked()
{
    accept();
}
