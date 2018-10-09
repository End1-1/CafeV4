#ifndef DLGEMPLOYEE_H
#define DLGEMPLOYEE_H

#include "qdlgquery.h"

namespace Ui {
class DlgEmployee;
}

class DlgEmployee : public QDlgQuery
{
    Q_OBJECT

public:
    explicit DlgEmployee(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~DlgEmployee();

private slots:
    void on_comboBox_2_currentIndexChanged(int index);
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::DlgEmployee *ui;
};

#endif // DLGEMPLOYEE_H
