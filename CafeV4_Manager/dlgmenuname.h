#ifndef DLGMENUNAME_H
#define DLGMENUNAME_H

#include "qdlgquery.h"

namespace Ui {
class DlgMenuName;
}

class DlgMenuName : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgMenuName(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgMenuName();

private:
    Ui::DlgMenuName *ui;
};

#endif // DLGMENUNAME_H
