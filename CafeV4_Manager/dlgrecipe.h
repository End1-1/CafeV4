#ifndef DLGRECIPE_H
#define DLGRECIPE_H

#include "qbasesqlwindow.h"
#include <QLineEdit>

namespace Ui {
class DlgRecipe;
}



class DlgRecipe : public QBaseSqlWindow
{
    Q_OBJECT    

public:
    explicit DlgRecipe(int dishId, QWidget *parent = 0);
    ~DlgRecipe();
    virtual void actionNew();
    virtual void actionDelete();
    virtual void actionSave();
    virtual void actionCostum(int action);

private slots:
    void textChangedQty(const QString &text);
    void on_tblGoods_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);

private:
    Ui::DlgRecipe *ui;
    int m_dishId;
    void countTotal();
    void addRow();
    void addGoods();
    void copyRecipe();
    void pasteRecipe();

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);
};

#endif // DLGRECIPE_H
