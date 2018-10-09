#ifndef CEDITDIALOG_H
#define CEDITDIALOG_H

#include "cdatabase.h"
#include <QDialog>

namespace Ui {
class CEditDialog;
}

class CEditDialog : public QDialog
{
    Q_OBJECT

public:
    enum FIELDTYPE {FIELD_TEXT, FIELD_ID_TEXT};
    explicit CEditDialog(QWidget *parent = 0);
    ~CEditDialog();
    static bool setValue(const QString &primaryKeyName, const QString &primaryKeyValue, const QString &tableName, const QString &field, const QString &caption, QVariant &value, int type, const QString &dataMapTable, const QString &dataMapId, const QString &dataMapName);
private slots:
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    Ui::CEditDialog *ui;
    QString fPrimaryKeyName;
    QString fPrimaryKeyValue;
    QString fTableName;
    QString fField;
    QString fDataMapId;
    QString fDataMapName;
    QString fDataMapTable;
    QMap<QString, QString> fDataMap;
    CDatabase fDb;
};

#endif // CEDITDIALOG_H
