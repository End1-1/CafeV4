#ifndef DLGDISCOUNTCARD_H
#define DLGDISCOUNTCARD_H

#include "qdlgquery.h"

namespace Ui {
class DlgDiscountCard;
}

class DlgDiscountCard : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgDiscountCard(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgDiscountCard();

private:
    Ui::DlgDiscountCard *ui;

protected:
    virtual void btnOK();
private slots:
    void on_leId_textChanged(const QString &arg1);
};

#endif // DLGDISCOUNTCARD_H
