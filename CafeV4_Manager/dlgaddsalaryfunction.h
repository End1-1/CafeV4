#ifndef DLGADDSALARYFUNCTION_H
#define DLGADDSALARYFUNCTION_H

#include <QDialog>

namespace Ui {
class DlgAddSalaryFunction;
}

class DlgAddSalaryFunction : public QDialog
{
    Q_OBJECT
    
public:
    struct Function {
        int id;
        QString name;
        QString desc;
        QString params;
        QString cell;
    };

    explicit DlgAddSalaryFunction(const QMap<int, Function> &functions, QWidget *parent = 0);
    ~DlgAddSalaryFunction();
    void setData(const Function &f);
    void getData(Function &f);
    
private slots:
    void on_btnCancel_clicked();
    void on_btnOk_clicked();
    void on_cbFunction_currentIndexChanged(int index);

private:
    Ui::DlgAddSalaryFunction *ui;
    const QMap<int, Function> &m_functions;
};

#endif // DLGADDSALARYFUNCTION_H
