#ifndef DLGDISHGROUPOFGROUP_H
#define DLGDISHGROUPOFGROUP_H

#include "qdlgquery.h"

namespace Ui {
class DlgDishGroupOfGroup;
}

class DlgDishGroupOfGroup : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgDishGroupOfGroup(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgDishGroupOfGroup();

private:
    Ui::DlgDishGroupOfGroup *ui;
};

#endif // DLGDISHGROUPOFGROUP_H
