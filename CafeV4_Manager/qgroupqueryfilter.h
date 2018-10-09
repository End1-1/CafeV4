#ifndef QGROUPQUERYFILTER_H
#define QGROUPQUERYFILTER_H

#include "qsqldrv.h"
#include <QDialog>

namespace Ui {
class QGroupQueryFilter;
}

typedef struct {
    QString valueTitle;
    QString valueName;
    QVariant value;
} BindValue;

class QGroupQueryFilter : public QDialog
{
    Q_OBJECT

public:
    explicit QGroupQueryFilter(QWidget *parent = 0);
    ~QGroupQueryFilter();
    QString m_sqlText;
    QString m_queryTitle;
    QList<BindValue> m_bindValues;

private slots:
    void on_btnCancel_clicked();
    void on_btnOk_clicked();

private:
    QSqlDrv *m_sqlDrv;
    Ui::QGroupQueryFilter *ui;
};

#endif // QGROUPQUERYFILTER_H
