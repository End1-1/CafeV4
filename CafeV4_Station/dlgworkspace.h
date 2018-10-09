#ifndef DLGWORKSPACE_H
#define DLGWORKSPACE_H

#include <QDialog>
#include <QModelIndex>
#include <QItemDelegate>
#include <QKeyEvent>
#include "core.h"

namespace Ui {
class dlgWorkspace;
}

class DlgWorkspace : public QDialog
{
    Q_OBJECT

    enum TableMode {tmHall = 1, tmTable, tmMenus, tmDishesTypes, tmDishes, tmMoveTable, tmMoveDish};

    class TableHeaderItemDelegate : public QItemDelegate {
    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

    class TableColDelegate : public QItemDelegate {
    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
    
public:
    explicit DlgWorkspace(QWidget *parent = 0);
    ~DlgWorkspace();

    void viewTables();
    void viewDishes(const QString &menuId, const QString &typeId);
    void scrollMainMenu(int direction);
    void btnQtyClicked(QObject *button);

private:
    QTimer m_timer;
    /* Заколебался писать QMap<..... */
    typedef QMap<QString, QString> Pairs;

    bool m_hallLoaded;
    bool m_menuLoaded;
    bool m_modLoaded;

    //Переменная необходима для задания количества блюда.
    QString m_dishQty;

    int m_loadedModulesCount;
    Pairs m_hallFilter;
    Pairs m_dishFilter;
    QStringList m_availableHalls;


    /* Контейнер для промежуточного хранения количеств перемещяемых
     * или удаляемых количеств, режима отображения меню. */
    QMap<QString, QVariant> m_data;

    Ui::dlgWorkspace *ui;
    Order *m_order;
    void checkLoadedModulesCount();
    void dishTypeSelected(const QString &groupName);
    void moveTable();
    void updateTableStatus(Order *order);

    /* Подготавливаемя к удалению или перемещению блюда. */
    void prepareToRemoveOrMovementDish(const char *confirmSlot);

    /* Отображение текущего количества перемещаемого или удаляемого количества. */
    void displayQtyToRemoveOrMovement(const QString &qty);

    void setDishModelEnable(const bool &visible);
    void configureTables();

    /* Ищем яйчейку в таблице по ID стола. */
    QModelIndex getTableIndex(const QVariant &tableId);

protected:
    void closeEvent(QCloseEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void slotTimeout();
    void slotUpdateTableStatus(int code, const QString &message);
    void slotHallLoaded(int code, const QString &message);
    void slotMenuLoaded(int code, const QString &message);
    void slotModLoaded(int code, const QString &message);
    void orderStopped(int code, const QString &message);
    void slotQtyText(QString &qty);
    void slotQtyRemoveOrMovemenet(QString &qty);
    void slotConfirmQtyForRemove(const double &qty);
    void slotConfirmQtyForMovement(const double &qty);
    void slotPayment();
    void slotClose();
    void slotEndTableMovement(int code);
    void settingsError(const QString &message);
    void newSettingsVersion();
    void endOfSettingsQuery(int code, const QString &message);
    void takeDiscountElement(const QString &qName, const QXmlAttributes &atts);
    void endOfDiscountElement(int code, const QString &message);
    void endChangeStaffQuery(int code, const QString &message);

    void on_btnOrderDown_clicked();
    void on_btnOrderUp_clicked();
    void on_btnPrintServiceCheck_clicked();
    void on_btnExit_clicked();
    void on_pushButton_12_clicked();
    void on_pushButton_18_clicked();
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
    void on_btnQtyDel_clicked();
    void on_btnPayment_clicked();
    void on_tableDishes_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_btnQtyAccept_clicked();
    void on_btnDishComment_clicked();
    void on_btnMoveDish_clicked();
    void on_btnCheckout_clicked();
    void on_btnRemoveWholeOrder_clicked();
    void on_tblGroup_clicked(const QModelIndex &index);
    void on_tblTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_pushButton_6_clicked();
    void on_tblDish_clicked(const QModelIndex &index);
    void on_btnQtyReject_clicked();
    void on_btnMenuUp_2_clicked();
    void on_btnMenuDown_2_clicked();
    void on_btnMenuUp_clicked();
    void on_btnMenuDown_clicked();
    void on_btnUpTables_clicked();
    void on_btnDownTables_clicked();
    void on_btnUpUpTables_clicked();
    void on_btnDownDownTables_clicked();
    void on_tblGroupOfGroup_clicked(const QModelIndex &index);
    void on_btnDishSpecialComment_clicked();
    void on_btnMoveOrder_clicked();
    void on_btnAdditional_clicked();
    void on_btnDiscount_clicked();
    void on_tableDishes_clicked(const QModelIndex &index);
    void on_btnCangeOrderOwner_clicked();

signals:
    void qtyText(QString &text);
    void confirmQty(const double &qty);
};

#endif // DLGWORKSPACE_H
