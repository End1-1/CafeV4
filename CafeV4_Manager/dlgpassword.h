#ifndef DLGPASSWORD_H
#define DLGPASSWORD_H

#include <QDialog>

namespace Ui {
class DlgPassword;
}

class DlgPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgPassword(QWidget *parent = 0);
    ~DlgPassword();
    static bool GetPasswords(QString &p1, QString &p2, QWidget *parent = 0);
    
private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::DlgPassword *ui;
    QString m_userId;
};

#endif // DLGPASSWORD_H
