#ifndef DLGMESSAGEBOX_H
#define DLGMESSAGEBOX_H

#include <QDialog>

namespace Ui {
class DlgMessageBox;
}

class DlgMessageBox : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgMessageBox(const QString &message, const int &level = 0, QWidget *parent = 0);
    ~DlgMessageBox();
    static int show(const QString &message, const int &level = 0, QWidget *parent = 0);
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::DlgMessageBox *ui;
};

#endif // DLGMESSAGEBOX_H
