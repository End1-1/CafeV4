#ifndef DLGHALL_H
#define DLGHALL_H

#include "qdlgquery.h"

namespace Ui {
class DlgHall;
}

class DlgHall : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgHall(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgHall();

private:
    Ui::DlgHall *ui;
};

#endif // DLGHALL_H
