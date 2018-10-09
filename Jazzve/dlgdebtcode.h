#ifndef DLGDEBTCODE_H
#define DLGDEBTCODE_H

#include "core.h"

namespace Ui {
class DlgDebtCode;
}

class DlgDebtCode : public QDialog
{
    Q_OBJECT
    
public:
    enum Mode {mInsert, mRemove};
    explicit DlgDebtCode(const Mode &mode, const QString &tableName, const QString &session, QWidget *parent = 0);
    ~DlgDebtCode();
    
private slots:
    void takeDebtCheck(const QString &qName, const QXmlAttributes &atts);
    void endDebtCheck(int code, const QString &message);
    void takeInsertDebt(const QString &qName, const QXmlAttributes &atts);
    void endInsertDebt(int code, const QString &message);
    void endRemoveDebt(int code, const QString &message);
    void on_leCustomer_returnPressed();
    void on_btnCancel_clicked();
    void on_btnOk_clicked();
    void on_leKey_editingFinished();
    void on_leKey_returnPressed();

private:
    Ui::DlgDebtCode *ui;
    QString m_session;
    Mode m_mode;
    bool correctData();

protected:
    void keyPressEvent(QKeyEvent *e);
};

#endif // DLGDEBTCODE_H
