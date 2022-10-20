#ifndef STAFFFOOD_H
#define STAFFFOOD_H

#include "qbasesqlwindow.h"

namespace Ui {
class StaffFood;
}

class StaffFood : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit StaffFood(QWidget *parent = nullptr);
    ~StaffFood();
    virtual void actionSave();

private:
    Ui::StaffFood *ui;

private slots:
    void qtyTextChanged(const QString &v);
};


#endif // STAFFFOOD_H
