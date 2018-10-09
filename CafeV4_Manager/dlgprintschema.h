#ifndef DLGPRINTSCHEMA_H
#define DLGPRINTSCHEMA_H

#include "qdlgquery.h"

namespace Ui {
class DlgPrintSchema;
}

class DlgPrintSchema : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgPrintSchema(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgPrintSchema();

private:
    Ui::DlgPrintSchema *ui;
};

#endif // DLGPRINTSCHEMA_H
