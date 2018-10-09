#ifndef DLGLOGIN_H
#define DLGLOGIN_H

#include <QDialog>
#include "core.h"
#include "qsqldrv.h"

namespace Ui {
class dlgLogin;
}

class dlgLogin : public QDialog
{
    Q_OBJECT

public:
    explicit dlgLogin(QWidget *parent = 0);
    ~dlgLogin();

private:
    QMap<int, QMap<QString,QString> > m_db;
    Ui::dlgLogin *ui;
    void loginOnServer();
    QSqlDrv *m_sqlDrv;

private slots:
    void cbDatabaseIndexChanged(int index);
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_leLogin_returnPressed();
    void on_lePassword_returnPressed();
    void on_btnShowPassword_pressed();
    void on_btnShowPassword_released();

protected:
    virtual void keyPressEvent(QKeyEvent *e);
};

#endif // DLGLOGIN_H
