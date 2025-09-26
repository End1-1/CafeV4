#include "storeorder.h"
#include "ui_storeorder.h"
#include "qsqlcache.h"
#include "printpreview.h"
#include "qbasegrid.h"
#include <QComboBox>
#include <QLineEdit>
#include <QPrintDialog>
#include <QPrinter>


StoreOrder::StoreOrder(const QString &docId, QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::StoreOrder)
{
    m_actions << "actionSave" << "actionPrint_preview" << "actionPrint" << "actionExport_to_Excel";
    setActionVisible("actionSave", ___ff_user->roleWrite(ROLE_M_STORE_DOCUMENTS));
    ui->setupUi(this);
    m_dock = new DockIdName(tr("Goods list"), this);
    m_tableCellEdit = 0;
    connect(m_dock, SIGNAL(clickedRow(int)), this, SLOT(dockAccepted(int)));
    setWindowTitle(tr("Store order"));
    resize(900, 600);

    ui->tableWidget->setColumnCount(6);
    ui->tableWidget->setColumnWidth(0, 150);
    ui->tableWidget->setColumnWidth(1, 300);
    ui->tableWidget->setColumnWidth(2, 80);
    ui->tableWidget->setColumnWidth(3, 80);
    ui->tableWidget->setColumnWidth(4, 80);
    ui->tableWidget->setColumnWidth(5, 150);
    QStringList tableCaptions;
    tableCaptions << tr("Code") << tr("Name")
                  << tr("Qty") << tr("Unit")
                  << tr("Price") << tr("Amount");
    ui->tableWidget->setHorizontalHeaderLabels(tableCaptions);
    ui->tableWidget->installEventFilter(this);

    getFoodList();
    ui->deDocDate->setDate(QDate::currentDate());  
    ui->tableWidget->installEventFilter(this);

    ui->cbStoreInputName->setCurrentIndex(-1);
    ui->cbStoreOutputName->setCurrentIndex(-1);
    ui->cbAction->setCurrentIndex(-1);
    ui->leOperator->setText(___ff_user->fullName);
    if (docId.toInt() > 0) {
        setId(docId);
    }

    qApp->processEvents();

}

StoreOrder::StoreOrder(QWidget *parent) :
    StoreOrder("0", parent)
{

}

StoreOrder::~StoreOrder()
{
    delete m_dock;
    delete ui;
}

void StoreOrder::setId(const QString &id)
{
    m_docNum = id;
    ui->lbDocNum->setText("#" + id);
    if (m_docNum.toInt())
        loadDoc();
}

void StoreOrder::actionSave()
{
    CHECK_VIEWER
    if (!checkDoc())
        return;

    if (!m_docNum.toInt())
        m_docNum = its(m_sqlDrv->genId("GEN_ST_DOCUMENTS_ID"));
    ui->lbDocNum->setText("#" + m_docNum);

    m_sqlDrv->prepare("delete from st_documents where id=:id");
    m_sqlDrv->bind(":id", m_docNum.toInt());
    m_sqlDrv->execSQL();

    m_sqlDrv->prepare("insert into st_documents (id, state_id, action_id, doc_date, "
                      "payment_method, store_input, store_output, editor_id,"
                      "admited_id, passed_id, provider_id, amount, comment) "
                      "values (:id, :state_id, :action_id, :doc_date,"
                      ":payment_method, :store_input, :store_output, :editor_id,"
                      ":admited_id,:passed_id,:provider_id,:amount,:comment)");
    m_sqlDrv->bind(":id", m_docNum.toInt());
    m_sqlDrv->bind(":state_id", 1);
    m_sqlDrv->bind(":action_id", ui->cbAction->currentItemData());
    m_sqlDrv->bind(":doc_date", ui->deDocDate->date());
    m_sqlDrv->bind(":payment_method", ui->cbPayment->itemData(ui->cbPayment->currentIndex()));
    m_sqlDrv->bind(":store_input", ui->cbStoreInputName->currentItemData());
    m_sqlDrv->bind(":store_output", ui->cbStoreOutputName->currentItemData());
    m_sqlDrv->bind(":editor_id", ___ff_user->id);
    m_sqlDrv->bind(":admited_id", ui->cbAdmited->currentItemData());
    m_sqlDrv->bind(":passed_id", ui->cbPassed->currentItemData());
    m_sqlDrv->bind(":provider_id", ui->cbPartnerName->currentItemData());
    m_sqlDrv->bind(":amount", ui->leAmount->text().toDouble());
    m_sqlDrv->bind(":comment", ui->leComment->text());
    m_sqlDrv->execSQL();

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        m_sqlDrv->prepare("insert into st_draft (doc_id, goods_id, qty, amount) "
                          "values (:doc_id, :goods_id, :qty, :amount)");
        m_sqlDrv->bind(":doc_id", m_docNum.toInt());
        m_sqlDrv->bind(":goods_id", ui->tableWidget->item(i, 0)->data(Qt::DisplayRole).toInt());
        m_sqlDrv->bind(":qty", ui->tableWidget->item(i, 2)->data(Qt::DisplayRole).toDouble());
        m_sqlDrv->bind(":amount", ui->tableWidget->item(i, 5)->data(Qt::DisplayRole).toDouble());
        m_sqlDrv->execSQL();
    }

    m_sqlDrv->close();
    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void StoreOrder::actionPrint()
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

void StoreOrder::actionPrintPreview()
{
    SizeMetrics sm(96);
    XmlPrintMaker xpm(&sm);
    getPrintable(xpm);
    PrintPreview *pp = new PrintPreview(this);
    pp->setPaintData(xpm, 0);
    pp->show();
}

void StoreOrder::actionExcel()
{

}


void StoreOrder::getPrintable(XmlPrintMaker &x)
{
    x.setFontName(APP_FONT_NAME);
    x.setFontSize(10);
    int top = 1;

    x.text(QSystem::objectName() + "/" + ui->cbAction->currentText(), 1, top);
    x.textRightAlign("N-" + m_docNum, 90, top);
    x.text(tr("Date"), 100, top);
    x.textRightAlign(ui->deDocDate->date().toString(DATE_FORMAT), 200, top);
    top += x.lastTextHeight() + 1;
    x.line(1, top, 90, top);
    x.line(100, top, 200, top);
    top ++;

    int actionCode = ui->cbAction->itemData(ui->cbAction->currentIndex()).toInt();
    if (actionCode == 1 || actionCode == 3 || actionCode == 7) {
        x.text(ui->lbStoreInput->text(), 1, top);
        x.textRightAlign(QSystem::objectName() + "/" + ui->cbStoreInputName->currentText(), 90, top);
        x.line(1, top + x.lastTextHeight() + 1, 90, top + x.lastTextHeight() + 1);

        x.text(ui->lbAdmited->text(), 100, top);
        x.textRightAlign(ui->cbAdmited->currentText(), 200 , top);
        x.line(100, top + x.lastTextHeight() + 1, 200, top + x.lastTextHeight() + 1);
        top += x.lastTextHeight() + 1;
    }

    if (ui->cbAction->itemData(ui->cbAction->currentIndex()) == DOC_INPUT) {
        if (ui->cbPartnerName->currentIndex() > -1) {
            x.text(ui->lbPartners->text(), 100, top);
            x.textRightAlign(ui->cbPartnerName->currentText(), 200, top);
            top += x.lastTextHeight() + 1;
            x.line(100, top + x.lastTextHeight() + 1, 200, top + x.lastTextHeight() + 1);
        }
    }

    if (actionCode > 1 && actionCode != 7) {
        x.text(ui->lbStoreOutput->text(), 1, top);
        x.textRightAlign(ui->cbStoreOutputName->currentText(), 90, top);
        x.line(1, top + x.lastTextHeight() + 1, 90, top + x.lastTextHeight() + 1);

        x.text(ui->lbPassed->text(), 100, top);
        x.textRightAlign(ui->cbPassed->currentText(), 200, top);
        x.line(100, top + x.lastTextHeight() + 1, 200, top + x.lastTextHeight() + 1);
        top += x.lastTextHeight() + 1;
    }

    top += 1;
    x.text(tr("Document maker"), 1, top);
    x.textRightAlign(ui->leOperator->text(), 90, top);
    x.line(0, top + x.lastTextHeight() + 1, 90, top + x.lastTextHeight() + 1);

    top += x.lastTextHeight() + 1;
    x.text(ui->lbComment->text(), 2, top);
    x.text(ui->leComment->text(), x.lastTextWidth() + 2, top); 
    x.line(0, top + x.lastTextHeight() + 1, 200, top + x.lastTextHeight() + 1);
    top += x.lastTextHeight() + 1;

    top += x.lastTextHeight() + 5;
    x.text(tr("List of goods"), 1, top);

    QList<int> points;
    points << 1 << 10 << 35 << 130 << 145 << 160 << 180 << 200;
    int rowHeight = 7;

    QStringList captions;
    captions << "NN" << tr("Code") << tr("Name") << tr("Qty")
             << tr("Unit") << tr("Price") << tr("Amount");

    top += x.lastTextHeight() + 1;
    x.line(1, top, 200, top);
    x.line(1, top + rowHeight, 200, top + rowHeight);
    for (int i = 0, count = points.count(); i < count; i++) {
        x.line(points.at(i), top, points.at(i), top + rowHeight);
        if (i < captions.count())
            x.text(captions.at(i), points.at(i) + 1, top + 1);
    }

    top += rowHeight;

    for (int i = 0, count = ui->tableWidget->rowCount(); i < count; i++) {
        for (int j = 0, count = points.count(); j < count; j++)
            x.line(points.at(j), top, points.at(j), top + rowHeight);
        x.text(QString::number(i + 1), points.at(0) + 1, top + 1);
        for (int j = 0; j < ui->tableWidget->columnCount(); j++) {
            QVariant v = ui->tableWidget->item(i, j)->data(Qt::DisplayRole);
            QString text;
            switch (v.type()) {
            case QVariant::Double:
                text = dts(v.toDouble());
                break;
            default:
                text = v.toString();
                break;
            }

            x.text(text, points.at(j + 1) + 1, top + 1);
        }
        top += rowHeight;
        x.line(1, top, 200, top);
        x.checkForNewPage(top);
    }
    for (int i = 3, count = points.count(); i < count; i += 2)
        x.line(points.at(i), top, points.at(i), top + rowHeight);
    x.text(tr("Total"), points.at(3) + 1, top + 1);
    x.text(ui->leAmount->text(), points.at(5) + 1, top + 1);
    top += rowHeight;
    x.line(points.at(3), top, 200, top);

    top += rowHeight * 2;
    x.checkForNewPage(top);
    x.line(1, top, 60, top);
    x.line(70, top, 130, top);
    x.line(140, top, 200, top);
    top += x.lastTextHeight();

    x.text(tr("Document maked by"), 1, top);
    x.text(ui->lbPassed->text(), 70, top);
    x.text(ui->lbAdmited->text(), 140, top);
    top += x.lastTextHeight() + 1;

    x.text(tr("Signature"), 1, top);
    x.text(tr("Signature"), 70, top);
    x.text(tr("Signature"), 140, top);

    top += x.lastTextHeight() * 3;
    x.checkForNewPage(top);
    x.setFontSize(6);
    x.text(QString("%1: %2 / %3").arg(tr("Printed")).arg(QSystem::objectName()).arg(QDateTime::currentDateTime().toString(DATETIME_FORMAT)), 1, top);
    x.finishPage();
}

void StoreOrder::getFoodList()
{
    m_dock->hide();
    m_foods.clear();
    m_sqlDrv->execSQL("select f.id, f.name, u.name "
                      "from food_names f, food_units u "
                      "where f.unit_id=u.id ");
    while (m_sqlDrv->next()) {
        Food f;
        f.id = m_sqlDrv->val().toInt();
        f.name = m_sqlDrv->val().toString();
        f.unit = m_sqlDrv->val().toString();
        m_foods[f.id] = f;
    }
    m_sqlDrv->close();

    QStringList ids, names;
    for (QMap<int, Food>::const_iterator i = m_foods.begin(); i != m_foods.end(); i++) {
        ids.append(its(i.key()));
        names.append(i.value().name);
    }
    m_dock->setCodeNameList(ids, names);
}

void StoreOrder::getFoodData(int row, double &qty, double &price, double &amount)
{
    qty = ui->tableWidget->item(row, 2)->text().toDouble();
    price = ui->tableWidget->item(row, 4)->text().toDouble();
    amount = ui->tableWidget->item(row, 5)->text().toDouble();
}

void StoreOrder::setFoodData(int row, double &qty, double &price, double &amount)
{
    ui->tableWidget->item(row, 2)->setText(dts(qty));
    ui->tableWidget->item(row, 4)->setText(dts(price));
    ui->tableWidget->item(row, 5)->setText(dts(amount));
    countAmount();
}

void StoreOrder::countAmount()
{
    double amount = 0;
    for (int i = 0; i < ui->tableWidget->rowCount(); i ++)
        amount += ui->tableWidget->item(i, 5)->text().toDouble();
    ui->leAmount->setText(dts(amount));
}

bool StoreOrder::checkDoc()
{
    QString error;
    if (ui->cbAction->currentIndex() < 0)
        error += tr("The action is not defined") + "\n";

    int docType = ui->cbAction->itemData(ui->cbAction->currentIndex()).toInt();
    switch (docType) {
    case DOC_ARTADRAMAS:
    case DOC_INPUT:
    case DOC_INVERNTORIZATION:
        if (ui->cbStoreInputName->currentIndex() < 0)
            error += tr("Input store is not selected") + "\n";
        break;
    case DOC_OUTPUT:
    case DOC_SALE:
        if (ui->cbStoreOutputName->currentIndex() < 0)
            error += tr("Output store is not selected") + "\n";
        break;
    case DOC_MOVEMENT:
    case DOC_COMPLECT_IN:
    case DOC_COMPLECT_OUT:
        if (ui->cbStoreInputName->currentIndex() < 0)
            error += tr("Input store is not selected") + "\n";
        if (ui->cbStoreOutputName->currentIndex() < 0)
            error += tr("Output store is not selected") + "\n";
        break;
    }

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        if (!ui->tableWidget->item(i, 0)->text().toInt()) {
            error += tr("One of the foods is not set") + "\n";
            break;
        }
    }

    for (int i = 0; i < ui->tableWidget->rowCount(); i++) {
        if (ui->tableWidget->item(i, 2)->text().toDouble() < 0.001 && ui->cbAction->currentItemData() != 7) {
            error += tr("One of the quantity of the foods is not set") + "\n";
            break;
        }
    }

    if (error.length())
        QMessageBox::critical(this, tr("Error"), error);

    return !error.length();
}

void StoreOrder::loadDoc()
{
    m_sqlDrv->prepare("select action_id, doc_date, store_input, store_output,"
                      "e.fname || ' ' || e.lname, admited_id, passed_id, provider_id, "
                      "amount, comment, payment_method "
                      "from st_documents st, employes e "
                      "where editor_id=e.id and st.id=:id");
    m_sqlDrv->bind(":id", m_docNum.toInt());
    m_sqlDrv->execSQL();
    if (m_sqlDrv->next()) {
        ui->cbAction->setIndexOfData(m_sqlDrv->val().toInt());
        ui->deDocDate->setDate(m_sqlDrv->val().toDate());
        ui->cbStoreInputName->setIndexOfData(m_sqlDrv->val().toInt());
        ui->cbStoreOutputName->setIndexOfData(m_sqlDrv->val().toInt());
        ui->leOperator->setText(m_sqlDrv->val().toString());
        ui->cbAdmited->setIndexOfData(m_sqlDrv->val().toInt());
        ui->cbPassed->setIndexOfData(m_sqlDrv->val().toInt());
        ui->cbPartnerName->setIndexOfData(m_sqlDrv->val().toInt());
        ui->leAmount->setText(m_sqlDrv->val().toString());
        ui->leComment->setText(m_sqlDrv->val().toString());
    }

    m_sqlDrv->prepare("select sdd.goods_id, f.name, sdd.qty, fu.name, iif(sdd.qty>0, sdd.amount/sdd.qty, 0), sdd.amount "
                      "from st_draft sdd, food_names f, food_units fu "
                      "where sdd.goods_id=f.id and f.unit_id=fu.id and sdd.doc_id=:doc_id "
                      "order by f.name ");
    m_sqlDrv->bind(":doc_id", m_docNum.toInt());
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
//        if (m_sqlDrv->valFloat("QTY") < 0.001) {
//            continue;
//        }
        int row = Toolkit::newRow(ui->tableWidget);
        for (int i = 0; i < ui->tableWidget->columnCount(); i++)
            ui->tableWidget->item(row, i)->setData(Qt::DisplayRole, m_sqlDrv->val());
    }

    m_sqlDrv->close();
}

bool StoreOrder::eventFilter(QObject *object, QEvent *event)
{
    if (object == m_tableCellEdit) {
        if (!m_tableCellEdit)
            return true;
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            switch (keyEvent->key()) {
            case Qt::Key_Up:
                if (m_dock->isVisible())
                    m_dock->rowUp();
                else
                    Toolkit::rowUp(ui->tableWidget);
                return true;
            case Qt::Key_Down:
                if (m_dock->isVisible())
                    m_dock->rowDown();
                else {
                    if (ui->tableWidget->currentRow() == ui->tableWidget->rowCount() - 1)
                        on_btnAppendDish_clicked();
                    else
                        Toolkit::rowDown(ui->tableWidget);
                }
                return true;
            case Qt::Key_Left:
                Toolkit::columnLeft(ui->tableWidget);
                return true;
            case Qt::Key_Right:
                Toolkit::columnRight(ui->tableWidget);
                return true;
            case Qt::Key_Enter:
            case Qt::Key_Return:
                if (m_dock->isVisible())
                    m_dock->tryClick();
                else
                    on_btnAppendDish_clicked();
                return true;
            case Qt::Key_Escape:
                if (m_dock->isVisible())
                    m_dock->hide();
                return true;
            }
        }
    }
    if (object == ui->tableWidget) {
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            switch (keyEvent->key()) {
            case Qt::Key_Up:
                Toolkit::rowUp(ui->tableWidget);
                return true;
            case Qt::Key_Down:
                if (ui->tableWidget->currentRow() == ui->tableWidget->rowCount() - 1)
                    on_btnAppendDish_clicked();
                else
                    Toolkit::rowDown(ui->tableWidget);
                return true;
            case Qt::Key_Left:
                Toolkit::columnLeft(ui->tableWidget);
                return true;
            case Qt::Key_Right:
                Toolkit::columnRight(ui->tableWidget);
                return true;
            case Qt::Key_Escape:
                if (m_dock->isVisible())
                    m_dock->hide();
                return true;
            }
        }
    }

    return QWidget::eventFilter(object, event);
}

void StoreOrder::dockAccepted(int id)
{
    m_dock->hide();
    int row = Toolkit::currentRow(ui->tableWidget, row);
    if (row < 0)
        return;
    Food &f = m_foods[id];
    if (m_tableCellEdit)
        m_tableCellEdit->disconnect();
    ui->tableWidget->item(row, 0)->setText(its(f.id));
    ui->tableWidget->item(row, 1)->setText(f.name);
    ui->tableWidget->item(row, 3)->setText(f.unit);
    double qty = 0, price = 0, amount = 0;
    setFoodData(row, qty, price, amount);
    ui->tableWidget->setCurrentCell(row, 2);
}

void StoreOrder::codeTextChanged(const QString &text)
{
    if (!text.length()) {
        m_dock->hide();
        return;
    }

    m_dock->setCodeFilter(text);
    m_dock->setPos(qobject_cast<QWidget*>(sender()));
    m_dock->show();
}

void StoreOrder::nameTextChanged(const QString &text)
{
    if (!text.length()) {
        m_dock->hide();
        return;
    }

    m_dock->setNameFilter(text);
    m_dock->setPos(qobject_cast<QWidget*>(sender()));
    m_dock->show();
}

void StoreOrder::qtyTextChanged(const QString &text)
{
    int row = Toolkit::currentRow(ui->tableWidget, row);
    if (row < 0)
        return;
    double qty, price, amount;
    getFoodData(row, qty, price, amount);
    qty = text.toDouble();
    amount = qty * price;
    setFoodData(row, qty, price, amount);
}

void StoreOrder::priceTextChanged(const QString &text)
{
    int row = Toolkit::currentRow(ui->tableWidget, row);
    if (row < 0)
        return;
    double qty, price, amount;
    getFoodData(row, qty, price, amount);
    price = text.toDouble();
    amount = qty * price;
    setFoodData(row, qty, price, amount);
}

void StoreOrder::amountTextChanged(const QString &text)
{
    int row = Toolkit::currentRow(ui->tableWidget, row);
    if (row < 0)
        return;
    double qty, price, amount;
    getFoodData(row, qty, price, amount);
    amount = text.toDouble();
    if (qty > 0)
        price = amount / qty;
    setFoodData(row, qty, price, amount);
}


void StoreOrder::on_btnAppendDish_clicked()
{
    int row = Toolkit::newRow(ui->tableWidget);
    ui->tableWidget->setCurrentCell(row, 0);
}

void StoreOrder::on_tableWidget_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    switch (previousColumn) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5: {
        QWidget *w = ui->tableWidget->cellWidget(previousRow, previousColumn);
        w->removeEventFilter(this);
        ui->tableWidget->removeCellWidget(previousRow, previousColumn);
        if (w) {
            delete w;
            w = 0;
        }
        if (m_tableCellEdit)
            m_tableCellEdit = 0;
        break;
    }
    }

    switch (currentColumn) {
    case 0:
    case 1:
    case 2:
    case 4:
    case 5: {
        QLineEdit *l = new QLineEdit();
        l->setFrame(false);
        m_tableCellEdit = l;
        m_tableCellEdit->installEventFilter(this);
        l->setText(ui->tableWidget->item(currentRow, currentColumn)->text());
        l->selectAll();
        switch (currentColumn) {
        case 0:
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(codeTextChanged(QString)));
            break;
        case 1:
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(nameTextChanged(QString)));
            break;
        case 2:
            l->setValidator(new QDoubleValidator(0, 99999999, 2));
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(qtyTextChanged(QString)));
            break;
        case 4:
            l->setValidator(new QDoubleValidator(0, 99999999, 2));
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(priceTextChanged(QString)));
            break;
        case 5:
            l->setValidator(new QDoubleValidator(0, 99999999, 2));
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(amountTextChanged(QString)));
            break;
        }
        ui->tableWidget->setCellWidget(currentRow, currentColumn, l);
        l->setFocus();
        break;
    }
    }
}

void StoreOrder::on_cbAction_currentIndexChanged(int index)
{
    ui->cbStoreInputName->setVisible(false);
    ui->lbStoreInput->setVisible(false);
    ui->cbStoreOutputName->setVisible(false);
    ui->lbStoreOutput->setVisible(false);

    if (index < 0)
        return;

    int actionId = ui->cbAction->itemData(index).toInt();
    switch (actionId) {
    case DOC_ARTADRAMAS:
    case DOC_INPUT:
    case DOC_INVERNTORIZATION:
        ui->cbStoreInputName->setVisible(true);
        ui->lbStoreInput->setVisible(true);
        break;
    case DOC_OUTPUT:
    case DOC_SALE:
        ui->cbStoreOutputName->setVisible(true);
        ui->lbStoreOutput->setVisible(true);
        break;
    case DOC_MOVEMENT:
    case DOC_COMPLECT_IN:
    case DOC_COMPLECT_OUT:
        ui->cbStoreInputName->setVisible(true);
        ui->lbStoreInput->setVisible(true);
        ui->cbStoreOutputName->setVisible(true);
        ui->lbStoreOutput->setVisible(true);
        break;
    }
}

void StoreOrder::on_btnRemoveDish_clicked()
{
    QModelIndexList l = ui->tableWidget->selectionModel()->selectedRows();
    if (!l.count()) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    ui->tableWidget->clearFocus();
    ui->tableWidget->setCurrentCell(l.at(0).row(), 3, QItemSelectionModel::Select);

    for (int i = l.count() - 1; i > -1; i--)
        ui->tableWidget->removeRow(l.at(i).row());

    countAmount();
}
