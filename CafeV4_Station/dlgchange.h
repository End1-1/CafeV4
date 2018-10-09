#ifndef DLGCHANGE_H
#define DLGCHANGE_H

#include "core.h"

namespace Ui {
class DlgChange;
}

class DlgChange : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgChange(const QString &amount, QWidget *parent = 0);
    ~DlgChange();
    
private slots:
    void on_pushButton_clicked();

    void on_btn1_clicked();

    void on_btn2_clicked();

    void on_btn3_clicked();

    void on_btn4_clicked();

    void on_btn5_clicked();

    void on_btn6_clicked();

    void on_btn7_clicked();

    void on_btn8_clicked();

    void on_btn9_clicked();

    void on_btn0_clicked();

    void on_btnPoint_clicked();

    void on_btnClear_clicked();

    void on_leDisplay_textChanged(const QString &arg1);

    void on_leDisplay_returnPressed();

private:
    Ui::DlgChange *ui;
    void btnClick(QObject *o);
};

#endif // DLGCHANGE_H
