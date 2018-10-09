#ifndef DLGORDER_H
#define DLGORDER_H

#include "core.h"

namespace Ui {
class DlgOrder;
}

class DlgOrder : public QDialog
{
    Q_OBJECT

private:
    bool m_flagClose;
    Order *m_order;
    StationAuth *m_staff;
    QString m_dishQty;
    QMap<QString, QVariant> m_data;
    void btnQtyClicked(QObject *button);
    void displayQtyToRemoveOrMovement(const QString &qty);
    void setControlEnabledForRemove(const bool &enable);
    
public:
    explicit DlgOrder(QWidget *parent = 0);
    void setMain(StationAuth *staff, const QString &tableId);
    ~DlgOrder();
    
private slots:
    void slotQtyText(QString &qty);
    void slotConfirmQtyDel();
    void slotConfirmQtyMovement();
    void slotQtyRemoveOrMovement(QString &qty);
    void takeTotalSale(const QString &qName, const QXmlAttributes &attr);
    void endOfTotalSale(int code, const QString &message);
    void orderStopped(int code, const QString &message);
    void takeDiscountElement(const QString &qName, const QXmlAttributes &atts);
    void endOfDiscountElement(int code, const QString &message);
    void endChangeStaffQuery(int code, const QString &message);
    void on_btnExit_clicked();
    void on_btnMenuDown_clicked();
    void on_btnMenuUp_clicked();
    void on_tbGroups_clicked(const QModelIndex &index);
    void on_tblGroupOfGroup_clicked(const QModelIndex &index);
    void on_tbDishes_clicked(const QModelIndex &index);
    void on_pushButton_18_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_15_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_16_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_17_clicked();
    void on_pushButton_19_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_20_clicked();
    void on_btnOrderDown_clicked();
    void on_btnOrderUp_clicked();
    void on_tbOrderDishes_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_btnDishComment_clicked();
    void on_btnQtyDel_clicked();
    void on_btnQtyReject_clicked();
    void on_btnQtyAccept_clicked();
    void on_btnPrintServiceCheck_clicked();
    void on_btnMoveOrder_clicked();
    void on_btnPayment_clicked();
    void on_btnRemoveWholeOrder_clicked();
    void on_btnCheckout_clicked();
    void on_btnMoveDish_clicked();
    void on_btnMenu_clicked();
    void on_btnDiscount_clicked();
    void on_btnDishListDown_clicked();
    void on_btnDishListUp_clicked();
    void on_tbOrderDishes_clicked(const QModelIndex &index);

private:
    Ui::DlgOrder *ui;
    void prepareForMovementOrRemoveDish(const char *slot);
    void setBtnQtyMoveEnabled(int row);

protected:
    void closeEvent(QCloseEvent *e);

signals:
    void qtyText(QString &text);
    void confirmQty();
};

#endif // DLGORDER_H
