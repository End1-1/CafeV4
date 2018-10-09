#ifndef DLGPRINTEROFSCHEMA_H
#define DLGPRINTEROFSCHEMA_H

#include "qdlgbase.h"

namespace Ui {
class DlgPrinterOfSchema;
}

class DlgPrinterOfSchema : public QDlgBase
{
    Q_OBJECT

public:
    explicit DlgPrinterOfSchema(QString lastWorkstation, QWidget *parent = 0);
    ~DlgPrinterOfSchema();
    QString m_lastWorkstation;
    QString m_printer;

private slots:
    void on_cbWorkstation_currentTextChanged(const QString &arg1);

private:
    Ui::DlgPrinterOfSchema *ui;
    QMap<QString, QStringList> m_printers;
    virtual void btnOK();
};

#endif // DLGPRINTEROFSCHEMA_H
