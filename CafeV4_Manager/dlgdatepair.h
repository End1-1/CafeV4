#ifndef DLGDATEPAIR_H
#define DLGDATEPAIR_H

#include <QDialog>

namespace Ui {
class DlgDatePair;
}

class DlgDatePair : public QDialog
{
    Q_OBJECT

public:
    explicit DlgDatePair(QWidget *parent = 0);
    ~DlgDatePair();
    static bool getDate(QDate &d1, QDate &d2, QWidget *parent = 0);

private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::DlgDatePair *ui;
};

#endif // DLGDATEPAIR_H
