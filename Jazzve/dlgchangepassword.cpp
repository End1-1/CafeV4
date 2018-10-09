#include "dlgchangepassword.h"
#include "ui_dlgchangepassword.h"
#include "dlggetpassword.h"
#include <QScrollBar>

DlgChangePassword::DlgChangePassword(const QString &session, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgChangePassword)
{
    ui->setupUi(this);
    showFullScreen();

    m_col = 0;
    m_row = 0;
    m_colCount = (ui->tableWidget->width() - 4) / 300;
    ui->tableWidget->setColumnCount(m_colCount);

    m_session = session;

    BaseNetwork *b = new BaseNetwork();
    b->appendData("sysop", "EMPLOYES");
    b->appendData("cafe", "1");
    b->appendData("session", session);
    connect(b, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(takeEmployee(QString,QXmlAttributes)));
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfEmployeeQuery(int,QString)));
    b->post();
}

DlgChangePassword::~DlgChangePassword()
{
    delete ui;
}

void DlgChangePassword::takeEmployee(const QString &qName, const QXmlAttributes &atts)
{
    if (qName == "employee") {
        if (!ui->tableWidget->rowCount())
            ui->tableWidget->setRowCount(1);
        if (m_col >= m_colCount) {
            ui->tableWidget->setRowCount(ui->tableWidget->rowCount() + 1);
            m_col = 0;
            m_row++;
        }
        QTableWidgetItem *item = new QTableWidgetItem(atts.value("NAME"));
        item->setData(Qt::UserRole, atts.value("ID"));
        ui->tableWidget->setItem(m_row, m_col++, item);
        return;
    }
}

void DlgChangePassword::on_pushButton_clicked()
{
    accept();
}


void DlgChangePassword::endOfEmployeeQuery(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    if (code)
        DlgMessageBox::show(message, 1, this);
}

void DlgChangePassword::endOfPasswordQuery(int code, const QString &message)
{
    if (code)
        DlgMessageBox::show(message, 1, this);
    else
        DlgMessageBox::show(tr("Password was changed"), 0, this);
}

void DlgChangePassword::on_pushButton_3_clicked()
{
    ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->value() + 10);
}

void DlgChangePassword::on_pushButton_4_clicked()
{
    ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->value() - 10);
}

void DlgChangePassword::on_pushButton_2_clicked()
{

}

void DlgChangePassword::on_btnChangePassword_clicked()
{
    QModelIndexList lst = ui->tableWidget->selectionModel()->selectedIndexes();
    if (!lst.count()) {
        DlgMessageBox::show(tr("Nobody selected"), 1, this);
        return;
    }

    DlgGetPassword *dlg = new DlgGetPassword(this);
    QString password;
    if (dlg->exec() == QDialog::Accepted)
        password = dlg->property("password").toString();
    else
        password = "nopasswordforchanges";
    delete dlg;

    if (password == "nopasswordforchanges")
        return;

    if (!password.length()) {
        DlgMessageBox::show(tr("Password cannot be empty"));
        return;
    }

    BaseNetwork *b = new BaseNetwork();
    b->appendData("sysop", "CHANGEPASSWORD");
    b->appendData("cafe", "1");
    b->appendData("session", m_session);
    b->appendData("user", lst.at(0).data(Qt::UserRole).toString());
    b->appendData("password", password);
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfPasswordQuery(int,QString)));
    b->post();
}
