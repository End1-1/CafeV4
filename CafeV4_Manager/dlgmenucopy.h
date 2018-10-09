#ifndef DLGMENUCOPY_H
#define DLGMENUCOPY_H

#include <QDialog>

namespace Ui {
class DlgMenuCopy;
}

class DlgMenuCopy : public QDialog
{
    Q_OBJECT

public:
    explicit DlgMenuCopy(QWidget *parent = 0);
    ~DlgMenuCopy();

private slots:
    void on_btnClose_clicked();

    void on_btnStart_clicked();

private:
    Ui::DlgMenuCopy *ui;
};

#endif // DLGMENUCOPY_H
