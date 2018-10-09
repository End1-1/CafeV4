#ifndef DLGCARDREGISTER_H
#define DLGCARDREGISTER_H

#include "qdlgbase.h"

namespace Ui {
class DlgCardRegister;
}

class DlgCardRegister : public QDlgBase
{
    Q_OBJECT

public:
    explicit DlgCardRegister(QWidget *parent = 0);
    ~DlgCardRegister();

private slots:
    void on_leCode_returnPressed();
    void on_btnNext_clicked();

private:
    Ui::DlgCardRegister *ui;
    QMap<QString, QString> m_cardMod;
    virtual void btnOK();
};

#endif // DLGCARDREGISTER_H
