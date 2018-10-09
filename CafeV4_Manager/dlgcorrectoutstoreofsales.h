#ifndef DLGCORRECTOUTSTOREOFSALES_H
#define DLGCORRECTOUTSTOREOFSALES_H

#include "qsqldrv.h"
#include <QDialog>

namespace Ui {
class DlgCorrectOutStoreOfSales;
}

class DlgCorrectOutStoreOfSales : public QDialog
{
    Q_OBJECT

public:
    explicit DlgCorrectOutStoreOfSales(QWidget *parent = 0);
    ~DlgCorrectOutStoreOfSales();
    void setData(const QString &food, int oldStore, int cashId);
    void setDate(QDate d1, QDate d2);

private slots:
    void on_btnStart_clicked();

private:
    Ui::DlgCorrectOutStoreOfSales *ui;
    QSqlDrv *m_sqlDriver;
};

#endif // DLGCORRECTOUTSTOREOFSALES_H
