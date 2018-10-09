#ifndef DLGCHANGEPASSWORD_H
#define DLGCHANGEPASSWORD_H

#include "core.h"

namespace Ui {
class DlgChangePassword;
}

class DlgChangePassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgChangePassword(const QString &session, QWidget *parent = 0);
    ~DlgChangePassword();
    
private slots:
    void takeEmployee(const QString &qName, const QXmlAttributes &atts);
    void endOfEmployeeQuery(int code, const QString &message);
    void endOfPasswordQuery(int code, const QString &message);
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_2_clicked();
    void on_btnChangePassword_clicked();

private:
    Ui::DlgChangePassword *ui;
    int m_row;
    int m_col;
    int m_colCount;
    QString m_session;
};

#endif // DLGCHANGEPASSWORD_H
