#include "dlglogin.h"
#include "ui_dlglogin.h"
#include <QMessageBox>
#include "dlgconnection.h"
#include "qsqldb.h"
#include "cnfmaindb.h"
#include "cgridwidget.h"

dlgLogin::dlgLogin(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgLogin)
{
    ui->setupUi(this);
    setWindowTitle(tr("Login"));

    m_sqlDrv = new QSqlDrv("FASTFF", "maindb");

    if (!m_sqlDrv->prepare("select id, alias, db_path, db_user, db_pass, obj_name, group_id, main_id, web, webpass "
                           "from sys_databases order by alias")) {
        reject();
        return;
    }
    if (!m_sqlDrv->execSQL()) {
        reject();
        return;
    }
    while (m_sqlDrv->m_query->next()) {
        QMap<QString, QString> db;
        db["alias"] = m_sqlDrv->valStr("ALIAS");
        db["dbId"] = m_sqlDrv->valStr("ID");
        db["dbpath"] = m_sqlDrv->valStr("DB_PATH");
        db["dbuser"] = m_sqlDrv->valStr("DB_USER");
        db["dbpass"] = m_sqlDrv->valStr("DB_PASS");
        db["obj_name"] = m_sqlDrv->valStr("OBJ_NAME");
        db["group_id"] = m_sqlDrv->valStr("GROUP_ID");
        db["main_id"] = m_sqlDrv->valStr("MAIN_ID");
        db["web"] = m_sqlDrv->valStr("WEB");
        db["webpass"] = m_sqlDrv->valStr("WEBPASS");
        m_db.insert(db["dbId"].toInt(), db);
        ui->cbDatabase->addItem(db["alias"], db["dbId"]);
        if (m_sqlDrv->valStr("alias") == __cnfmaindb.fLastDatabase)
            ui->cbDatabase->setCurrentIndex(ui->cbDatabase->count() - 1);
    }
    m_sqlDrv->close();

    connect(ui->cbDatabase, SIGNAL(currentIndexChanged(int)), this, SLOT(cbDatabaseIndexChanged(int)));

    if (__cnfmaindb.fLastUsername.length() > 0) {
        ui->leLogin->setText(__cnfmaindb.fLastUsername);
        ui->lePassword->setFocus();
    }
}

dlgLogin::~dlgLogin()
{
    delete ui;
    delete m_sqlDrv;
}

void dlgLogin::on_pushButton_clicked()
{
    reject();
}

void dlgLogin::loginOnServer()
{
    if (!ui->cbDatabase->count()) {
        QMessageBox::warning(this, tr("Warning"), tr("No database in the list"));
        return;
    }

    int index = ui->cbDatabase->currentIndex();
    if (index < 0) {
        QMessageBox::critical(this, tr("Error"), tr("No database is selected"));
        return;
    }

    QMap<QString, QString> &m = m_db[ui->cbDatabase->itemData(ui->cbDatabase->currentIndex()).toInt()];
    QSqlDatabase &db = QSqlDB::dbByName("clientdb");
    db.close();
    db.setHostName("");
    db.setDatabaseName(m["dbpath"]);
    db.setUserName(m["dbuser"]);
    db.setPassword(m["dbpass"]);
    db.setConnectOptions("lc_ctype=utf-8");
    if (!db.open()) {
        QMessageBox::critical(this, tr("Connection error"), db.lastError().databaseText());
        return;
    }

    __DBPATH__ = m["dbpath"];
    __DBUSER__ = m["dbuser"];
    __DBPASS__ = m["dbpass"];
    if (___ff_user) {
        delete ___ff_user;
        ___ff_user = 0;
    }

    ___ff_user = new FF_User("clientdb");
    ___ff_user->setCredentails(ui->leLogin->text(), ui->lePassword->text());
    if (!___ff_user->auth2()) {
        QMessageBox::critical(this, tr("Error"), tr("Access denied"));
        ui->lePassword->clear();
        ui->lePassword->setFocus();
        return;
    }

    __cnfmaindb.fLastUsername = ui->leLogin->text();
    __cnfmaindb.write();

    /*
    QSystem::m_logWriter->setDb(QSystem::homePath(), ___ff_user->m_db.databaseName(),
                                ___ff_user->m_db.userName(),
                                ___ff_user->m_db.password());
                                */
    QSystem::setObjectName(m["obj_name"]);
    QSystem::setMainFlag(!m["main_id"].toInt());
    QSystem::setDbGroup(m["group_id"].toInt());
    QSystem::WebUrl = m["web"];
    QSystem::WebPass = m["webpass"];

    accept();
}

void dlgLogin::cbDatabaseIndexChanged(int index)
{
    Q_UNUSED(index)
    __cnfmaindb.fLastDatabase = ui->cbDatabase->currentText();
    __cnfmaindb.write();
}

void dlgLogin::on_pushButton_2_clicked()
{
    loginOnServer();
}

void dlgLogin::on_leLogin_returnPressed()
{
    ui->lePassword->setFocus();
}

void dlgLogin::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return:
        break;
    default:
        QDialog::keyPressEvent(e);
        break;
    }
}

void dlgLogin::on_lePassword_returnPressed()
{
    loginOnServer();
}

void dlgLogin::on_btnShowPassword_pressed()
{
    ui->lePassword->setEchoMode(QLineEdit::Normal);
}

void dlgLogin::on_btnShowPassword_released()
{
    ui->lePassword->setEchoMode(QLineEdit::Password);
}
