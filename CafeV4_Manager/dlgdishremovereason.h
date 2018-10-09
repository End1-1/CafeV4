#ifndef DLGDISHREMOVEREASON_H
#define DLGDISHREMOVEREASON_H

#include "qdlgquery.h"

namespace Ui {
class DlgDishRemoveReason;
}

class DlgDishRemoveReason : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgDishRemoveReason(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgDishRemoveReason();

private:
    Ui::DlgDishRemoveReason *ui;
};

#endif // DLGDISHREMOVEREASON_H
