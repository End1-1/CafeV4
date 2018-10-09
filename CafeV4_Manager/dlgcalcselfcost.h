#ifndef DLGCALCSELFCOST_H
#define DLGCALCSELFCOST_H

#include <QDialog>
#include "core.h"

namespace Ui {
class DlgCalcSelfCost;
}

class DlgCalcSelfCost : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCalcSelfCost(QWidget *parent = 0);
    ~DlgCalcSelfCost();

private slots:
    void on_btnStart_clicked();
    void on_btnStop_clicked();

private:
    bool m_started;
    QSqlDrv *m_sqlDrv;
    Ui::DlgCalcSelfCost *ui;
    bool getPricesInRange(QMap<int, double> &data);
    void updateGoods(int goodsid, double price);
    void updatePrices_1();
    void updatePrices_2();
    void finish();
};

#endif // DLGCALCSELFCOST_H
