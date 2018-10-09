#ifndef CFILTERDIALOG_H
#define CFILTERDIALOG_H

#include <QDialog>

namespace Ui {
class CFilterDialog;
}

class CFilterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CFilterDialog(const QString &title, QWidget *parent = 0);
    ~CFilterDialog();
    bool isEmpty();
    void clearValues();
    void setFilterValues(const QList<QStringList> &values);
    bool getFilter();
    QStringList fResult;
    virtual int exec();
private slots:
    void on_tblData_cellDoubleClicked(int row, int column);

    void on_leSearch_textChanged(const QString &arg1);

    void on_btnUncheck_clicked();

    void on_btnApply_clicked();

    void on_btnRefresh_clicked();

private:
    Ui::CFilterDialog *ui;
    void getResult();
};

#endif // CFILTERDIALOG_H
