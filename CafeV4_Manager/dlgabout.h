#ifndef DLGABOUT_H
#define DLGABOUT_H

#include <QDialog>

namespace Ui {
class DlgAbout;
}

class DlgAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DlgAbout(QWidget *parent = 0);
    ~DlgAbout();
    
private slots:
    void on_btnClose_clicked();

private:
    Ui::DlgAbout *ui;
};

#endif // DLGABOUT_H