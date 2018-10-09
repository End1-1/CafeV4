#ifndef QGROUPQUERYBINDVALUES_H
#define QGROUPQUERYBINDVALUES_H

#include "qgroupqueryfilter.h"
#include <QDialog>

namespace Ui {
class QGroupQueryBindValues;
}

class QGroupQueryBindValues : public QDialog
{
    Q_OBJECT

public:
    explicit QGroupQueryBindValues(QList<BindValue> &values, QWidget *parent = 0);
    ~QGroupQueryBindValues();

private slots:
    void txtChange(const QString &v);
    void dateChanged(const QDate &d);
    void on_btnCancel_clicked();

    void on_btnOK_clicked();

private:
    QList<BindValue> &m_bindDst;
    QList<BindValue> m_bindTemp;
    QMap<QWidget*, BindValue*> m_bindMap;
    Ui::QGroupQueryBindValues *ui;
    void addRow(BindValue &bv);
};

#endif // QGROUPQUERYBINDVALUES_H
