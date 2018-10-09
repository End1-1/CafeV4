#ifndef DLGSIMPLESELECT_H
#define DLGSIMPLESELECT_H

#include <QDialog>

namespace Ui {
class DlgSimpleSelect;
}

class DlgSimpleSelect : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgSimpleSelect(QWidget *parent = 0);
    ~DlgSimpleSelect();
    static bool getValue(QWidget *parent, const QString &caption, const QMap<QString, QString> &data, QString &valueStr, QString &valueVar);
    
private slots:
    void on_btnCancel_clicked();
    void on_btnOk_clicked();

private:
    Ui::DlgSimpleSelect *ui;
};

#endif // DLGSIMPLESELECT_H
