#ifndef DLGFOODGROUPS_H
#define DLGFOODGROUPS_H

#include "qdlgquery.h"

namespace Ui {
class DlgFoodGroups;
}

class DlgFoodGroups : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgFoodGroups(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgFoodGroups();

private:
    Ui::DlgFoodGroups *ui;

protected:
    virtual void btnOk();
};

#endif // DLGFOODGROUPS_H
