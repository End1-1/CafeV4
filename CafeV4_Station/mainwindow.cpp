#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkReply>
#include <QDebug>
#include <QXmlInputSource>
#include <QXmlAttributes>
#include <QInputDialog>
#include "dlgworkspace.h"
#include "core.h"

#define ATTR_AUTH 1
#define ATTR_TABLE_DATA 2

MainWindow::MainWindow(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();
    ui->lbInfo->setText(QString("%1").arg(Connection::server()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::reject()
{
}

void MainWindow::accept()
{
}

void MainWindow::Login()
{
    connect(&___staff, SIGNAL(endOfQuery(int,QString)), SLOT(slotLoginNetReply(int, QString)), Qt::UniqueConnection);
    ___staff.auth2(ui->lePassword->text(), DBID);
    ui->lePassword->clear();
}

void MainWindow::slotBtnClick()
{
    QPushButton *btn = qobject_cast<QPushButton *>(sender());
    if (btn->text().length() == 1) {
        switch (btn->text().toLatin1().at(0)) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '0':
            ui->lePassword->setText(ui->lePassword->text() + btn->text());
            break;
        case 'X':
            ui->lePassword->clear();
            break;
        case '<':
            ui->lePassword->setText(ui->lePassword->text().mid(0, ui->lePassword->text().length() - 1));
            break;
        }
    } else if (!btn->text().compare(tr("Enter"))) {
        Login();
    }
    ui->lePassword->setFocus();
}

void MainWindow::on_pushButton_4_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_5_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_6_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_13_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_2_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_7_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_10_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_14_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_3_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_8_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_11_clicked()
{
    slotBtnClick();
}

void MainWindow::on_pushButton_15_clicked()
{
    slotBtnClick();
}

void MainWindow::on_lePassword_returnPressed()
{
    Login();
}

void MainWindow::slotLoginNetReply(int code, const QString &message)
{
    disconnect(&___staff, SIGNAL(endOfQuery(int,QString)), this, SLOT(slotLoginNetReply(int, QString)));
    if (code) {
        DlgMessageBox::show(message, 1, this);
        return;
    }

    DlgWorkspace *dlgWorkspace = new DlgWorkspace(this);
    dlgWorkspace->exec();
    delete dlgWorkspace;
}

void MainWindow::on_pushButton_9_clicked()
{
    qApp->quit();
}

void MainWindow::on_btnSettings_clicked()
{
    QString settingsPassword = QInputDialog::getText(this, tr("Settings password"), tr("Password"), QLineEdit::Password);
    if (settingsPassword == Connection::settingsPassword()) {
        dlgMainDBSettings *d = new dlgMainDBSettings(this);
        d->exec();
        delete d;
    } else
        DlgMessageBox::show(tr("Access denied"), 1, this);
}
