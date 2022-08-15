#include "qdlgemployesaccess.h"
#include "ui_qdlgemployesaccess.h"
#include "ff_user.h"
#include "core.h"

QDlgEmployesAccess::QDlgEmployesAccess(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::QDlgEmployesAccess)
{
    ui->setupUi(this);
    m_actions << "actionSave" << "actionCopy" << "actionPaste";
    setWindowTitle(tr("Employes access editor"));

    ui->tblSales->setColumnWidth(0, 400);
    ui->tblSales->setColumnWidth(1, 100);
    ui->tblSales->setColumnWidth(2, 100);
    ui->tblManagment->setColumnWidth(0, 400);
    ui->tblManagment->setColumnWidth(1, 100);
    ui->tblManagment->setColumnWidth(2, 100);
}

QDlgEmployesAccess::~QDlgEmployesAccess()
{
    delete ui;
}

void QDlgEmployesAccess::actionSave()
{
    CHECK_VIEWER_AND_MAINDB
    saveRole(ui->tblSales, 1);
    saveRole(ui->tblManagment, 2);
}

void QDlgEmployesAccess::actionCostum(int action)
{
    Q_UNUSED(action)
}

void QDlgEmployesAccess::setGroup(const QString &groupId)
{
    m_groupId = groupId;
    checkForNewRole();
    load();
}

void QDlgEmployesAccess::checkForNewRole()
{
    QStringList l1;
    l1 << ROLE_CHANGE_PASSWORD << ROLE_ORDER_CHANGE_STAFF << ROLE_ORDER_DISCOUNT_AFTER_CHECKOUT
      << ROLE_ORDER_MULTIPLE_CHECKOUT << ROLE_ORDER_REMOVE_PRINTED_QTY << ROLE_WAITER_REPORTS
      << ROLE_EDIT_ORDER << ROLE_EDIT_AFTER_CHECKOUT << ROLE_REMOVE_ORDER << ROLE_MOVE_DISH
      << ROLE_MOVE_ORDER << ROLE_REMOVE_ORDER_FROM_CASH << ROLE_W_PAYMENT << ROLE_W_VIEW_SALES_AMOUNT
      << ROLE_W_PRESENT << ROLE_W_SPESIAL_ACTIONS << ROLE_REMOVE_ORDER_AFTER_CHECKOUT;

    QStringList l2;
    l2 << ROLE_M_SALARY << ROLE_M_EMPLOYES << ROLE_M_STORE_DOCUMENTS
       << ROLE_M_SYNC_WITH_AS << ROLE_M_COMMON_DIRECTORY << ROLE_M_MENU
       << ROLE_M_CALC_OUTPUT << ROLE_M_REMOVE_ORDERS << ROLE_M_VIEWER
       << ROLE_M_SALARY_TO_AS << ROLE_M_SALES << ROLE_M_DISCOUNT
       << ROLE_M_CANCELATION << ROLE_M_GROUP_QUERIES;

    initNewOldRoles(l1, 1, m_groupId);
    initNewOldRoles(l2, 2, m_groupId);
}

void QDlgEmployesAccess::initNewOldRoles(QStringList roles, int module_id, const QString &groupId)
{
    QStringList l3;
    m_sqlDrv->openDB();
    if (!m_sqlDrv->prepare("select distinct(key_name) from employes_access where module_id=:module_id and group_id=:group_id"))
        return;
    m_sqlDrv->bind(":module_id", module_id);
    m_sqlDrv->bind(":group_id", groupId.toInt());
    if (!m_sqlDrv->execSQL())
        return;
    while (m_sqlDrv->m_query->next())
        l3.append(m_sqlDrv->valStr("KEY_NAME"));

    /* append new roles */
    m_sqlDrv->prepare("insert into employes_access  (id, group_id, module_id, key_name, key_value) values (null, :group_id, :module_id, :key_name, '000')");
    m_sqlDrv->bind(":module_id", module_id);
    for (QStringList::const_iterator i = roles.begin(); i != roles.end(); i++) {
        if (!l3.contains(*i)) {
            l3.append(*i);
            m_sqlDrv->bind(":group_id", groupId.toInt());
            m_sqlDrv->bind(":key_name", *i);
            m_sqlDrv->execSQL();
        }
    }

    /* remove unused roles */
    m_sqlDrv->prepare("delete from employes_access where module_id=:module_id and key_name=:key_name");
    m_sqlDrv->bind(":module_id", module_id);
    for (QStringList::const_iterator i = l3.begin(); i != l3.end(); i++)
        if (!roles.contains(*i)) {
            m_sqlDrv->bind(":key_name", *i);
            m_sqlDrv->execSQL();
        }
    /* special for administrator */
    m_sqlDrv->execSQL("update employes_access set key_value='111' where group_id=1");
    m_sqlDrv->prepare("update employes_access set key_value='000' where group_id=1 and key_name=:key_name");
    m_sqlDrv->bind(":key_name", ROLE_M_VIEWER);
    m_sqlDrv->execSQL();
    m_sqlDrv->close();
}

bool QDlgEmployesAccess::load()
{
    m_sqlDrv->prepare("select module_id, key_name, key_value from employes_access where group_id=:group_id");
    m_sqlDrv->bind(":group_id", m_groupId);
    m_sqlDrv->execSQL();
    bool result = false;
    while (m_sqlDrv->m_query->next()) {
        result = true;
        switch (m_sqlDrv->valInt("MODULE_ID")) {
        case 1:
            addRole(ui->tblSales, m_sqlDrv->valStr("KEY_NAME"), m_sqlDrv->valStr("KEY_VALUE"));
            break;
        case 2:
            addRole(ui->tblManagment, m_sqlDrv->valStr("KEY_NAME"), m_sqlDrv->valStr("KEY_VALUE"));
            break;
        }
    }
    m_sqlDrv->close();
    return result;
}

void QDlgEmployesAccess::addRole(QTableWidget *t, const QString &key, const QString &val)
{
    int newRow = t->rowCount();
    t->setRowCount(newRow + 1);
    QTableWidgetItem *i1 = new QTableWidgetItem(key);
    QTableWidgetItem *i2 = new QTableWidgetItem();
    i2->setFlags(i2->flags() | Qt::ItemIsUserCheckable);
    i2->setCheckState(val.at(0) == QChar('1') ? Qt::Checked : Qt::Unchecked);
    QTableWidgetItem *i3 = new QTableWidgetItem();
    i3->setFlags(i3->flags() | Qt::ItemIsUserCheckable);
    i3->setCheckState(val.at(1) == QChar('1') ? Qt::Checked : Qt::Unchecked);
    t->setItem(newRow, 0, i1);
    t->setItem(newRow, 1, i2);
    t->setItem(newRow, 2, i3);
}

void QDlgEmployesAccess::saveRole(QTableWidget *t, int module)
{
    m_sqlDrv->prepare("update employes_access set key_value=:key_value where module_id=:module_id and key_name=:key_name and group_id=:group_id");
    for (int i = 0; i < t->rowCount(); i++) {
        QString value;
        QTableWidgetItem *it = t->item(i, 1);
        value = it->checkState() == Qt::Checked ? "1" : "0";
        it = t->item(i, 2);
        value += it->checkState() == Qt::Checked ? "1" : "0";
        value += "0";
        m_sqlDrv->bind(":key_value", value);
        m_sqlDrv->bind(":key_name", t->item(i, 0)->text());
        m_sqlDrv->bind(":module_id", module);
        m_sqlDrv->bind(":group_id", m_groupId);
        m_sqlDrv->execSQL();
    }
}
