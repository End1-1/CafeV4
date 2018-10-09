#include "dlgdebtcode.h"
#include "ui_dlgdebtcode.h"

DlgDebtCode::DlgDebtCode(const Mode &mode, const QString &tableName, const QString &session, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgDebtCode)
{
    ui->setupUi(this);
    ui->leKey->setText(tableName.toUpper());
    m_session = session;
    m_mode = mode;
    if (m_mode == mRemove) {
        ui->leCustomer->setEnabled(false);
    }
}

DlgDebtCode::~DlgDebtCode()
{
    delete ui;
}

void DlgDebtCode::takeDebtCheck(const QString &qName, const QXmlAttributes &atts)
{
    if (qName == "") {
        return;
    }
}

void DlgDebtCode::endDebtCheck(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    ___dlgProgress->hide();
    if (code) {
        DlgMessageBox::show(message, 1, this);
        return;
    }
}

void DlgDebtCode::takeInsertDebt(const QString &qName, const QXmlAttributes &atts)
{
}

void DlgDebtCode::on_leCustomer_returnPressed()
{
    ui->leKey->setFocus();
}

void DlgDebtCode::on_btnCancel_clicked()
{
    reject();
}

void DlgDebtCode::on_btnOk_clicked()
{
    if (!correctData())
        return;

    ___dlgProgress->show();
    BaseNetwork *b = new BaseNetwork();

    switch (m_mode) {
    case mInsert: {
        b->appendData("sysop", "DEBT_INSERT");
        b->appendData("customer", ui->leCustomer->text());
        b->appendData("table", ui->leKey->text());
        b->appendData("host", Connection::lockName());
        b->appendData("cafe", "1");
        b->appendData("su", "1");
        b->appendData("session", m_session);
        connect(b, SIGNAL(takeElement(QString,QXmlAttributes)), SLOT(takeInsertDebt(QString,QXmlAttribute)));
        connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endInsertDebt(int,QString)));
        break;
    }
    case mRemove: {
        b->appendData("sysop", "DEBT_REMOVE");
        b->appendData("table", ui->leKey->text());
        b->appendData("cafe", "1");
        b->appendData("su", "1");
        b->appendData("session", m_session);
        connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endRemoveDebt(int,QString)));
        break;
    }
    }

    b->post();
}

bool DlgDebtCode::correctData()
{
    ui->leCustomer->setText(ui->leCustomer->text().trimmed());
    ui->leKey->setText(ui->leKey->text().trimmed());

    bool result = true;
    QString message;

    if (m_mode == mInsert) {
        if (!ui->leCustomer->text().length()) {
            message += tr("The code of customer cannot be empty") + "\r\n";
            result = false;
        }
    }

    if (!ui->leKey->text().length()) {
        message += tr("The table name cannot be empty") + "\r\n";
        result = false;
    }


    if (!result)
        DlgMessageBox::show(message, 1, this);

    return result;
}

void DlgDebtCode::keyPressEvent(QKeyEvent *e)
{
    if ((e->key() == Qt::Key_Return) || (e->key() == Qt::Key_Enter))
        return;

    QDialog::keyPressEvent(e);
}

void DlgDebtCode::endInsertDebt(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    ___dlgProgress->hide();
    if (code) {
        DlgMessageBox::show(message, 1, this);
        return;
    }

    DlgMessageBox::show(QString(tr("The table '%1' is now reserved for '%2'")).arg(ui->leKey->text()).arg(ui->leCustomer->text()));
    close();
}

void DlgDebtCode::endRemoveDebt(int code, const QString &message)
{
    BaseNetwork::removeInstance(sender());
    ___dlgProgress->hide();
    if (code) {
        DlgMessageBox::show(message, 1, this);
        return;
    }

    close();
}

void DlgDebtCode::on_leKey_editingFinished()
{
    if (ui->leKey->property("BUSY").toBool())
        return;

    ui->leKey->setProperty("BUSY", true);
    ui->leKey->setText(ui->leKey->text().toUpper());
    ui->leKey->setProperty("BUSY", false);
}

void DlgDebtCode::on_leKey_returnPressed()
{
    ui->btnOk->click();
}
