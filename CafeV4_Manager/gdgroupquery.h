#ifndef GDGROUPQUERY_H
#define GDGROUPQUERY_H

#include "qdlgquery.h"

namespace Ui {
class GDGroupQuery;
}

class GDGroupQuery : public QDlgQuery
{
    Q_OBJECT

public:
    explicit GDGroupQuery(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~GDGroupQuery();

private:
    Ui::GDGroupQuery *ui;
};

#endif // GDGROUPQUERY_H
