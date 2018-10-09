#ifndef DLGGOODS_H
#define DLGGOODS_H

#include "qdlgquery.h"

namespace Ui {
class DlgGoods;
}

class DlgGoods : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgGoods(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgGoods();
    virtual void setNext(bool next, const QString &code);

private:
    Ui::DlgGoods *ui;

protected:
    virtual void btnOK();
private slots:
    void on_chNewIdRezim_clicked(bool checked);
    void on_leName_textChanged(const QString &arg1);
    void on_pushButton_3_clicked();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_cbGroup_currentIndexChanged(int index);
    void on_cbUnit_currentIndexChanged(int index);
};

#endif // DLGGOODS_H
