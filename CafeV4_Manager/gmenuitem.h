#ifndef GMENUITEM_H
#define GMENUITEM_H

#include "qbasesqlwindow.h"
#include "qsqldrv.h"
#include "qpcombobox.h"
#include <QLineEdit>

namespace Ui {
class GMenuItem;
}

class QCellLineEdit : public QLineEdit {
    Q_OBJECT
private:
    QTableWidget *m_table;
public:
    int m_col;
    int m_row;
    QCellLineEdit(int col, int row, QWidget *parent = 0);
private slots:
    void currentTextChanged(const QString &text);

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual void focusOutEvent(QFocusEvent *e);

signals:
    void focusChanged(bool focused);
};

class GMenuItem : public QBaseSqlWindow
{
    Q_OBJECT

private:
    static int m_lastTypeIndex;
    static int m_lastPaymentIndex;
    static int m_lastColor;
    static int m_lastTab;

public:
    explicit GMenuItem(QWidget *parent = 0);
    ~GMenuItem();
    virtual void actionNew();
    virtual void actionSave();
    virtual void actionSearch();
    void setTabIndex(int index);
    void setDish(int id);

private slots:
    void storeChange(int index);
    void printChange(int index);
    void priceChange(const QString &str);
    void print1Change(const QString &str);
    void print2Change(const QString &str);
    void textChangedQty(const QString &text); //recipe
    void showRecipeMenu(const QPoint &pos); // recipe
    void webResponse(const QString &response, bool isError);
    void webRawResponse(QByteArray &data, bool isError);

    void on_btnColor_clicked();
    void on_gridMenu_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_tblGoods_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_actionRNew_row_triggered();
    void on_actionRDelete_row_triggered();
    void on_actionRCopy_recipe_triggered();
    void on_actionRPaste_recipe_triggered();
    void on_btnLoadImage_clicked();
    void on_cbType_currentIndexChanged(int index);
    void on_cbPayment_currentIndexChanged(int index);

    void on_chCopyNumToCliboard_clicked(bool checked);

private:
    int m_id;
    QString m_imgLink; // On web server
    QString m_imgPath; //For changed
    QLineEdit *m_lePrice;
    QPComboBox *m_cbStore;
    QPComboBox *m_cbPrint;
    void loadMenu();
    void loadRecipe();

    /* Recipe */
    void countTotal();
    void addGoods();
    void downloadImageFromServer();
    void uploadImageToServer();
    Ui::GMenuItem *ui;

protected slots:
    bool checkOk();

protected:
    virtual void keyPressEvent(QKeyEvent *e);


};

#endif // GMENUITEM_H
