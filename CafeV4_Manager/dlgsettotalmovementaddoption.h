#ifndef DLGSETTOTALMOVEMENTADDOPTION_H
#define DLGSETTOTALMOVEMENTADDOPTION_H

#include <QDialog>

namespace Ui {
class DlgSetTotalMovementAddOption;
}

class DlgSetTotalMovementAddOption : public QDialog
{
    Q_OBJECT

public:
    explicit DlgSetTotalMovementAddOption(QWidget *parent = 0);
    ~DlgSetTotalMovementAddOption();
    static int getResult(QList<bool> &result, QWidget *parent);

private slots:
    void on_btnCancel_clicked();
    void on_btnOk_clicked();

private:
    Ui::DlgSetTotalMovementAddOption *ui;
};

#endif // DLGSETTOTALMOVEMENTADDOPTION_H
