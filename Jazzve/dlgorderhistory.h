#ifndef DLGORDERHISTORY_H
#define DLGORDERHISTORY_H

#include "core.h"

namespace Ui {
class DlgOrderHistory;
}

class DlgOrderHistory : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgOrderHistory(const QString &orderid, const QString &session, QWidget *parent = 0);
    ~DlgOrderHistory();
    
private:
    Ui::DlgOrderHistory *ui;

private slots:
    void takeList(const QString &qName, const QXmlAttributes &atts);
    void endOfList(int code, const QString &message);
    void on_btnClose_clicked();
};

#endif // DLGORDERHISTORY_H
