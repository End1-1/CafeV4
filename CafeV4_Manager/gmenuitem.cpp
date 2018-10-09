#include "gmenuitem.h"
#include "ui_gmenuitem.h"
#include <QColorDialog>
#include "qsqlcache.h"
#include "../core/toolkit.h"
#include "core.h"
#include "dlgfoodcache.h"
#include <QClipboard>
#include <QMenu>
#include <QFileDialog>
#include "qnet.h"
#include "qdlgrecipehistory.h"

int GMenuItem::m_lastTypeIndex;
int GMenuItem::m_lastPaymentIndex;
int GMenuItem::m_lastColor = 878787;
int GMenuItem::m_lastTab = 0;

GMenuItem::GMenuItem(int id, QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::GMenuItem)
{
    ui->setupUi(this);
    setWindowTitle(tr("New dish"));
    m_mdiButton->setIcon(QIcon(":/img/recipe.png"));
    m_id = id;

    if (!id) {
        ui->cbType->setCurrentIndex(m_lastTypeIndex);
        ui->cbPayment->setCurrentIndex(m_lastPaymentIndex);
        if (m_lastColor == 878787)
            m_lastColor = -1;
        ui->leColor->setText(QString::number(m_lastColor));
    } else
        ui->leCode->setText(QString::number(id));
    ui->tabWidget->setCurrentIndex(m_lastTab);

    /* From menu item */
    m_actions << "actionSave" << "actionSearch" << "actionNew";

    m_cbStore = 0;
    m_cbPrint = 0;
    m_lePrice = 0;

    ui->gridMenu->setColumnCount(7);
    ui->gridMenu->setColumnWidth(0, 25);
    ui->gridMenu->setColumnWidth(1, 150);
    ui->gridMenu->setColumnWidth(2, 80);
    ui->gridMenu->setColumnWidth(3, 150);
    ui->gridMenu->setColumnWidth(4, 150);
    ui->gridMenu->setColumnWidth(5, 100);
    ui->gridMenu->setColumnWidth(6, 100);

    loadMenu();

    ui->tblGoods->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tblGoods, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showRecipeMenu(QPoint)));

    ui->tblTotal->setRowCount(1);
    for (int i = 0; i < ui->tblTotal->columnCount(); i++)
        ui->tblTotal->setItem(0, i, new QTableWidgetItem());

    ui->tabWidget->setCurrentIndex(m_lastTab);
    if (QSystem::WebUrl.length())
        ui->lbWebInfo->setText(QSystem::WebUrl);
    ui->btnLoadImage->setEnabled(QSystem::WebUrl.length());
}

GMenuItem::~GMenuItem()
{
    m_lastTab = ui->tabWidget->currentIndex();
    delete ui;
}

void GMenuItem::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    GMenuItem *i = new GMenuItem(0, this);
    i->show();
}

void GMenuItem::actionSave()
{
    CHECK_VIEWER_AND_MAINDB
    if (!checkOk())
        return;

    bool update = m_id > 0;
    if (!m_sqlDrv->openDB()) {
        QMessageBox::critical(this, tr("SQL error"), tr("Cannot connect to database"));
        return;
    }

    /* Step one */
    if (!m_id) {
        m_id = m_sqlDrv->genId("GEN_ME_DISHES_ID");
        ui->leCode->setText(QString::number(m_id));
        m_sqlDrv->prepare("insert into me_dishes (id, type_id, name, payment_mod, color, queue, remind, img_link, descr) "
                          "values "
                          "(:id, :type_id, :name, :payment_mod, :color, :queue, :remid, :img_link, :descr)");
    } else {
        m_sqlDrv->prepare("update me_dishes set type_id=:type_id, name=:name, payment_mod=:payment_mod, color=:color,"
                          "queue=:queue, remind=:remind, img_link=:img_link, descr=:descr, rdish=:rdish, rprice=:rprice where id=:id");
    }
    m_sqlDrv->bind(":id", m_id);
    m_sqlDrv->bind(":type_id", ui->cbType->currentItemData());
    m_sqlDrv->bind(":name", ui->leName->text());
    m_sqlDrv->bind(":payment_mod", ui->cbPayment->currentItemData());
    m_sqlDrv->bind(":color", ui->leColor->text().toInt());
    m_sqlDrv->bind(":queue", ui->leQueue->text().toInt());
    m_sqlDrv->bind(":remind", ui->chRemind->isChecked());
    m_sqlDrv->bind(":img_link", m_imgLink);
    m_sqlDrv->bind(":descr", ui->leDescription->text());
    m_sqlDrv->bind(":rdish", ui->leSecondCode->text().toInt());
    m_sqlDrv->bind(":rprice", ui->leRPrice->text());
    if (!m_sqlDrv->execSQL()) {
        if (!update)
            m_id = 0;
        return;
    }
    setWindowTitle(ui->leName->text());

    /* Step two */
    if (!m_sqlDrv->prepare("delete from me_dishes_menu where dish_id=:dish_id"))
        return;
    m_sqlDrv->bind(":dish_id", m_id);
    if (!m_sqlDrv->execSQL())
        return;
    if (!m_sqlDrv->prepare("insert into me_dishes_menu (menu_id, dish_id, price, print_schema, store_id, state_id, print1, print2) "
                           "values (:menu_id, :dish_id, :price, :print_schema, :store_id, :state_id, :print1, :print2)"))
        return;
    for (int i = 0; i < ui->gridMenu->rowCount(); i++) {
        if (ui->gridMenu->item(i, 0)->checkState() == Qt::Checked) {
            if (!ui->gridMenu->item(i, 4)->data(Qt::UserRole).toInt()) {
                m_sqlDrv->rollback();
                QMessageBox::critical(this, tr("Error"), tr("Printer schema is not defined"));
                return;
            }
            if (!ui->gridMenu->item(i, 3)->data(Qt::UserRole).toInt()) {
                m_sqlDrv->rollback();
                QMessageBox::critical(this, tr("Error"), tr("Storage is not defined"));
                return;
            }
            if (!ui->gridMenu->item(i, 2)->data(Qt::DisplayRole).toDouble()) {
                m_sqlDrv->rollback();
                QMessageBox::critical(this, tr("Error"), tr("Price is not defined"));
                return;
            }
        }
        m_sqlDrv->bind(":menu_id", ui->gridMenu->item(i, 1)->data(Qt::UserRole));
        m_sqlDrv->bind(":dish_id", m_id);
        m_sqlDrv->bind(":price", ui->gridMenu->item(i, 2)->data(Qt::DisplayRole).toFloat());
        m_sqlDrv->bind(":print_schema", ui->gridMenu->item(i, 4)->data(Qt::UserRole).toInt());
        m_sqlDrv->bind(":store_id", ui->gridMenu->item(i, 3)->data(Qt::UserRole).toInt());
        m_sqlDrv->bind(":state_id", (int)(ui->gridMenu->item(i, 0)->checkState() == Qt::Checked));
        m_sqlDrv->bind(":print1", ui->gridMenu->item(i, 5)->data(Qt::DisplayRole).toString());
        m_sqlDrv->bind(":print2", ui->gridMenu->item(i, 6)->data(Qt::DisplayRole).toString());
        if (!m_sqlDrv->execSQL())
            return;
        }

    /* Recipe */
    m_sqlDrv->prepare("execute procedure backup_recipes(:user_id, :dish_id)");
    m_sqlDrv->bind(":user_id", ___ff_user->id);
    m_sqlDrv->bind(":dish_id", m_id);
    m_sqlDrv->execSQL();
    m_sqlDrv->prepare("delete from me_recipes where dish_id=:dish_id");
    m_sqlDrv->bind(":dish_id", m_id);
    m_sqlDrv->execSQL();
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        m_sqlDrv->prepare("insert into me_recipes (id, dish_id, goods_id, qty, dish_recipe, dish_qty, price) "
                          "values (null, :dish_id, :goods_id, :qty, 0, 0, 0)");
        m_sqlDrv->bind(":dish_id", m_id);
        m_sqlDrv->bind(":goods_id", ui->tblGoods->item(i, 0)->data(Qt::DisplayRole));
        m_sqlDrv->bind(":qty", ui->tblGoods->item(i, 2)->data(Qt::DisplayRole));
        m_sqlDrv->execSQL();
    }
    m_sqlDrv->close();
    uploadImageToServer();
    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void GMenuItem::actionSearch()
{
    QDlgRecipeHistory *rh = new QDlgRecipeHistory(m_id, this);
    rh->exec();
    delete rh;
}

void GMenuItem::setTabIndex(int index)
{
    m_lastTab = index;
    ui->tabWidget->setCurrentIndex(index);
}

void GMenuItem::storeChange(int index)
{
    int row = Toolkit::currentRow(ui->gridMenu, row);
    if (row < 0)
        return;

    ui->gridMenu->item(row, 3)->setData(Qt::DisplayRole, m_cbStore->currentText());
    ui->gridMenu->item(row, 3)->setData(Qt::UserRole, m_cbStore->itemData(index));
}

void GMenuItem::printChange(int index)
{
    int row = Toolkit::currentRow(ui->gridMenu, row);
    if (row < 0)
        return;


    ui->gridMenu->item(row, 4)->setData(Qt::DisplayRole, m_cbPrint->currentText());
    ui->gridMenu->item(row, 4)->setData(Qt::UserRole, m_cbPrint->itemData(index));
}

void GMenuItem::priceChange(const QString &str)
{
    int row = Toolkit::currentRow(ui->gridMenu, row);
    if (row < 0)
        return;

    ui->gridMenu->item(row, 2)->setData(Qt::DisplayRole, str);
}

void GMenuItem::print1Change(const QString &str)
{
    int row = Toolkit::currentRow(ui->gridMenu, row);
    if (row < 0)
        return;

    ui->gridMenu->item(row, 5)->setData(Qt::DisplayRole, str);
}

void GMenuItem::print2Change(const QString &str)
{
    int row = Toolkit::currentRow(ui->gridMenu, row);
    if (row < 0)
        return;

    ui->gridMenu->item(row, 6)->setData(Qt::DisplayRole, str);
}

void GMenuItem::textChangedQty(const QString &text)
{
    QCellLineEdit *l = static_cast<QCellLineEdit*>(sender());
    double price = ui->tblGoods->item(l->m_row, 4)->text().toDouble();
    double amount = text.toDouble() * price;
    ui->tblGoods->item(l->m_row, 5)->setText(dts(amount));
    countTotal();
}

void GMenuItem::on_btnColor_clicked()
{
    QColor color = QColorDialog::getColor(QColor::fromRgb(ui->leColor->text().toInt()), this, tr("Color for dish"));
    ui->leColor->setText(QString::number((int)color.rgb()));
}

bool GMenuItem::checkOk()
{
    QString errorMsg;
    ui->leName->setText(ui->leName->text().trimmed());
    if (!ui->leName->text().length())
        errorMsg += tr("Name is empty") + "\n";

    if (ui->cbType->currentIndex() < 0)
        errorMsg += tr("Type is not selected") + "\n";

    if (errorMsg.length()) {
        QMessageBox::critical(this, tr("Error"), errorMsg);
        return false;
    }

    m_lastTypeIndex = ui->cbType->currentIndex();
    m_lastPaymentIndex = ui->cbPayment->currentIndex();
    m_lastColor = ui->leColor->text().toInt();

    return true;
}


void GMenuItem::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Insert)
        on_actionRNew_row_triggered();
    else if (e->key() == Qt::Key_Delete)
        on_actionRDelete_row_triggered();
    else if (e->matches(QKeySequence::Copy))
        on_actionRCopy_recipe_triggered();
    else if (e->matches(QKeySequence::Paste))
        on_actionRPaste_recipe_triggered();
}



void GMenuItem::loadMenu()
{
    QSqlCache c;
    QMap<int, QString> &c_menu = c.getCache("ME_MENUS");
    QMap<int, QString> &c_store = c.getCache("ST_STORAGES");
    QMap<int, QString> &c_print_schema = c.getCache("ME_PRINTER_SCHEMA");

    ui->gridMenu->setRowCount(c_menu.count());
    int row = 0;
    for (QMap<int, QString>::const_iterator it = c_menu.begin(); it != c_menu.end(); it++) {
        QTableWidgetItem *itemCheck = new QTableWidgetItem();
        itemCheck->setData(Qt::UserRole, it.key());
        itemCheck->setFlags(itemCheck->flags() | (Qt::ItemIsSelectable | Qt::ItemIsUserCheckable));
        itemCheck->setCheckState(Qt::Unchecked);
        ui->gridMenu->setItem(row, 0, itemCheck);
        QTableWidgetItem *menuItem = new QTableWidgetItem(it.value());
        menuItem->setData(Qt::UserRole, it.key());
        ui->gridMenu->setItem(row, 1, menuItem);
        ui->gridMenu->setItem(row, 2, new QTableWidgetItem());
        ui->gridMenu->setItem(row, 3, new QTableWidgetItem());
        ui->gridMenu->setItem(row, 4, new QTableWidgetItem());
        ui->gridMenu->setItem(row, 5, new QTableWidgetItem());
        ui->gridMenu->setItem(row, 6, new QTableWidgetItem());
        row++;
    }

    if (!m_sqlDrv->prepare("select menu_id, price, store_id, print_schema, state_id, print1, print2 from me_dishes_menu where dish_id=:dish_id"))
        return;
    m_sqlDrv->bind(":dish_id", m_id);
    if (!m_sqlDrv->execSQL())
        return;
    while (m_sqlDrv->m_query->next()) {
        row = -1;
        for (int i = 0; i < ui->gridMenu->rowCount(); i++) {
            QTableWidgetItem *menuItem = ui->gridMenu->item(i, 0);
            if (menuItem->data(Qt::UserRole).toInt() == m_sqlDrv->m_query->value("MENU_ID").toInt()) {
                row = i;
                break;
            }
        }
        if (row < 0)
            continue;
        QTableWidgetItem *item = ui->gridMenu->item(row, 0);
        item->setCheckState(m_sqlDrv->m_query->value("STATE_ID").toInt() ? Qt::Checked : Qt::Unchecked);
        item = ui->gridMenu->item(row, 2);
        item->setData(Qt::DisplayRole, Toolkit::formatDouble(m_sqlDrv->m_query->value("PRICE").toDouble()));
        item = ui->gridMenu->item(row, 3);
        item->setData(Qt::DisplayRole, c_store.value(m_sqlDrv->m_query->value("STORE_ID").toInt()));
        item->setData(Qt::UserRole, m_sqlDrv->m_query->value("STORE_ID").toInt());
        item = ui->gridMenu->item(row, 4);
        item->setData(Qt::DisplayRole, c_print_schema.value(m_sqlDrv->m_query->value("PRINT_SCHEMA").toInt()));
        item->setData(Qt::UserRole, m_sqlDrv->m_query->value("PRINT_SCHEMA").toInt());
        item = ui->gridMenu->item(row, 5);
        item->setData(Qt::DisplayRole, m_sqlDrv->m_query->value("PRINT1").toString());
        item = ui->gridMenu->item(row, 6);
        item->setData(Qt::DisplayRole, m_sqlDrv->m_query->value("PRINT2").toString());
    }
    m_sqlDrv->close();

    SETTINGS
    qDebug() << s.value("gmenuitem_payment", 0).toInt();
    ui->cbPayment->setIndexOfData(s.value("gmenuitem_payment", 0).toInt());
    ui->cbType->setIndexOfData(s.value("gmenuitem_type", 0).toInt());

    if (!m_id)
        return;

    if (!m_sqlDrv->openDB())
        return;

    m_sqlDrv->prepare("select md.type_id, md.name, md.color, md.queue, md.payment_mod, md.remind, md.img_link, md.descr, md.rdish, "
                      "md.rprice "
                      "from me_dishes md where md.id=:id");
    m_sqlDrv->bind(":id", m_id);
    m_sqlDrv->execSQL();
    if (m_sqlDrv->next()) {
        ui->cbType->setIndexOfData(m_sqlDrv->val().toInt());
        ui->leName->setText(m_sqlDrv->val().toString());
        setWindowTitle(ui->leName->text());
        ui->leColor->setText(QString::number(m_sqlDrv->val().toInt()));
        ui->leQueue->setText(m_sqlDrv->val().toString());
        ui->cbPayment->setIndexOfData(m_sqlDrv->val().toInt());
        ui->chRemind->setChecked(m_sqlDrv->val().toBool());
        m_imgLink = m_sqlDrv->val().toString();
        ui->leDescription->setText(m_sqlDrv->val().toString());
        ui->leSecondCode->setText(m_sqlDrv->val().toString());
        ui->leRPrice->setText(m_sqlDrv->val().toString());
    }
    m_sqlDrv->close();

    loadRecipe();
    downloadImageFromServer();
}

void GMenuItem::loadRecipe()
{
    QList<int> widths;
    widths << 100 << 250 << 100 << 100 << 100 << 100;
    for (int i = 0; i < widths.count(); i++) {
        ui->tblGoods->setColumnWidth(i, widths.at(i));
        ui->tblTotal->setColumnWidth(i, widths.at(i));
        ui->tblTotal->setItem(0, i, new QTableWidgetItem(""));
    }
    ui->tblGoods->installEventFilter(this);

    m_sqlDrv->prepare("select mr.goods_id, fn.name, mr.qty, fu.name, fn.price, mr.qty*fn.price "
                      "from me_recipes mr, food_names fn, food_units fu "
                      "where mr.goods_id=fn.id and fn.unit_id=fu.id and mr.dish_id=:dish_id");
    m_sqlDrv->bind(":dish_id", m_id);
    m_sqlDrv->execSQL();
    double qty = 0, amount = 0;
    while (m_sqlDrv->next()) {
        int newRow = Toolkit::newRow(ui->tblGoods);
        for (int i = 0; i < ui->tblGoods->columnCount(); i++) {
            ui->tblGoods->item(newRow, i)->setData(Qt::DisplayRole, m_sqlDrv->val());
            if (i == 2)
                ui->tblGoods->item(newRow, i)->setData(Qt::UserRole, 3);
        }
       qty += ui->tblGoods->item(newRow, 2)->text().toDouble();
       amount += ui->tblGoods->item(newRow, 5)->text().toDouble();
    }

    Toolkit::newRow(ui->tblTotal);
    ui->tblTotal->item(0, 2)->setText(dts(qty));
    ui->tblTotal->item(0, 5)->setText(dts(amount));

    m_sqlDrv->close();
}

void GMenuItem::countTotal()
{
    double totalAmount = 0, totalQty = 0;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (ui->tblGoods->item(i, 2))
            totalQty += ui->tblGoods->item(i, 2)->text().toDouble();
        if (ui->tblGoods->item(i, 5))
            totalAmount += ui->tblGoods->item(i, 5)->text().toDouble();
    }
    ui->tblTotal->item(0, 2)->setText(dts(totalQty));
    ui->tblTotal->item(0, 5)->setText(dts(totalAmount));
}

void GMenuItem::addGoods()
{
    DlgFoodCache *d = new DlgFoodCache(this);
    if (d->exec() == QDialog::Accepted) {
        Food f = d->m_result;
        int row = ui->tblGoods->currentRow();
        if (row < 0)
            return;
        ui->tblGoods->item(row, 0)->setText(its(f.id));
        ui->tblGoods->item(row, 1)->setText(f.name);
        ui->tblGoods->item(row, 2)->setData(Qt::DisplayRole, 0.0);
        ui->tblGoods->item(row, 3)->setText(f.unit);
        ui->tblGoods->item(row, 4)->setData(Qt::DisplayRole, f.price.toDouble());
        ui->tblGoods->item(row, 5)->setData(Qt::DisplayRole, 0.0);
        ui->tblGoods->setCurrentCell(row, 2);
    }
    delete d;
}

void GMenuItem::downloadImageFromServer()
{
    if (!m_imgLink.length())
        return;
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getRawResponse(QByteArray&,bool)), this, SLOT(webRawResponse(QByteArray&,bool)));
    n->downloadRawData(QSystem::WebUrl + "/" + m_imgLink);
}

void GMenuItem::uploadImageToServer()
{
    if (!m_imgPath.length())
        return;
    QFile f(m_imgPath);
    if (!f.open(QIODevice::ReadOnly))
        return;
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), this, SLOT(webResponse(QString,bool)));
    n->URL = QSystem::WebUrl + "/menuimgup.php";

    n->addData("--" + boundary.toLatin1() + "\r\n");
    n->addData("phrase", QSystem::WebPass);
    n->addData("level", "3");
    n->addData("item_id", QString::number(m_id));

    n->addData(QString("Content-Disposition: form-data; name=\"file\"; filename=\"" + m_imgPath + "\";\r\n").toLatin1());
    n->addData("Content-Type: image/png\r\n");
    n->addData("Content-transfer-encoding: binary\r\n\r\n");
    n->addData(f.readAll() + "\r\n");
    n->addData("--" + boundary.toLatin1() + "--\r\n");

    n->go();
}

void GMenuItem::showRecipeMenu(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(ui->actionRNew_row);
    m->addAction(ui->actionRDelete_row);
    m->addAction(ui->actionRCopy_recipe);
    m->addAction(ui->actionRPaste_recipe);
    m->popup(ui->tblGoods->mapToGlobal(pos));
}

void GMenuItem::webResponse(const QString &response, bool isError)
{
    sender()->deleteLater();
    if (isError) {
        QMessageBox::critical(this, tr("Network error"), response);
        return;
    }
}

void GMenuItem::webRawResponse(QByteArray &data, bool isError)
{
    QNet *n = static_cast<QNet*>(sender());
    if (isError) {
        QMessageBox::critical(this, tr("Network error"), data);
    } else {
        QPixmap p;
        p.loadFromData(data);
        QGraphicsScene *s = new QGraphicsScene();
        s->addPixmap(p);
        ui->grImage->setScene(s);
        ui->grImage->fitInView(s->itemsBoundingRect(), Qt::KeepAspectRatio);
    }
    n->deleteLater();
}

void GMenuItem::on_gridMenu_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    if (previous) {
        QWidget *w = ui->gridMenu->cellWidget(previous->row(), previous->column());
        if (w) {
            ui->gridMenu->removeCellWidget(previous->row(), previous->column());
            delete w;
        }
    }

    if (current) {
        QTableWidgetItem *item = ui->gridMenu->item(current->row(), 0);
        if (item->checkState() != Qt::Checked)
            return;

        switch (current->column()) {
        case 2:
            m_lePrice = new QLineEdit();
            m_lePrice->setValidator(new QDoubleValidator(0, 1000000, 2));
            ui->gridMenu->setCellWidget(current->row(), current->column(), m_lePrice);
            m_lePrice->setText(ui->gridMenu->item(current->row(), 2)->data(Qt::DisplayRole).toString());
            m_lePrice->setFrame(false);
            m_lePrice->setFocus();
            connect(m_lePrice, SIGNAL(textChanged(QString)), SLOT(priceChange(QString)));
            break;
        case 3:
            m_cbStore = new QPComboBox();
            m_cbStore->setProperty("fieldId", "ID");
            m_cbStore->setProperty("fieldName", "NAME");
            m_cbStore->setProperty("table", "ST_STORAGES");
            ui->gridMenu->setCellWidget(current->row(), current->column(), m_cbStore);
            m_cbStore->setCurrentIndex(m_cbStore->findText(ui->gridMenu->item(current->row(), current->column())->data(Qt::DisplayRole).toString()));
            m_cbStore->setFrame(false);
            m_cbStore->setFocus();
            connect(m_cbStore, SIGNAL(currentIndexChanged(int)), SLOT(storeChange(int)));
            break;
        case 4:
            m_cbPrint = new QPComboBox();
            m_cbPrint->setProperty("fieldId", "ID");
            m_cbPrint->setProperty("fieldName", "NAME");
            m_cbPrint->setProperty("table", "ME_PRINTER_SCHEMA");
            ui->gridMenu->setCellWidget(current->row(), current->column(), m_cbPrint);
            m_cbPrint->setCurrentIndex(m_cbPrint->findText(ui->gridMenu->item(current->row(), current->column())->data(Qt::DisplayRole).toString()));
            m_cbPrint->setFrame(false);
            m_cbPrint->setFocus();
            connect(m_cbPrint, SIGNAL(currentIndexChanged(int)), SLOT(printChange(int)));
            break;
        case 5:
        {
            QPComboBox *c = new QPComboBox();
            c->addItem("");
            c->addItem("b1");
            c->addItem("bar");
            c->addItem("kit");
            c->addItem("local");
            ui->gridMenu->setCellWidget(current->row(), current->column(), c);
            c->setCurrentIndex(c->findText(ui->gridMenu->item(current->row(), current->column())->data(Qt::DisplayRole).toString()));
            connect(c, SIGNAL(currentTextChanged(QString)), this, SLOT(print1Change(QString)));
            break;
        }
        case 6:
        {
            QPComboBox *c = new QPComboBox();
            c->addItem("");
            c->addItem("b1");
            c->addItem("bar");
            c->addItem("kit");
            c->addItem("local");
            ui->gridMenu->setCellWidget(current->row(), current->column(), c);
            c->setCurrentIndex(c->findText(ui->gridMenu->item(current->row(), current->column())->data(Qt::DisplayRole).toString()));
            connect(c, SIGNAL(currentTextChanged(QString)), this, SLOT(print2Change(QString)));
            break;
        }
        }
    }
}

void GMenuItem::on_tblGoods_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    switch(previousColumn) {
    case 2:
        QWidget *w = ui->tblGoods->cellWidget(previousRow, previousColumn);
        if (w) {
            ui->tblGoods->removeCellWidget(previousRow, previousColumn);
            delete w;
            w = 0;
        }
        break;
    }

    switch (currentColumn) {
    case 2:
        QCellLineEdit *l = new QCellLineEdit(currentColumn, currentRow, ui->tblGoods);
        switch (currentColumn) {
        case 2:
            l->setValidator(new QDoubleValidator(0, 999, 3));
            connect(l, SIGNAL(textChanged(QString)), this, SLOT(textChangedQty(QString)));
            l->setText(QString::number(ui->tblGoods->item(currentRow, currentColumn)->data(Qt::DisplayRole).toDouble(), 'f', 3));
            l->selectAll();
            l->setFocus();
            break;
        }
        break;
    }
}

QCellLineEdit::QCellLineEdit(int col, int row, QWidget *parent) :
    QLineEdit(parent)
{
    m_col = col;
    m_row = row;
    m_table = qobject_cast<QTableWidget*>(parent);
    setText(m_table->item(m_row, m_col)->text());
    setFrame(false);
    m_table->setCellWidget(m_row, m_col, this);
    connect(this, SIGNAL(textChanged(QString)), this, SLOT(currentTextChanged(QString)));
}

void QCellLineEdit::currentTextChanged(const QString &text)
{
    m_table->item(m_row, m_col)->setText(text);
}

void QCellLineEdit::focusInEvent(QFocusEvent *e)
{
    Q_UNUSED(e)
    emit focusChanged(true);
}

void QCellLineEdit::focusOutEvent(QFocusEvent *e)
{
    Q_UNUSED(e)
    emit focusChanged(false);
}

void GMenuItem::on_actionRNew_row_triggered()
{
    CHECK_VIEWER_AND_MAINDB
    int row = Toolkit::newRow(ui->tblGoods);
    ui->tblGoods->setCurrentCell(row, 1);
    addGoods();
}

void GMenuItem::on_actionRDelete_row_triggered()
{
    CHECK_VIEWER_AND_MAINDB
    QSet<int> rows;
    if (!selectedRows(ui->tblGoods, rows)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    for (QSet<int>::const_iterator it = rows.begin(); it != rows.end(); it++)
        ui->tblGoods->removeRow(*it);
}

void GMenuItem::on_actionRCopy_recipe_triggered()
{
    QString recipe;
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        QString row;
        for (int j = 0; j < ui->tblGoods->columnCount(); j++) {
            if (ui->tblGoods->item(i, j))
                row += ui->tblGoods->item(i, j)->data(Qt::DisplayRole).toString() + "\t";
        }
        recipe += row + "\r";
    }
    QClipboard *c = qApp->clipboard();
    c->setText(recipe);
}

void GMenuItem::on_actionRPaste_recipe_triggered()
{
    CHECK_VIEWER_AND_MAINDB
    QClipboard *c = qApp->clipboard();
    QString recipe = c->text();
    QStringList rows = recipe.split("\r", QString::SkipEmptyParts);
    for (int i = 0; i < rows.count(); i++) {
        int rowCount = ui->tblGoods->rowCount();
        ui->tblGoods->setRowCount(rowCount + 1);
        QStringList cols = rows.at(i).split("\t", QString::SkipEmptyParts);
        for (int j = 0; j < cols.count(); j++)
            ui->tblGoods->setItem(rowCount, j, new QTableWidgetItem(cols.at(j)));
    }
    countTotal();
}

void GMenuItem::on_btnLoadImage_clicked()
{
    CHECK_VIEWER_AND_MAINDB
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select image"), "", "*.png;*.jpen;*.jpg");
    if (!fileName.length())
        return;
    m_imgPath = fileName;
    QPixmap pixmap(fileName);
    QGraphicsScene *s = new QGraphicsScene();
    s->addPixmap(pixmap);
    ui->grImage->setScene(s);
    ui->grImage->fitInView(s->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void GMenuItem::on_cbType_currentIndexChanged(int index)
{
    SETTINGS
    s.setValue("gmenuitem_type", ui->cbType->itemData(index));
}

void GMenuItem::on_cbPayment_currentIndexChanged(int index)
{
    SETTINGS
    s.setValue("gmenuitem_payment", ui->cbPayment->itemData(index));
}
