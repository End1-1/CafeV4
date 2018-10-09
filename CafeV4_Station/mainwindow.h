#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QDialog
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void reject();
    void accept();
    
private:
    Ui::MainWindow *ui;
    void Login();

private slots:
    void slotBtnClick();
    void on_pushButton_4_clicked();
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
    void slotLoginNetReply(int code, const QString &message);
    void on_pushButton_9_clicked();
    void on_btnSettings_clicked();
};

#endif // MAINWINDOW_H
