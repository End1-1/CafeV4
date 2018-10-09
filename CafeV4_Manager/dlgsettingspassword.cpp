#include "dlgsettingspassword.h"
#include "ui_dlgsettingspassword.h"
#include "core.h"
#include <QMessageBox>

dlgSettingsPassword::dlgSettingsPassword(const QString &pass, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgSettingsPassword),
    m_pass(pass)
{
    ui->setupUi(this);
}

dlgSettingsPassword::~dlgSettingsPassword()
{
    delete ui;
}

void dlgSettingsPassword::on_btnCancel_clicked()
{
    reject();
}

void dlgSettingsPassword::on_btnOk_clicked()
{
    if (m_pass.compare(ui->lnSettingsPassword->text()))  {
        QMessageBox::critical(this, tr("Error"), tr("Invalid password"));
        return;
    }

    accept();
}
