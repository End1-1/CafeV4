#ifndef DLGGOODSUNITS_H
#define DLGGOODSUNITS_H

#include "qdlgquery.h"

namespace Ui {
class DlgGoodsUnits;
}

class DlgGoodsUnits : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgGoodsUnits(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgGoodsUnits();

private:
    Ui::DlgGoodsUnits *ui;
};

#endif // DLGGOODSUNITS_H
