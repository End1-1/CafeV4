#include "ceditdialog.h"
#include "ui_ceditdialog.h"
#include "cgridwidget.h"

CEditDialog::CEditDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CEditDialog)
{
    ui->setupUi(this);
    fDb.setConnection(__DBPATH__, __DBUSER__, __DBPASS__, "lc_ctype=UTF8");
}

CEditDialog::~CEditDialog()
{
    delete ui;
}

bool CEditDialog::setValue(const QString &primaryKeyName, const QString &primaryKeyValue, const QString &tableName, const QString &field, const QString &caption, QVariant &value, int type,
                           const QString &dataMapTable, const QString &dataMapId, const QString &dataMapName)
{
    CEditDialog *d = new CEditDialog();
    d->fPrimaryKeyName = primaryKeyName;
    d->fPrimaryKeyValue = primaryKeyValue;
    d->fTableName = tableName;
    d->fField = field;
    d->fDataMapTable = dataMapTable;
    d->fDataMapId = dataMapId;
    d->fDataMapName = dataMapName;
    d->ui->lbName->setText(caption);
    d->ui->wText->setVisible(type == FIELD_TEXT);
    d->ui->wIdText->setVisible(type == FIELD_ID_TEXT);
    switch (type) {
    case FIELD_TEXT:
        d->ui->leText->setText(value.toString());
        break;
    case FIELD_ID_TEXT: {
        d->fDb.prepare("select " + dataMapId + ", " + dataMapName + " from " + dataMapTable);
        d->fDb.exec();
        for (int i = 0; i < d->fDb.fRawData.count(); i++) {
            d->fDataMap[d->fDb.fRawData.at(i).at(1).toString()] = d->fDb.fRawData.at(i).at(0).toString();
        }
        d->ui->leTextCode->setText(d->fDataMap[value.toString()]);
        d->ui->leTextName->setText(value.toString());
        break;
    }
    }
    bool result = d->exec() == QDialog::Accepted;
    if (result) {
        switch (type) {
        case FIELD_TEXT:
            value = d->ui->leText->text();
            break;
        }
    }
    delete d;
    return result;
}

void CEditDialog::on_btnCancel_clicked()
{
    reject();
}

void CEditDialog::on_btnOK_clicked()
{
    /* update master */
    fDb.prepare("update " + fTableName + " set " + fField + "=:" + fField + " where " + fPrimaryKeyName + "=:" + fPrimaryKeyName);
    fDb.bindValue(":" + fField, ui->leTextCode->text());
    fDb.bindValue(":" + fPrimaryKeyName, fPrimaryKeyValue);
    fDb.exec();
    /* update data map table */
    fDb.prepare("update " + fDataMapTable + " set " + fDataMapName + "=:" + fDataMapName + " where " + fDataMapId + "=:" + fDataMapId);
    fDb.bindValue(":" + fDataMapName, ui->leTextCode->text());
    fDb.bindValue(":" + fDataMapId, ui->leTextName->text());
    fDb.exec();
}
