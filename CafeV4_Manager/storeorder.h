#ifndef STOREORDER_H
#define STOREORDER_H

#include "qbasesqlwindow.h"
#include "core.h"
#include "dockidname.h"

namespace Ui {
class StoreOrder;
}

class StoreOrder : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit StoreOrder(const QString &docId, QWidget *parent  = 0);
    explicit StoreOrder(QWidget *parent = 0);
    ~StoreOrder();    
    void setId(const QString &id);
    virtual void actionSave();
    virtual void actionPrint();
    virtual void actionPrintPreview();
    virtual void actionExcel();
    void getPrintable(XmlPrintMaker &x);

private:
    struct Food {
        int id;
        QString name;
        QString unit;
    };

    DockIdName *m_dock;
    QWidget *m_tableCellEdit;
    QMap<int, Food> m_foods;
    QString m_docNum;
    Ui::StoreOrder *ui;
    void getFoodList();
    void getFoodData(int row, double &qty, double &price, double &amount);
    void setFoodData(int row, double &qty, double &price, double &amount);
    void countAmount();
    bool checkDoc();
    void loadDoc();

protected:
    bool eventFilter(QObject *object, QEvent *event);

private slots:
    void dockAccepted(int id);
    void codeTextChanged(const QString &text);
    void nameTextChanged(const QString &text);
    void qtyTextChanged(const QString &text);
    void priceTextChanged(const QString &text);
    void amountTextChanged(const QString &text);
    void on_btnAppendDish_clicked();
    void on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_cbAction_currentIndexChanged(int index);
    void on_btnRemoveDish_clicked();
};

#endif // STOREORDER_H
