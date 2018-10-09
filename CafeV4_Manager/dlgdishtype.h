#ifndef DLGDISHTYPE_H
#define DLGDISHTYPE_H

#include "qdlgquery.h"

namespace Ui {
class DlgDishType;
}

class DlgDishType : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgDishType(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgDishType();

private:
    Ui::DlgDishType *ui;

protected:
    virtual void btnOK();
};

#endif // DLGDISHTYPE_H
