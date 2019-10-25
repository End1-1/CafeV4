#include "dlgsalarydoc.h"
#include "ui_dlgsalarydoc.h"
#include "mdibutton.h"
#include "dlgsimpleselect.h"
#include "core.h"
#include "../../common/qdlgcache.h"
#include <QUuid>
#include <QLocale>

#define AMOUNT_COL 4
#define GROUP_COL 1

DlgSalaryDoc::DlgSalaryDoc(const QString &docid, QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::DlgSalaryDoc)
{
    ui->setupUi(this);
    m_loadedAutomaticaly = false;
    ui->leDocNum->setText(docid);
    setWindowTitle(tr("Salary document") + " #" + docid);
    ui->leOperator->setText(___ff_user->fullName);
    ui->deDate->setDate(QDate::currentDate());
    ui->deStart->setDate(QDate::currentDate());
    ui->deEnd->setDate(QDate::currentDate());

    m_actions << "actionSave" << "actionPrint" << "actionMagic" << "actionRefresh" << "actionExport_to_Excel";
    setActionVisible("actionMagic", ___ff_user->roleRead(ROLE_M_SALARY_TO_AS));

    connect(ui->cbEmployee, SIGNAL(keyEnterPressed()), SLOT(cbEmployeeKeyEnterPressed()));
    QList<int> gridWidth;
    gridWidth << 0 << 0 << 250 << 400 << 100;
    int col = 0;
    for (QList<int>::const_iterator it = gridWidth.begin(); it != gridWidth.end(); it++)
        ui->tblList->setColumnWidth(col++, *it);

    m_sqlDrv = new QSqlDrv(___ff_user->fullName, "clientdb");
    m_sqlDrv->prepare("select acc_debit_credit from sys_databases where upper(db_path)=upper(:db_path)");
    m_sqlDrv->bind(":db_path", m_sqlDrv->m_db.databaseName().toUpper());
    m_sqlDrv->execSQL();
    if (m_sqlDrv->next())
        ui->leDebetCredit->setText(m_sqlDrv->valStr("ACC_DEBIT_CREDIT"));
    m_sqlDrv->close();
    QSettings s("Jazzve", "Cafe4");
    s.setValue("def_cash_index", false);
    getCashList();
    getEmployesList();
    if (docid.toInt())
        getDoc();

    QStringList l = s.value("def_cash_id_sal_doc", "").toString().split(";", QString::SkipEmptyParts);
    QMap<QString, QString> v;
    for (QStringList::const_iterator it = l.begin(); it != l.end(); it++) {
        QStringList t = (*it).split("=", QString::SkipEmptyParts);
        v[t.at(0)] = t.at(1);
    }
    ui->cbCash->setCurrentIndex(v[QSystem::objectName()].toInt());
    s.setValue("def_cash_index", true);

    show();
}

DlgSalaryDoc::~DlgSalaryDoc()
{
    delete ui;
}

void DlgSalaryDoc::actionPrintPreview()
{

}

void DlgSalaryDoc::actionPrint()
{
    QPrinter prn;
    QPrintDialog pd(&prn);
    if (pd.exec() != QDialog::Accepted)
        return;

    SizeMetrics sm(prn.resolution());
    XmlPrintMaker xpm(&sm);
    getPrintable(xpm);
    QPainter painter(&prn);
    XmlPrinter xPrn(&painter, &sm);
    for (int i = 0, count = xpm.pageCount(); i < count; i++) {
        if (i)
            prn.newPage();
        xPrn.render(xpm.page(i));
    }
}

void DlgSalaryDoc::actionSave()
{
    if (___ff_user->id != 1) {
        CHECK_VIEWER
        if (!___ff_user->roleWrite(ROLE_M_SALARY)) {
            QMessageBox::critical(this, tr("Access denied"), tr("You cannot create the salary document"));
            return;
        }

        if (ui->leDocNum->text().toInt()) {
            if (!___ff_user->roleEdit(ROLE_M_SALARY)) {
                QMessageBox::critical(this, tr("Access denied"), tr("You cannot edit this document"));
                return;
            }
        }
    }

    QString errorMsg = "";
    if (ui->cbCash->currentIndex() < 0)
        errorMsg += tr("The cash is not selected") + "\r\n";

    if (!ui->tblList->rowCount())
        errorMsg += tr("Employes list is empty") + "\r\n";

    if (errorMsg.length()) {
        QMessageBox::critical(this, tr("Error"), errorMsg);
        return;
    }

    m_sqlDrv->m_db.transaction();

    ui->leOperator->setText(___ff_user->fullName);
    if (!m_loadedAutomaticaly) {
        getSalariesFormulas();
        countSalaries();
    }

    if (ui->leDocNum->text().toInt()) {
        m_sqlDrv->prepare("delete from salary_doc where id=:id");
        m_sqlDrv->bind(":id", ui->leDocNum->text());
        m_sqlDrv->execSQL();
        m_sqlDrv->prepare("delete from salary_body where doc_id=:doc_id");
        m_sqlDrv->bind(":doc_id", ui->leDocNum->text());
        m_sqlDrv->execSQL();
    } else
        ui->leDocNum->setText(QString::number(m_sqlDrv->genId("GEN_SALARY_DOC_ID")));

    m_sqlDrv->prepare("insert into salary_doc (id, doc_date, cash_id, operator_id, date1, date2, halls, hc) values(:id, :doc_date, :cash_id, :operator_id, :date1, :date2, :halls, 0)");
    m_sqlDrv->bind(":id", ui->leDocNum->text());
    m_sqlDrv->bind(":doc_date", ui->deDate->date());
    m_sqlDrv->bind(":cash_id", ui->cbCash->itemData(ui->cbCash->currentIndex()));
    m_sqlDrv->bind(":operator_id", ___ff_user->id);
    m_sqlDrv->bind(":date1", ui->deStart->date());
    m_sqlDrv->bind(":date2", ui->deEnd->date());
    m_sqlDrv->bind(":halls", m_hallValues);
    m_sqlDrv->execSQL();

    m_sqlDrv->prepare("insert into salary_body values (null, :doc_id, :employee_id, :amount, :role_id)");
    m_sqlDrv->bind(":doc_id", ui->leDocNum->text());
    for (int i = 0, count = ui->tblList->rowCount(); i < count; i++) {
        m_sqlDrv->bind(":employee_id", ui->tblList->item(i, 0)->data(Qt::DisplayRole).toString());
        m_sqlDrv->bind(":amount", ui->tblList->item(i, AMOUNT_COL)->data(Qt::DisplayRole).toString());
        m_sqlDrv->bind(":role_id", ui->tblList->item(i, 1)->data(Qt::DisplayRole).toString());
        m_sqlDrv->execSQL();
    }

    m_sqlDrv->close();
    changeSavedState(true);
}

void DlgSalaryDoc::actionCostum(int a)
{
    switch (a) {
    case ACTION_MAGIC: {
        if (ui->leAmount->text().toDouble() < 0.1) {
            QMessageBox::warning(this, tr("Error"), tr("Document is not complited"));
            return;
        }
        QSqlDatabase d = QSqlDatabase::addDatabase("QODBC", "salary");
        m_sqlDrv->openDB();
        m_sqlDrv->execSQL("select conn_str, username, pwd, salary_db from sys_as_conn");
        if (m_sqlDrv->m_query->next()) {
            d.setDatabaseName(m_sqlDrv->valStr("CONN_STR") + ";Database=" + m_sqlDrv->valStr("SALARY_DB"));
            d.setUserName(m_sqlDrv->valStr("USERNAME"));
            d.setPassword(m_sqlDrv->valStr("PWD"));
        }
        m_sqlDrv->close();
        if (!d.open()) {
            QMessageBox::critical(this, tr("Error"), tr("Cannot connect to AS database"));
            return;
        }
        d.transaction();
        QString docId = QUuid::createUuid().toString();
        QString docNum = QSystem::objectName() + "-" + ui->leDocNum->text();
        QSqlQuery q(d);
        q.prepare("select fISN from DOCUMENTS where fDOCNUM=:fDOCNUM");
        q.bindValue(":fDOCNUM", docNum);
        if (!q.exec())  {
            QMessageBox::critical(this, tr("SQL Error"), q.lastError().databaseText());
            d.close();
            return;
        }
        if (q.next()) {
            m_sqlDrv->openDB();
            m_sqlDrv->execSQL("update salary_doc set hc=1 where id=" + ui->leDocNum->text());
            m_sqlDrv->close();
            d.close();
            QMessageBox::warning(this, tr("Warning"), tr("The document exists"));
            return;
        }
        q.prepare("insert into DOCUMENTS ("
                  "fISN, fDOCTYPE, fDOCSTATE, fDATE, fORDERNUM, fDOCNUM, fCUR, fSUMM, fCOMMENT, fBODY, fPARTNAME, "
                  "fUSERID, fPARTID, fCRPARTID, fMTID, fEXPTYPE, fINVN, fENTRYSTATE, "
                  "fEMPLIDRESPIN, fEMPLIDRESPOUT, fVATTYPE, fSPEC, fCHANGEDATE,fEXTBODY,fETLSTATE) VALUES ("
                  ":fISN, :fDOCTYPE, :fDOCSTATE, '" + ui->deDate->date().toString("yyyy-MM-dd") + "', :fORDERNUM, :fDOCNUM, :fCUR, :fSUMM, :fCOMMENT, :fBODY, :fPARTNAME, "
                  ":fUSERID, :fPARTID, :fCRPARTID, :fMTID, '', :fINVN, :fENTRYSTATE, "
                  ":fEMPLIDRESPIN, :fEMPLIDRESPOUT, '', :fSPEC, current_timestamp,'', '')");
        q.bindValue(":fISN", docId);
        q.bindValue(":fDOCTYPE", 1);
        q.bindValue(":fDOCSTATE", 1);
        q.bindValue(":fORDERNUM", ui->leDocNum->text().toInt());
        q.bindValue(":fDOCNUM", docNum);
        q.bindValue(":fCUR", "AMD");
        q.bindValue(":fSUMM", ui->leAmount->text());
        q.bindValue(":fCOMMENT", "");
        q.bindValue(":fBODY", "\r\nRATE:1.0000\r\nRATEBASE:1.0000\r\n");
        q.bindValue(":fPARTNAME", "");
        q.bindValue(":fUSERID", 0);
        q.bindValue(":fPARTID", -1);
        q.bindValue(":fCRPARTID", -1);
        q.bindValue(":fMTID", -1);
        q.bindValue(":fINVN", "");
        q.bindValue(":fENTRYSTATE", 1);
        q.bindValue(":fEMPLIDRESPIN", -1);
        q.bindValue(":fEMPLIDRESPOUT", 1);
        q.bindValue(":fSPEC", "");
        if (!q.exec()) {
            QMessageBox::critical(this, tr("SQL Error"), q.lastError().databaseText());
            d.close();
            return;
        }
        QStringList cor = ui->leDebetCredit->text().split("/");
        q.prepare("insert into DRAFTENTRIES ( "
                  "fACCDB, fACCCR, fPARTDBID, fPARTCRID, fCURRCODEDB, fCURRCODECR, fSUMM, "
                  "fCURSUMM, fCOMMENT, fBASE, fOP, fTRANS, "
                  "fANALYTIC1DB, fANALYTIC1CR, fANALYTIC2DB, fANALYTIC2CR) values ("
                  ":fACCDB, :fACCCOR, :fPARTDBID, :fPARTCRID, 'AMD', 'AMD', :fSUMM, "
                  ":fCURSUMM, :fCOMMENT, :fBASE, 'M', :fTRANS, "
                  "'', '', '', '')");
        for (int i = 0; i < ui->tblList->rowCount(); i++) {
            q.bindValue(":fACCDB", cor[0]);
            q.bindValue(":fACCCOR", cor[1]);
            q.bindValue(":fPARTDBID", -1);
            q.bindValue(":fPARTCRID", ui->tblList->item(i, 0)->data(Qt::DisplayRole).toInt() + 100000);
            q.bindValue(":fSUMM", ui->tblList->item(i, AMOUNT_COL)->data(Qt::DisplayRole).toDouble());
            q.bindValue(":fCURSUMM", ui->tblList->item(i, AMOUNT_COL)->data(Qt::DisplayRole).toDouble());
            q.bindValue(":fBASE", docId);
            q.bindValue(":fCOMMENT", ui->tblList->item(i, 3)->data(Qt::DisplayRole).toString());
            q.bindValue(":fTRANS", i + 1);
            if (!q.exec()) {
                QMessageBox::critical(this, tr("SQL Error"), q.lastError().databaseText());
                d.close();
                return;
            }
        }
        m_sqlDrv->openDB();
        m_sqlDrv->execSQL("update salary_doc set hc=1 where id=" + ui->leDocNum->text());
        m_sqlDrv->close();
        d.commit();
        d.close();
    }
    }
    QMessageBox::information(this, tr("Information"), tr("Saved to AS database"));
}

void DlgSalaryDoc::actionRefresh()
{
    ui->tblList->clearContents();
    ui->tblList->setRowCount(0);
    QMap<int, float> pmap;
    pmap[14] = 0.033;
    pmap[15] = 0.03;
    pmap[3] = 0.028;
    pmap[16] = 0.025;
    pmap[49] = 0.025;
    m_sqlDrv->openDB();
    QString sql1 = "select e.id, e.group_id, e.fname || ' ' || e.lname as name, eg.name as gname, sum(o.amount) as amount "
                   "from o_order o, employes e, employes_group eg "
                   "where o.state_id=2 and o.staff_id=e.id and e.group_id=eg.id and e.group_id not in (59) "
                   "and o.date_cash between :date1 and :date2 "
                   "group by 1, 2, 3, 4 "
                    "order by 3 ",
            sql2 = "select e.id, e.group_id, e.fname || ' ' || e.lname as name, eg.name as gname, "
                   "sum((od.qty * od.price) + (o.amount_inc_value * od.qty * od.price) - (((od.qty * od.price) + (o.amount_inc_value * od.qty * od.price)) * amount_dec_value)) as amount "
                    "from o_order o, employes e, employes_group eg, o_dishes od "
                    "where o.state_id=2 and od.last_user=e.id and e.group_id=eg.id and od.state_id=1 and o.id=od.order_id and e.group_id not in (59) "
                    "and o.date_cash between :date1 and :date2 "
                    "group by 1, 2, 3, 4 "
                    "order by 3 ";
    if (ui->rbOrders->isChecked())
        m_sqlDrv->prepare(sql1);
    else
        m_sqlDrv->prepare(sql2);
    m_sqlDrv->bind(":date1", ui->deStart->date());
    m_sqlDrv->bind(":date2", ui->deEnd->date());
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        if (!pmap.contains(m_sqlDrv->valInt("GROUP_ID")))
            continue;
        float v = m_sqlDrv->valFloat("AMOUNT") * (pmap[m_sqlDrv->valInt("GROUP_ID")]);
        int vi = ((int)(v / 100)) * 100;
        if (vi < 3000)
            vi = 3000;
        int row = ui->tblList->rowCount();
        ui->tblList->setRowCount(row + 1);
        for (int i = 0; i < ui->tblList->columnCount(); i++)
            ui->tblList->setItem(row, i, new QTableWidgetItem());
        ui->tblList->item(row, 0)->setText(m_sqlDrv->valStr("ID"));
        ui->tblList->item(row, 1)->setText(m_sqlDrv->valStr("GROUP_ID"));
        ui->tblList->item(row, 2)->setText(m_sqlDrv->valStr("GNAME"));
        ui->tblList->item(row, 3)->setText(m_sqlDrv->valStr("NAME"));
        ui->tblList->item(row, 4)->setText(QString::number(vi));
    }
    m_loadedAutomaticaly = true;
    countTotalAmount();
}

void DlgSalaryDoc::addEmployee(QList<QVariant> &data)
{
    ui->tblList->setRowCount(ui->tblList->rowCount() + 1);
    int row = ui->tblList->rowCount() - 1;
    for (int i = 0, count = ui->tblList->columnCount(); i < count; i++) {
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, data[i]);
        if ((i != AMOUNT_COL) || ui->leDocNum->text().toInt())
            item->setFlags(item->flags() ^ Qt::ItemIsEditable);
        ui->tblList->setItem(row, i, item);
    }
    countTotalAmount();
}

void DlgSalaryDoc::getPrintable(XmlPrintMaker &x)
{
    x.setFontName(APP_FONT_NAME);
    x.setFontSize(14);
    int top = -1;
    int rowHeight = 10;
    int col = 0;

    QList<int> vertDev;
    QStringList text;
    vertDev << 0 << 10 << 50 << 120 << 150 << 195;

    //Document header
    x.text(tr("Salary payment document") + " N-" + ui->leDocNum->text(), 2, top);
    top += x.lastTextHeight() + 1;
    x.setFontSize(10);
    x.textRightAlign(QString("%1: %2").arg(tr("Cash")).arg(ui->cbCash->currentText()), 195, top);
    top += x.lastTextHeight() + 1;
    x.textRightAlign(QString("%1: %2").arg(tr("Document date")).arg(ui->deDate->date().toString(DATE_FORMAT)), 195, top);
    top += x.lastTextHeight() + 1;
    x.textRightAlign(QString("%1: %2 - %3").arg(tr("Date range")).arg(ui->deStart->date().toString(DATE_FORMAT))
                     .arg(ui->deEnd->date().toString(DATE_FORMAT)), 195, top);
    top += x.lastTextHeight() + 2;
    if (m_hallNames.length()) {
        x.text(tr("Hall"), 2, top);
        x.textRightAlign(ui->leHall->text(), 195, top);
        top += x.lastTextHeight() + 1;
    }

    //Workers table header
    x.setFontBold(true);
    for (QList<int>::const_iterator it = vertDev.begin(); it != vertDev.end(); it++)
        x.line(*it, top, *it, top + rowHeight);
    x.text(tr("N"), 2, top + 1);
    x.text(tr("Post"), 12, top + 1);
    x.text(tr("Full name"), 52, top + 1);
    x.text(tr("Amount"), 122, top + 1);
    x.text(tr("Signature"), 152, top + 1);
    x.line(0, top, 195, top);
    x.line(0, top + rowHeight, 195, top + rowHeight);
    top += rowHeight;

    /* List of employes */
    QLocale l;
    x.setFontBold(false);
    for (int i = 0, count = ui->tblList->rowCount(); i < count; i++) {
        x.line(0, top + rowHeight, 195, top + rowHeight);
        for (QList<int>::const_iterator it = vertDev.begin(); it != vertDev.end(); it++)
            x.line(*it, top, *it, top + rowHeight);
        text.clear();
        text << QString("%1").arg(i + 1)
             << ui->tblList->item(i, 2)->text()
             << ui->tblList->item(i, 3)->text()
             << l.toString(ui->tblList->item(i, AMOUNT_COL)->text().toFloat(), 'f', 0);

        col = 0;
        for (QStringList::const_iterator it = text.begin(); it != text.end(); it++)
            x.text(*it, vertDev[col++] + 2, top + 2);
        top += rowHeight;
        x.checkForNewPage(top);
    }

    //Print total amount
    x.checkForNewPage(top);
    vertDev.clear();
    text.clear();
    vertDev << 0 << 120 << 195;
    text << tr("Total") << l.toString(ui->leAmount->text().toFloat(), 'f', 0);
    x.line(0, top + rowHeight, 195, top + rowHeight);
    //top += x.lastTextHeight() + 2;
    for (QList<int>::const_iterator it = vertDev.begin(); it != vertDev.end(); it++)
        x.line(*it, top, *it, top + rowHeight);
    col = 0;
    for (QStringList::const_iterator it = text.begin(); it != text.end(); it++)
        x.text(*it, vertDev[col++] + 2, top + 2);

    //Operator
    top += (rowHeight * 2);
    x.text(___ff_user->fullName, 5, top - 5);
    x.line(0, top, 60, top);
    x.text(tr("Operator"), 5, top);
    x.line(140, top, 195, top);
    x.text(tr("Signature"), 145, top);
    top += 10;
    x.checkForNewPage(top);
    x.setFontSize(8);
    x.text(tr("Printed") + ": " + QSystem::objectName() + " / " + QDateTime::currentDateTime().toString(DATETIME_FORMAT), 5, top);
    top += x.lastTextHeight() + 1;

    x.finishPage();
}

void DlgSalaryDoc::getEmployesList()
{
    m_employes.clear();
    m_post.clear();
    m_sqlDrv->execSQL("select eg.id as post_id, e.id as employee_id, eg.name as post_name, e.fname || ' ' || e.lname as employee_name, "
                      "e.fname || ' ' || e.lname || ' / ' || eg.name as name_post "
                      "from employes e, employes_group eg "
                      "where e.state_id=1 and e.group_id=eg.id and e.group_id not in (59) "
                      "order by e.fname");
    int colCount = m_sqlDrv->m_query->record().count();
    while (m_sqlDrv->m_query->next()) {
        QMap<QString, QString> e;
        for (int i = 0; i < colCount; i++)
            e[m_sqlDrv->m_query->record().fieldName(i)] = m_sqlDrv->m_query->value(i).toString();
        m_employes[m_sqlDrv->valInt("EMPLOYEE_ID")] = e;
        ui->cbEmployee->addItem(m_sqlDrv->valStr("NAME_POST"), m_sqlDrv->valInt("EMPLOYEE_ID"));
    }

    m_sqlDrv->execSQL("select id, name from employes_group order by 2");
    while (m_sqlDrv->m_query->next())
        m_post[m_sqlDrv->valStr("ID")] = m_sqlDrv->valStr("NAME");

    m_sqlDrv->close();
}

void DlgSalaryDoc::getCashList()
{
    m_sqlDrv->execSQL("select id, name from cash order by 2");
    while (m_sqlDrv->m_query->next())
        ui->cbCash->addItem(m_sqlDrv->valStr("NAME"), m_sqlDrv->valInt("ID"));
}

void DlgSalaryDoc::getDoc()
{
    m_sqlDrv->prepare("select doc_date, cash_id, operator_id, e.fname || ' ' || e.lname as operator_name, "
                      "date1, date2, halls from salary_doc s, employes e where s.operator_id=e.id and s.id=:id");
    m_sqlDrv->bind(":id", ui->leDocNum->text());
    m_sqlDrv->execSQL();
    if (m_sqlDrv->m_query->next()) {
        ui->deDate->setDate(m_sqlDrv->valDate("DOC_DATE"));
        ui->deStart->setDate(m_sqlDrv->valDate("DATE1"));
        ui->deEnd->setDate(m_sqlDrv->valDate("DATE2"));
        ui->cbCash->setCurrentIndex(ui->cbCash->findData(m_sqlDrv->valInt("CASH_ID")));
        ui->leOperator->setText(m_sqlDrv->valStr("OPERATOR_NAME"));
        m_hallValues = m_sqlDrv->valStr("HALLS");
    }
    m_sqlDrv->prepare("select sb.employee_id, e.fname || ' ' || e.lname as employee_name, sb.role_id as post_id, sb.amount "
                      "from salary_body sb, employes e "
                      "where sb.employee_id=e.id and sb.doc_id=:id");
    m_sqlDrv->bind(":id", ui->leDocNum->text());
    m_sqlDrv->execSQL();
    while (m_sqlDrv->m_query->next()) {
        QList<QVariant> data;
        data.append(m_sqlDrv->valStr("EMPLOYEE_ID"));
        data.append(m_sqlDrv->valStr("POST_ID"));
        data.append(m_post[m_sqlDrv->valStr("POST_ID")]);
        data.append(m_sqlDrv->valStr("EMPLOYEE_NAME"));
        data.append(m_sqlDrv->valFloat("AMOUNT"));
        addEmployee(data);
    }
    if (m_hallValues.length()) {
        m_sqlDrv->execSQL("select name from h_hall where id in (" + m_hallValues + ")");
        bool f = true;
        while (m_sqlDrv->next()) {
            if (f)
                f = false;
            else
                m_hallNames += ",";
            m_hallNames += m_sqlDrv->valStr("NAME");
        }
        ui->leHall->setText(m_hallNames);
    }
    m_sqlDrv->close();
    changeSavedState(true);
}

void DlgSalaryDoc::changeSavedState(bool saved)
{
    setActionVisible("actionPrint", saved);
    setActionVisible("actionSave", !saved);
}

void DlgSalaryDoc::countSalaries()
{
    float amount = 0;
    for (int i = 0, count = ui->tblList->rowCount(); i < count; i++) {
        amount = getSalaryOfGroup(ui->tblList->item(i, 1)->data(Qt::DisplayRole).toInt());
        if (amount < 0)
            amount = ui->tblList->item(i, AMOUNT_COL)->text().toFloat();
        ui->tblList->item(i, AMOUNT_COL)
                ->setText(QString::number(amount, 'f', 0));
    }
    countTotalAmount();
}

void DlgSalaryDoc::getSalariesFormulas()
{
    m_salaries.clear();
    m_sqlDrv->execSQL("select id, group_id, func, params, cell from employes_salary order by group_id, id");
    int group = 0;
    while (m_sqlDrv->m_query->next()) {
        if (group != m_sqlDrv->valInt("GROUP_ID")) {
            group = m_sqlDrv->valInt("GROUP_ID");
            m_salaries[group] = QList<Salary>();
        }
        Salary s;
        s.function = m_sqlDrv->valInt("FUNC");
        s.params = m_sqlDrv->valStr("PARAMS");
        s.cell = m_sqlDrv->valInt("CELL");
        m_salaries[group].append(s);
    }
    m_sqlDrv->close();
}

void DlgSalaryDoc::countTotalAmount()
{
    double amount = 0;
    for (int i = 0, count = ui->tblList->rowCount(); i < count; i++)
        amount += ui->tblList->item(i, AMOUNT_COL)->data(Qt::DisplayRole).toDouble();
    ui->leAmount->setText(Toolkit::formatDouble(amount));
}

void DlgSalaryDoc::countTotalAmount(const QString &text)
{
    Q_UNUSED(text)
    countTotalAmount();
}

void DlgSalaryDoc::cbEmployeeKeyEnterPressed()
{
    QString name = ui->cbEmployee->currentText();
    int index = ui->cbEmployee->findText(name, Qt::MatchExactly);
    if (index < 0)
        return;
    ui->cbEmployee->setCurrentIndex(index);
    ui->btnAdd->click();
}

void DlgSalaryDoc::on_btnAdd_clicked()
{
    int index = ui->cbEmployee->currentIndex();
    if (index < 0)
        return;
    int employee_id = ui->cbEmployee->itemData(index).toInt();
    if (employee_id < 1) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid employee id"));
        return;
    }

    QMap<QString, QString> &employee_data = m_employes[employee_id];
    QList<QVariant> data;
    data.append(employee_data["EMPLOYEE_ID"]);
    data.append(employee_data["POST_ID"]);
    data.append(employee_data["POST_NAME"]);
    data.append(employee_data["EMPLOYEE_NAME"]);
    data.append(0.0);
    addEmployee(data);
    ui->cbEmployee->setCurrentIndex(-1);
    ui->cbEmployee->setFocus();
    ui->tblList->scrollToBottom();

    changeSavedState(false);
}


QTableWidgetLineEdit::QTableWidgetLineEdit(QTableWidgetItem *item)
{
    m_tableWidgetItem = item;
    if (m_tableWidgetItem)
        setText(m_tableWidgetItem->text());
    setSelection(0, text().length());

    setValidator(new QDoubleValidator());
    connect(this, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
}

void QTableWidgetLineEdit::textChanged(const QString &text)
{
    m_tableWidgetItem->setText(text);
}

void QTableWidgetLineEdit::focusOutEvent(QFocusEvent *e)
{
    Q_UNUSED(e)
    m_tableWidgetItem->setText(text());
}

void DlgSalaryDoc::on_tblList_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if (previousColumn == AMOUNT_COL) {
        QLineEdit *e = static_cast<QLineEdit*>(ui->tblList->cellWidget(previousRow, previousColumn));
        ui->tblList->removeCellWidget(previousRow, previousColumn);
        delete e;
    }

    if (currentColumn == AMOUNT_COL) {
        QTableWidgetLineEdit *e = new QTableWidgetLineEdit(ui->tblList->item(currentRow, currentColumn));
        connect(e, SIGNAL(focusOut()), SLOT(countTotalAmount()));
        connect(e, SIGNAL(textChanged(QString)), SLOT(countTotalAmount(QString)));
        ui->tblList->setCellWidget(currentRow, currentColumn, e);
        e->setFrame(false);
        e->setFocus();
    }
}

void DlgSalaryDoc::on_btnDel_clicked()
{
    QModelIndexList rows = ui->tblList->selectionModel()->selectedRows();
    if (!rows.count())
        return;
    if (QMessageBox::warning(this, tr("Warning"),  tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;
    for (int i = rows.count() - 1; i > -1; i--)
        ui->tblList->removeRow(rows.at(i).row());
    countTotalAmount();
    changeSavedState(false);
}

void DlgSalaryDoc::on_btnChangePost_clicked()
{
    QModelIndexList rows = ui->tblList->selectionModel()->selectedRows();
    if (!rows.count())
        return;
    QString pPostName, pPostId;
    if (!DlgSimpleSelect::getValue(this, tr("Change post"), m_post, pPostName, pPostId))
        return;
    for (QModelIndexList::const_iterator it = rows.begin(); it != rows.end(); it++) {
        ui->tblList->item(it->row(), 1)->setText(pPostId);
        ui->tblList->item(it->row(), 2)->setText(pPostName);
    }
    changeSavedState(false);
    countTotalAmount();
}

void DlgSalaryDoc::on_deDate_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    changeSavedState(false);
}

void DlgSalaryDoc::on_deStart_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    changeSavedState(false);
}

void DlgSalaryDoc::on_deEnd_dateChanged(const QDate &date)
{
    Q_UNUSED(date)
    changeSavedState(false);
}

float DlgSalaryDoc::getSalaryOfGroup(int groupId)
{
#define OMANUAL 1
#define OTOTALSALE 2
#define ORETURN 3
#define OCOUNTPERCENT 4
#define OROUND 5
#define OLESS 6
#define OGREATER 7
#define OSETVALUE 8
#define OINCREASE 9
#define OCOUNTEMPLOYES 10
#define ODEVIDE 11
#define OSUBSTRACT 12
#define OSALEDURATION1 13
#define OADD 14
#define OTOTALSALE2 15
#define OSALARYOFGROUP 16
#define OSUMSALARYOFGROUP 17
#define OTOTALSALE3 18
#define OAMOUNTOFEMPLOYEE 19
    //date1 = date1 + one day (param1) + time (param2) ; date2 = date1 + one day + time (param3),

    QMap<int, float> mem;
    QString hallCondition = "";
    if (m_hallValues.length())
        hallCondition = " and o.table_id in (select id from h_table where hall_id in (" + m_hallValues + "))";
    QList<Salary> steps = m_salaries[groupId];
    for (int i = 0; i < steps.count(); i++) {
        Salary s = steps.at(i);
        QStringList params = s.params.split(";");
        switch (s.function) {
        case OMANUAL:
            mem[s.cell] = -1;
            break;
        case OTOTALSALE2: {
            QDateTime dt1 = QDateTime::fromString(ui->deStart->date().toString(DATE_FORMAT) + " 00:00:00", DATETIME_FORMAT).addDays(params[0].toInt());
            QDateTime dt2 = QDateTime::fromString(ui->deStart->date().toString(DATE_FORMAT) + " 00:00:00", DATETIME_FORMAT).addDays(params[0].toInt());
            dt1 = dt1.addSecs(params[1].toInt());
            dt2 = dt2.addSecs(params[2].toInt());
            m_sqlDrv->prepare("select sum(amount) as amount from o_order o "
                              " where (date_open > :date1 and date_close < :date2) and state_id=2" + hallCondition);
            m_sqlDrv->bind(":date1", dt1);
            m_sqlDrv->bind(":date2", dt2);
            m_sqlDrv->bind(":date3", ui->deStart->date().addDays(1));
            m_sqlDrv->execSQL();
            m_sqlDrv->next();
            mem[s.cell] = m_sqlDrv->val().toFloat();
            break;
        }
        case OTOTALSALE3: {
            QDateTime dt1 = QDateTime::fromString(ui->deStart->date().toString(DATE_FORMAT) + " 00:00:00", DATETIME_FORMAT);
            QDateTime dt2 = QDateTime::fromString(ui->deStart->date().toString(DATE_FORMAT) + " 00:00:00", DATETIME_FORMAT);
            dt1 = dt1.addSecs(params[1].toInt());
            dt2 = dt2.addSecs(params[2].toInt());
            m_sqlDrv->prepare("select sum(amount) as amount from o_order o "
                              " where date_close between :date1 and :date2 and state_id=2" + hallCondition);
            m_sqlDrv->bind(":date1", dt1);
            m_sqlDrv->bind(":date2", dt2);
            m_sqlDrv->execSQL();
            m_sqlDrv->next();
            mem[s.cell] = m_sqlDrv->val().toFloat();
            break;
        }
        case OTOTALSALE:
            m_sqlDrv->prepare("select sum(amount) as amount from o_order o where date_cash between :date1 and :date2 and state_id=2" + hallCondition);
            m_sqlDrv->bind(":date1", ui->deStart->date());
            m_sqlDrv->bind(":date2", ui->deEnd->date());
            m_sqlDrv->execSQL();
            m_sqlDrv->m_query->next();
            mem[s.cell] = m_sqlDrv->valFloat("AMOUNT");
            break;
        case ORETURN:
            return mem[s.cell];
            break;
        case OCOUNTPERCENT:
            mem[s.cell] = mem[params[1].toInt()] * params[0].toFloat();
            break;
        case OROUND:
            mem[s.cell] = trunc(mem[s.cell]);
            break;
        case OLESS:
            if (mem[params[0].toInt()] < mem[params[1].toInt()])
                mem[s.cell] = mem[params[2].toInt()];
            else
                mem[s.cell] = mem[params[3].toInt()];
            break;
        case OGREATER:
            if (mem[params[0].toInt()] > mem[params[1].toInt()])
                mem[s.cell] = mem[params[2].toInt()];
            else
                mem[s.cell] = mem[params[3].toInt()];
            break;
        case OSETVALUE:
            mem[s.cell] = params[0].toFloat();
            break;
        case OINCREASE:
            mem[s.cell] = params[1].toFloat() + mem[params[0].toInt()];
            break;
        case OCOUNTEMPLOYES:
            mem[s.cell] = 0;
            for (int i = 0; i < ui->tblList->rowCount(); i++) {
                QStringList emps = params[0].split(",", QString::SkipEmptyParts);
                ui->tblList->item(i, GROUP_COL)->text();
                if (emps.contains(ui->tblList->item(i, GROUP_COL)->text()))
                    mem[s.cell] ++;
            }
            break;
        case ODEVIDE:
            if (mem[params[0].toInt()] != 0)
                mem[s.cell] /= mem[params[0].toInt()];
            else
                mem[s.cell] = 0;
            break;
        case OSUBSTRACT:
            mem[s.cell] = mem[params[0].toInt()] - mem[params[1].toInt()];
            break;
        case OSALEDURATION1:
        {
            QDateTime start = QDateTime::fromString(ui->deStart->date().toString(DATE_FORMAT) + " " + params[0] + ":00", DATETIME_FORMAT);
            QDateTime end = QDateTime::fromString(ui->deStart->date().toString(DATE_FORMAT) + " " + params[0] + ":00", DATETIME_FORMAT);
            end = end.addSecs((60 * 60 * params[1].toInt()) - 1);
            m_sqlDrv->prepare("select sum(amount) as amount from o_order o where date_close between :date1 and :date2 and state_id=2" + hallCondition);
            m_sqlDrv->bind(":date1", start);
            m_sqlDrv->bind(":date2", end);
            m_sqlDrv->execSQL();
            m_sqlDrv->m_query->next();
            mem[s.cell] = m_sqlDrv->valFloat("AMOUNT");
        }
            break;
        case OADD:
            mem[s.cell] = mem[params[0].toInt()] + mem[params[1].toInt()];
            break;
        case OSALARYOFGROUP: {
            bool found = false;
            for (int i = 0; i < ui->tblList->rowCount(); i++)
                if (ui->tblList->item(i, 1)->data(Qt::DisplayRole).toInt() == params[0].toInt()) {
                    found = true;
                    break;
                }
            if (found)
                mem[s.cell] = getSalaryOfGroup(params[0].toInt());
            else
                mem[s.cell] = 0;
            break;
        }
        case OSUMSALARYOFGROUP: {
            int qty = 0;
            for (int i = 0; i < ui->tblList->rowCount(); i++)
                if (ui->tblList->item(i, 1)->data(Qt::DisplayRole).toInt() == params[0].toInt())
                    qty++;
            mem[s.cell] = mem[params[1].toInt()] * qty;
            break;
        }
        }
    }
    return -1;
}

void DlgSalaryDoc::on_btnHall_clicked()
{
    QDlgCache *dlg = new QDlgCache("H_HALL", "", this);
    if (dlg->exec() == QDialog::Accepted) {
        dlg->getValues(m_hallNames, m_hallValues);
        ui->leHall->setText(m_hallNames);
    }
    delete dlg;
}

void DlgSalaryDoc::on_cbCash_currentIndexChanged(int index)
{
    QSettings s("Jazzve", "Cafe4");
    if (!s.value("def_cash_index").toBool())
        return;
    QStringList l = s.value("def_cash_id_sal_doc", "").toString().split(";", QString::SkipEmptyParts);
    QMap<QString, QString> v;
    for (QStringList::const_iterator it = l.begin(); it != l.end(); it++) {
        QStringList t = (*it).split("=", QString::SkipEmptyParts);
        v[t.at(0)] = t.at(1);
    }
    v[QSystem::objectName()] = QString::number(index);
    QString str;
    for (QMap<QString, QString>::const_iterator it = v.begin(); it != v.end(); it++)
        str += it.key() + "=" + it.value() + ";";
    s.setValue("def_cash_id_sal_doc", str);
}
