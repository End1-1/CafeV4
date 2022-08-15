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
    explicit DlgOrder(QWidget *parent = 0);
    void setId(const QString &id);
    virtual void actionDelete();
    virtual void actionCostum(int action);
    virtual void actionSave();
    ~DlgOrder();

private slots:
    void on_tabWidget_currentChanged(int index);

private:
    Ui::DlgOrder *ui;
    QString m_id;
    void loadOrder();
    void showHistory();
    void copyDishes();
    void loadHistory();
};

#endif // DLGORDER_H
