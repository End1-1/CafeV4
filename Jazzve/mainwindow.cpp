#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dlgorder.h"
#include "core.h"
#include "dlgchangepassword.h"
#include "dlgdebtcode.h"
#include "../core_gui/dlgorders.h"
#include <QShortcut>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifndef FITNES
    ui->btnDebt->setVisible(false);
    ui->btnRemoveDebt->setVisible(false);
#endif
    qApp->processEvents();
    m_enterType = etNone;

    ui->lbDateTime->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));
    ui->lbAmount->setText("0/0");

    ___dlgProgress = new DlgProgress(this);
    ___dlgProgress->show();

    connect(&m_userProgram, SIGNAL(endOfQuery(int,QString)), SLOT(beginProgram(int, QString)));
    connect(&m_staff, SIGNAL(endOfQuery(int,QString)), SLOT(authQuery(int, QString)));
    connect(&___hall, SIGNAL(tableRefreshed()), SLOT(hallUpdated()));
    connect(&___menu, SIGNAL(endOfQuery(int,QString)), SLOT(menuUpdated(int,QString)));

    m_userProgram.auth1(Connection::user(), Connection::password());
    QFont font(qApp->font());
    font.setPointSize(12);
    font.setBold(true);
    ui->btnEnter->setFont(font);

    QShortcut *f5 = new QShortcut(QKeySequence("F5"), this);
    QShortcut *f6 = new QShortcut(QKeySequence("F3"), this);
    connect(f5, SIGNAL(activated()), SLOT(on_btnDebt_clicked()));
    connect(f6, SIGNAL(activated()), SLOT(on_btnRemoveDebt_clicked()));

    connect(ui->lePassword, SIGNAL(focusOut()), SLOT(passEditLostFocus()));
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(timeout()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_9_clicked()
{
    close();
}

void MainWindow::beginProgram(int code, const QString &message)
{
    if (!___settings) {
        ___settings = new Settings(Connection::waiterSettings());
        connect(___settings, SIGNAL(endOfQuery(int,QString)), SLOT(endOfSettings(int,QString)));
        ___settings->loadSettings(m_userProgram.session(), "1");
    }

    if (!___mod)
        ___mod = new OrderModifierList(m_userProgram.session());

    if (code == CODE_SUCCESS) {
        ___settings->uploadPrinters(m_userProgram.session());
        ___menu.post(m_userProgram.session());
    } else {
        ___dlgProgress->hide();
        DlgMessageBox::show(message, 0, this);
    }
    ui->lePassword->setFocus();
    m_timer.start(60000);
}

void MainWindow::hallUpdated()
{
    ui->lbAmount->setText(QString("%1 %2 / %3").arg(tr("Orders")).arg(___hall.pCount()).arg(Toolkit::formatDouble(___hall.pAmount())));
    ui->wdtNumpad->setEnabled(true);
    ui->lePassword->setFocus();
    ___dlgProgress->hide();
}

void MainWindow::menuUpdated(int code, const QString &message)
{
    Q_UNUSED(code)
    Q_UNUSED(message)
}

void MainWindow::authQuery(int code, const QString &message)
{
    ___dlgProgress->hide();
    m_timer.stop();
    if (!code) {
        switch (m_enterType) {
        case etTable: {
            if (m_enterData.toInt() == 0)
                break;
            if (!m_staff.getReadPermission(PERMISSION_READ_ORDER)) {
                DlgMessageBox::show(tr("Access denied"), 1, this);
                break;
            }
            DlgOrder *dlgOrder = new DlgOrder(this);
            dlgOrder->setMain(&m_staff, m_enterData);
            dlgOrder->exec();
            delete dlgOrder;
            m_enterType = etNone;
            ui->btnEnter->setText("");
            break;
        }
        case etPassword: {
            if (!m_staff.getWritePermission(PERMISSION_CHANGE_PASSWORD)) {
                DlgMessageBox::show(tr("Access denied"), 1, this);
                break;
            }
            DlgChangePassword *dlg = new DlgChangePassword(m_staff.session(), this);
            dlg->exec();
            delete dlg;
            break;
        }
        case etOrders: {
            if (!m_staff.getReadPermission(PERMISSION_STATION_REPORTS)) {
                DlgMessageBox::show(tr("Access denied"), 1, this);
                return;
            }
            DlgOrders *d = new DlgOrders(m_staff, this);
            d->exec();
            delete d;
            break;
        }
        case etDebtInsert: {
            if (!m_staff.getWritePermission(PERMISSION_CUSTOMER_DEBT)) {
                DlgMessageBox::show(tr("Access denied"), 1, this);
                return;
            }
            DlgDebtCode *d = new DlgDebtCode(DlgDebtCode::mInsert, m_enterData, m_staff.session(), this);
            d->exec();
            delete d;
            break;
        }
        case etDebtRemove: {
            if (!m_staff.getWritePermission(PERMISSION_CUSTOMER_DEBT)) {
                DlgMessageBox::show(tr("Access denied"), 1, this);
                return;
            }
            DlgDebtCode *d = new DlgDebtCode(DlgDebtCode::mRemove, m_enterData, m_staff.session(), this);
            d->exec();
            delete d;
            break;
        }
        default:
            break;
        }
    } else
        DlgMessageBox::show(message, 1, this);
    m_enterData = "";
    m_enterType = etNone;
    ui->btnEnter->setText("");
    ___hall.refreshData();
    m_timer.start();
}

void MainWindow::endOfSettings(int code, const QString &message)
{
    if (code)
        DlgMessageBox::show(message, 1 , this);
    ___dlgProgress->hide();
    m_currentHall = ___settings->toString(SETTINGS_HALL_ID);
    ___hall.setAvailableHall(___settings->toString(SETTINGS_AVAILABLE_HALLS));
    ___hall.setSession(m_userProgram.session());
    ___hall.setTable(ui->tables);
    ___hall.setHallPanel(ui->wdtHall);
    ___hall.loadData();
    connect(&___hall, SIGNAL(tableClick(QString,QString)), SLOT(tableClick(QString,QString)));
}

void MainWindow::tableClick(const QString &tableId, const QString &tableName)
{
    m_enterData = tableId;
    m_enterType = etTable;
    ui->btnEnter->setText(tableName);
}

void MainWindow::passEditLostFocus()
{
    ui->lePassword->setFocus();
}

void MainWindow::timeout()
{
    ui->lbDateTime->setText(QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm"));
    QMap<QString, QString> request;
    request["session"] = m_userProgram.session();
    CafeNetwork *c = new CafeNetwork(PAGE_S_DATE, this);
    c->setSlots(SLOT(setDate(QString, QXmlAttributes)));
    c->post(request);
    ___hall.refreshData();
}

void MainWindow::setDate(const QString &qName, const QXmlAttributes &atts)
{
    return;
    if (qName == "set-date")
        ___settings->setSystemDateTime(atts.value(0));
}

void MainWindow::btnNumpadPressed(QObject *button)
{
    QPushButton *btn = qobject_cast<QPushButton *>(button);

    if (!btn->text().compare("X")) {
        ui->lePassword->clear();
        return;
    }

    if (!btn->text().compare("<")) {
        ui->lePassword->setText(ui->lePassword->text().left(ui->lePassword->text().length() - 1));
        return;
    }

    ui->lePassword->setText(ui->lePassword->text() + btn->text());
}

void MainWindow::auth()
{
    if (m_enterType == etNone) {
        ui->lePassword->clear();
        DlgMessageBox::show("");
        return;
    }
    ___dlgProgress->show(tr("Checking password"));
    m_staff.auth2(ui->lePassword->text());
    ui->lePassword->clear();
}

void MainWindow::closeEvent(QCloseEvent *e)
{
    ___dlgProgress->show(tr("Exit"));
    m_userProgram.release();
    for (int i = 0; i < 500000; i++)
        qApp->processEvents();
    QMainWindow::closeEvent(e);
}

void MainWindow::on_pushButton_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_5_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_6_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_13_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_2_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_7_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_10_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_14_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_3_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_8_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_11_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_pushButton_15_clicked()
{
    btnNumpadPressed(sender());
}

void MainWindow::on_lePassword_returnPressed()
{
    auth();
}

void MainWindow::on_btnEnter_clicked()
{
    auth();
}

void MainWindow::on_btnPassword_clicked()
{
    m_enterType = etPassword;
    ui->btnEnter->setText(tr("Enter to passwords"));
}

void MainWindow::on_btnTablesDown_clicked()
{
    ui->tables->verticalScrollBar()->setValue(ui->tables->verticalScrollBar()->value() + 10);
}

void MainWindow::on_btnTablesUp_clicked()
{
    ui->tables->verticalScrollBar()->setValue(ui->tables->verticalScrollBar()->value() - 10);
}

void MainWindow::on_btnSettings_clicked()
{
    QString settingsPassword = QInputDialog::getText(this, tr("Settings password"), tr("Password"), QLineEdit::Password);
    if (settingsPassword.length() || (!Connection::settingsPassword().length())) {
        if (settingsPassword == Connection::settingsPassword()) {
            dlgMainDBSettings *d = new dlgMainDBSettings(this);
            d->exec();
            delete d;
        } else
            DlgMessageBox::show(tr("Access denied"), 1, this);
    }
}

void MainWindow::on_btnOrders_clicked()
{
    m_enterType = etOrders;
    ui->btnEnter->setText(tr("Orders history"));
}

void MainWindow::on_btnDebt_clicked()
{
    Hall2::Table t;
    QString tableName;
    if (___hall.getTable(m_enterData, t)) {
        tableName = t.toString(TABLE_NAME);
        m_enterData = "";
    }

    DlgDebtCode *d = new DlgDebtCode(DlgDebtCode::mInsert, m_enterData, m_userProgram.session(), this);
    d->exec();
    delete d;
    ___hall.refreshData();
}

void MainWindow::on_btnRemoveDebt_clicked()
{
    Hall2::Table t;
    QString tableName;
    if (___hall.getTable(m_enterData, t)) {
        tableName = t.toString(TABLE_NAME);
        m_enterData = "";
    }

    //m_enterType = etDebtRemove;
    DlgDebtCode *d = new DlgDebtCode(DlgDebtCode::mRemove, m_enterData, m_userProgram.session(), this);
    d->exec();
    delete d;
    ___hall.refreshData();
}

void MainWindow::on_actionNewDebt_triggered()
{
    on_btnDebt_clicked();
}

void MainWindow::on_actionDebRemove_triggered()
{
    on_btnRemoveDebt_clicked();
}

void MainWindow::on_btnBusyTables_clicked()
{
    bool busy = ___hall.busyFilter();
    if (busy)
        ui->btnBusyTables->setIcon(QIcon(":/new/prefix1/res/icons/table.png"));
    else
        ui->btnBusyTables->setIcon(QIcon(":/new/prefix1/res/icons/anchor.png"));
    ___hall.setBusyFilter();
}
