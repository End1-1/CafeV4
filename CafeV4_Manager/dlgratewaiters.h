#ifndef DLGRATEWAITERS_H
#define DLGRATEWAITERS_H

#include <QDialog>

namespace Ui {
class DlgRateWaiters;
}

class DlgRateWaiters : public QDialog
{
    Q_OBJECT

public:
    explicit DlgRateWaiters(QWidget *parent = nullptr);
    ~DlgRateWaiters();
    static void getRates(const QStringList &names, QList<int> &rates);

private slots:
    void on_tbl_cellChanged(int row, int column);

private:
    Ui::DlgRateWaiters *ui;
};

#endif // DLGRATEWAITERS_H
