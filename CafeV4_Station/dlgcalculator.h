#ifndef DLGCALCULATOR_H
#define DLGCALCULATOR_H

#include "core.h"

namespace Ui {
class DlgCalculator;
}

class DlgCalculator : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgCalculator(QWidget *parent = 0);
    ~DlgCalculator();
    
private slots:
    void on_btnOk_clicked();
    void on_btnCancel_clicked();

private:
    Ui::DlgCalculator *ui;

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // DLGCALCULATOR_H
