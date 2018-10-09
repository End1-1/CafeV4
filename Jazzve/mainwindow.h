#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include "core.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    enum EnterType {etNone = 0, etTable, etPassword, etOrders, etDebtInsert, etDebtRemove};
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void beginProgram(int code, const QString &message);
    void hallUpdated();
    void menuUpdated(int code, const QString &message);
    void authQuery(int code, const QString &message);
    void endOfSettings(int code, const QString &message);
    void tableClick(const QString &tableId, const QString &tableName);
    void passEditLostFocus();
    void timeout();
    void setDate(const QString &qName, const QXmlAttributes &atts);

    void on_pushButton_9_clicked();
    void on_pushButton_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_13_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_14_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_15_clicked();
    void on_lePassword_returnPressed();
    void on_btnEnter_clicked();    
    void on_btnPassword_clicked();
    void on_btnTablesDown_clicked();
    void on_btnTablesUp_clicked();
    void on_btnSettings_clicked();
    void on_btnOrders_clicked();
    void on_btnDebt_clicked();
    void on_btnRemoveDebt_clicked();
    void on_actionNewDebt_triggered();
    void on_actionDebRemove_triggered();
    void on_btnBusyTables_clicked();

private:
    Ui::MainWindow *ui;
    QString m_currentHall;
    QMap<QString, QString> m_hallFilter;
    StationAuth m_userProgram;
    StationAuth m_staff;
    EnterType m_enterType;
    QString m_enterData;
    QTimer m_timer;
    void btnNumpadPressed(QObject *button);
    void auth();

protected:
    void closeEvent(QCloseEvent *e);
};

#endif // MAINWINDOW_H
