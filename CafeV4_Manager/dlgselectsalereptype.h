#ifndef DLGSELECTSALEREPTYPE_H
#define DLGSELECTSALEREPTYPE_H

#include <QDialog>

namespace Ui {
class DlgSelectSaleRepType;
}

class DlgSelectSaleRepType : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSelectSaleRepType(QWidget *parent = 0);
    ~DlgSelectSaleRepType();

private slots:
    void on_btNormalReport_clicked();

    void on_btnTotalView_clicked();

private:
    Ui::DlgSelectSaleRepType *ui;
};

#endif // DLGSELECTSALEREPTYPE_H
