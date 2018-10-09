#ifndef WDTCALCULATOR_H
#define WDTCALCULATOR_H

#include "core.h"
#include <QValidator>

class EditValidator : public QValidator {
    virtual State validate(QString &str, int &pos) const;

public:
    static bool decimalPoint(QString &str, int pos);
    static bool charOp(const QChar &chr);
};

namespace Ui {
class WdtCalculator;
}

class WdtCalculator : public QWidget
{
    Q_OBJECT
    
public:
    explicit WdtCalculator(QWidget *parent = 0);
    ~WdtCalculator();
    void calculate();
    void clear();
    
private:
    Ui::WdtCalculator *ui;

protected:
    bool eventFilter(QObject *o, QEvent *e);

private slots:
    void btnClick();
    void on_btnClear_clicked();
    void on_btn0_clicked();
    void on_btn1_clicked();
    void on_btn2_clicked();
    void on_btn3_clicked();
    void on_btn4_clicked();
    void on_btn5_clicked();
    void on_btn6_clicked();
    void on_btn7_clicked();
    void on_btn8_clicked();
    void on_btn9_clicked();
    void on_btnPlus_clicked();
    void on_btnMinus_clicked();
    void on_btnMult_clicked();
    void on_btnDiv_clicked();
    void on_btnCalc_clicked();
};

#endif // WDTCALCULATOR_H
