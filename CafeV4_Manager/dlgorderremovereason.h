#ifndef DLGORDERREMOVEREASON_H
#define DLGORDERREMOVEREASON_H

#include <QDialog>

namespace Ui {
class DlgOrderRemoveReason;
}

class DlgOrderRemoveReason : public QDialog
{
    Q_OBJECT

public:
    explicit DlgOrderRemoveReason(QWidget *parent = 0);
    ~DlgOrderRemoveReason();
    static bool reason(QString &reason, QWidget *parent = 0);
private slots:
    void on_btnCancel_clicked();

    void on_btnOk_clicked();

private:
    Ui::DlgOrderRemoveReason *ui;
};

#endif // DLGORDERREMOVEREASON_H
