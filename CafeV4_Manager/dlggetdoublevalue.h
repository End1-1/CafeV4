#ifndef DLGGETDOUBLEVALUE_H
#define DLGGETDOUBLEVALUE_H

#include <QDialog>

namespace Ui {
class DlgGetDoubleValue;
}

class DlgGetDoubleValue : public QDialog
{
    Q_OBJECT

public:
    explicit DlgGetDoubleValue(QWidget *parent = 0);
    ~DlgGetDoubleValue();
    static bool getDoubleValue(double &value, const QString &caption, QWidget *parent = 0);

private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::DlgGetDoubleValue *ui;
};

#endif // DLGGETDOUBLEVALUE_H
