#ifndef GDEMPLOYESGROUPS_H
#define GDEMPLOYESGROUPS_H

#include "qdlgquery.h"

namespace Ui {
class GDEmployesGroups;
}

class GDEmployesGroups : public QDlgQuery
{
    Q_OBJECT

public:
    explicit GDEmployesGroups(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~GDEmployesGroups();

private:
    Ui::GDEmployesGroups *ui;
};

#endif // GDEMPLOYESGROUPS_H
