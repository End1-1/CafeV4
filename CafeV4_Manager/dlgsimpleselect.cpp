#include "dlgsimpleselect.h"
#include "ui_dlgsimpleselect.h"
#include <QMessageBox>

DlgSimpleSelect::DlgSimpleSelect(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgSimpleSelect)
{
    ui->setupUi(this);
}

DlgSimpleSelect::~DlgSimpleSelect()
{
    delete ui;
}

bool DlgSimpleSelect::getValue(QWidget *parent, const QString &caption, const QMap<QString, QString> &data, QString &valueStr, QString &valueVar)
{
    bool result = false;
    DlgSimpleSelect *dlg = new DlgSimpleSelect(parent);
    dlg->ui->lbCaption->setText(caption);
    dlg->setWindowTitle(caption);
    QMap<QString, QString> postreverse;
    for (QMap<QString, QString>::const_iterator it = data.begin(); it != data.end(); it++) {
        postreverse[it.value()] = it.key();
    }
    for (QMap<QString, QString>::const_iterator it = postreverse.begin(); it != postreverse.end(); it++) {
        dlg->ui->cbData->addItem(it.key(), it.value());
    }
    dlg->ui->cbData->setCurrentIndex(-1);
    if (dlg->exec() == QDialog::Accepted) {
        valueStr = dlg->ui->cbData->currentText();
        valueVar = dlg->ui->cbData->itemData(dlg->ui->cbData->currentIndex()).toString();
        result = true;
    }
    delete dlg;
    return result;
}

void DlgSimpleSelect::on_btnCancel_clicked()
{
    reject();
}

void DlgSimpleSelect::on_btnOk_clicked()
{
    if (ui->cbData->currentIndex() < 0) {
        QMessageBox::critical(this, tr("Error"), tr("Nothing is selected"));
        return;
    }

    accept();
}
