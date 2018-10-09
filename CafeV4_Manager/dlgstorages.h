#ifndef DLGSTORAGES_H
#define DLGSTORAGES_H

#include "qdlgquery.h"

namespace Ui {
class DlgStorages;
}

class DlgStorages : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgStorages(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgStorages();

private:
    Ui::DlgStorages *ui;
};

#endif // DLGSTORAGES_H
