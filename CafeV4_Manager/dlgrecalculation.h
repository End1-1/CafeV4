#ifndef DLGRECALCULATION_H
#define DLGRECALCULATION_H

#include "qsqldrv.h"
#include <QDialog>

namespace Ui {
class DlgRecalculation;
}

class DlgRecalculation : public QDialog
{
    Q_OBJECT

private:
    struct Goods {
        float qty;
        float price;
    };
    
public:
    explicit DlgRecalculation(QWidget *parent = 0);
    ~DlgRecalculation();
    
private slots:
    void on_btnStart_clicked();

private:
    Ui::DlgRecalculation *ui;
    QSqlDrv *m_sqlDrv;
    void start();

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // DLGRECALCULATION_H
