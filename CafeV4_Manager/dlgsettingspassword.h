#ifndef DLGSETTINGSPASSWORD_H
#define DLGSETTINGSPASSWORD_H

#include <QDialog>

namespace Ui {
class dlgSettingsPassword;
}

class dlgSettingsPassword : public QDialog
{
    Q_OBJECT
    
public:
    explicit dlgSettingsPassword(const QString &pass, QWidget *parent = 0);
    ~dlgSettingsPassword();
    
private slots:
    void on_btnCancel_clicked();
    void on_btnOk_clicked();

private:
    Ui::dlgSettingsPassword *ui;
    QString m_pass;
};

#endif // DLGSETTINGSPASSWORD_H
