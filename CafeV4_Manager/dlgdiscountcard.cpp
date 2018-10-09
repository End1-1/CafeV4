#include "dlgdiscountcard.h"
#include "ui_dlgdiscountcard.h"

DlgDiscountCard::DlgDiscountCard(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgDiscountCard)
{
    ui->setupUi(this);
}

DlgDiscountCard::~DlgDiscountCard()
{
    delete ui;
}

void DlgDiscountCard::btnOK()
{
    m_sqlDriver->openDB();
    m_sqlDriver->prepare("update or insert into costumers_names (id) values (:id) matching (id)");
    m_sqlDriver->bind(":id", ui->leId->text());
    m_sqlDriver->execSQL();
    m_sqlDriver->close();
    QDlgQuery::btnOK();
}

void DlgDiscountCard::on_leId_textChanged(const QString &arg1)
{
    if (arg1.length() != 4)
        return;
    m_sqlDriver->openDB();
    m_sqlDriver->prepare("select state_id, name, info, mod_id, code from costumers_names where id=:id");
    m_sqlDriver->bind(":id", arg1);
    m_sqlDriver->execSQL();
    if (m_sqlDriver->next()) {
        ui->cbState->setIndexOfData(m_sqlDriver->valInt("STATE_ID"));
        ui->cbDiscount->setIndexOfData(m_sqlDriver->valInt("MOD_ID"));
        ui->leName->setText(m_sqlDriver->valStr("NAME"));
        ui->leInfo->setText(m_sqlDriver->valStr("INFO"));
        ui->leCode->setText(m_sqlDriver->valStr("CODE"));
    }
    m_sqlDriver->close();
}
