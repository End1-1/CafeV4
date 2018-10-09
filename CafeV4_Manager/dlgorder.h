#ifndef DLGORDER_H
#define DLGORDER_H

#include "qbasesqlwindow.h"

namespace Ui {
class DlgOrder;
}

class DlgOrder : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit DlgOrder(const QString &id, QWidget *parent = 0);
    virtual void actionDelete();
    virtual void actionCostum(int action);
    virtual void actionSave();
    ~DlgOrder();

private:
    Ui::DlgOrder *ui;
    QString m_id;
    void loadOrder();
    void showHistory();
    void copyDishes();
};

#endif // DLGORDER_H
