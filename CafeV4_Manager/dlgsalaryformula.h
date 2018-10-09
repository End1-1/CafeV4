#ifndef DLGSALARYFORMULA_H
#define DLGSALARYFORMULA_H

#include "qbasesqlwindow.h"
#include "dlgaddsalaryfunction.h"

#define FUNCTION_ID "FUNC"
#define FUNCTION_NAME "NAME"

namespace Ui {
class DlgSalaryFormula;
}

class DlgSalaryFormula : public QBaseSqlWindow
{
    Q_OBJECT
    
public:
    explicit DlgSalaryFormula(const QString &postId, const QString &postName, QWidget *parent = 0);
    ~DlgSalaryFormula();
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionCostum(int action);
    virtual void actionSave();
    
private:
    Ui::DlgSalaryFormula *ui;
    QString m_postId;
    QMap<int, DlgAddSalaryFunction::Function> m_functions;
    void getFormulaData();
    void functionToTable(int row, const DlgAddSalaryFunction::Function &f);
    void replaceRows(int r1, int r2);
    void rowUp();
    void rowDown();

private slots:
    void on_table_doubleClicked(const QModelIndex &index);
};

#endif // DLGSALARYFORMULA_H
