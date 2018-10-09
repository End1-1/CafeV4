#include "dlgorderhistory.h"
#include "ui_dlgorderhistory.h"

DlgOrderHistory::DlgOrderHistory(const QString &orderid, const QString &session, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgOrderHistory)
{
    ui->setupUi(this);
    BaseNetwork *b = new BaseNetwork;
    b->appendData("sysop", "ORDERHISTORY");
    b->appendData("cafe", "1");
    b->appendData("su", "1");
    b->appendData("session", session);
    b->appendData("order", orderid);
    connect(b, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(takeList(QString,QXmlAttributes)));
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfList(int,QString)));
    b->post();
}

DlgOrderHistory::~DlgOrderHistory()
{
    delete ui;
}

void DlgOrderHistory::takeList(const QString &qName, const QXmlAttributes &atts)
{
}


void DlgOrderHistory::endOfList(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    if (code)
        DlgMessageBox::show(message, 1, this);
}

void DlgOrderHistory::on_btnClose_clicked()
{
    close();
}
