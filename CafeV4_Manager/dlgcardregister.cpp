#include "dlgcardregister.h"
#include "ui_dlgcardregister.h"

DlgCardRegister::DlgCardRegister(QWidget *parent) :
    QDlgBase(parent),
    ui(new Ui::DlgCardRegister)
{
    ui->setupUi(this);
    m_cardMod["2"] = "5";
    m_cardMod["3"] = "10";
    m_cardMod["4"] = "7";
    m_cardMod["5"] = "11";
    m_cardMod["6"] = "15";
    m_cardMod["7"] = "20";
    m_cardMod["8"] = "30";
}

DlgCardRegister::~DlgCardRegister()
{
    delete ui;
}

void DlgCardRegister::on_leCode_returnPressed()
{
    QString code = ui->leCode->text().replace(";", "").replace("?", "").simplified().replace(" ", "");
    ui->leCode->setText(code);
    if (code.length() != 14) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid card code"));
        return;
    }
    QString prefix = code.mid(0, 1);
    QString regdate = code.mid(1, 6);
    QString discount = QString::number(code.mid(7, 2).toInt());
    if (!discount.length()) {
        QMessageBox::critical(this, tr("Error"), tr("Unknown discount value"));
        return;
    }
    int id = code.mid(9, 5).toInt();
    m_sqlDriver->openDB();
    m_sqlDriver->prepare("select id, name, info from costumers_names where id=:id");
    m_sqlDriver->bind(":id", id);
    m_sqlDriver->execSQL();
    if (m_sqlDriver->next()) {
        ui->leId->setText(m_sqlDriver->valStr("ID"));
        ui->leDiscount->setText(discount);
        ui->leName->setText(m_sqlDriver->valStr("NAME"));
        ui->leInfo->setText(m_sqlDriver->valStr("INFO"));
    } else {
        QMessageBox::critical(this, tr("Error"), tr("This card is not registered"));
    }
    m_sqlDriver->close();
}

void DlgCardRegister::btnOK()
{
    return;
}

void DlgCardRegister::on_btnNext_clicked()
{
    m_sqlDriver->openDB();
    m_sqlDriver->prepare("update costumers_names set state_id=2, name=:name, info=:info, mod_id=:mod_id, code=:code where id=:id");
    m_sqlDriver->bind(":name", ui->leName->text());
    m_sqlDriver->bind(":info", ui->leInfo->text());
    m_sqlDriver->bind(":mod_id", m_cardMod.key(ui->leDiscount->text()).toInt());
    m_sqlDriver->bind(":code", ui->leCode->text());
    m_sqlDriver->bind(":id", ui->leId->text());
    m_sqlDriver->execSQL();
    m_sqlDriver->close();
    ui->leId->clear();
    ui->leName->clear();
    ui->leInfo->clear();
    ui->leDiscount->clear();
    ui->leCode->clear();
    ui->leCode->setFocus();
}
