#include "dlgrecipe.h"
#include "ui_dlgrecipe.h"
#include "core.h"
#include "dlgfoodcache.h"
#include <QClipboard>
#include "gmenuitem.h"

DlgRecipe::DlgRecipe(int dishId, QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::DlgRecipe),
    m_dishId(dishId)
{
    m_actions << "actionNew" << "actionDelete" << "actionExport_to_Excel" << "actionSave" << "actionCopy" << "actionPaste";
    ui->setupUi(this);
    m_sqlDrv->prepare("select md.name, mt.name from me_dishes md, me_types mt where md.type_id=mt.id and md.id=:id");
    m_sqlDrv->bind(":id", m_dishId);
    m_sqlDrv->execSQL();
    m_sqlDrv->next();

    QString title = m_sqlDrv->val().toString() + ": " + m_sqlDrv->val().toString();
    setWindowTitle(tr("Recipe") + " " + title);
    ui->leName->setText(title);

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
    m_sqlDrv->bind(":dish_id", m_dishId);
    m_sqlDrv->execSQL();
    double qty = 0, amount = 0;
    while (m_sqlDrv->next()) {
        int newRow = Toolkit::newRow(ui->tblGoods);
        for (int i = 0; i < ui->tblGoods->columnCount(); i++)
            ui->tblGoods->item(newRow, i)->setText(m_sqlDrv->val().toString());
       qty += ui->tblGoods->item(newRow, 2)->text().toDouble();
       amount += ui->tblGoods->item(newRow, 5)->text().toDouble();
    }

    Toolkit::newRow(ui->tblTotal);
    ui->tblTotal->item(0, 2)->setText(dts(qty));
    ui->tblTotal->item(0, 5)->setText(dts(amount));

    m_sqlDrv->close();
}

DlgRecipe::~DlgRecipe()
{
    delete ui;
}

void DlgRecipe::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    addRow();
}

void DlgRecipe::actionDelete()
{
    CHECK_VIEWER_AND_MAINDB
    QModelIndexList l = ui->tblGoods->selectionModel()->selectedRows();
    if (!l.count()) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    for (int i = l.count() - 1; i > -1; i--)
        ui->tblGoods->removeRow(l.at(i).row());
}

void DlgRecipe::actionSave()
{
    CHECK_VIEWER_AND_MAINDB
    m_sqlDrv->openDB();
    m_sqlDrv->prepare("delete from me_recipes where dish_id=:dish_id");
    m_sqlDrv->bind(":dish_id", m_dishId);
    m_sqlDrv->execSQL();
    m_sqlDrv->prepare("insert into me_recipes (dish_id, goods_id, qty) values (:dish_id, :goods_id, :qty)");
    for (int i = 0; i < ui->tblGoods->rowCount(); i++) {
        if (!ui->tblGoods->item(i, 0)->text().toInt())
            continue;
        m_sqlDrv->bind(":dish_id", m_dishId);
        m_sqlDrv->bind(":goods_id", ui->tblGoods->item(i, 0)->text().toInt());
        m_sqlDrv->bind(":qty", ui->tblGoods->item(i, 2)->text().toFloat());
        m_sqlDrv->execSQL();
    }
    m_sqlDrv->close();
    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void DlgRecipe::actionCostum(int action)
{
    switch (action) {
    case ACTION_COPY:
        copyRecipe();
        break;
    case ACTION_PASTE:
        CHECK_VIEWER_AND_MAINDB
        pasteRecipe();
        break;
    }
}

void DlgRecipe::textChangedQty(const QString &text)
{
    QCellLineEdit *l = static_cast<QCellLineEdit*>(sender());
    double price = ui->tblGoods->item(l->m_row, 4)->text().toDouble();
    double amount = text.toDouble() * price;
    ui->tblGoods->item(l->m_row, 5)->setText(dts(amount));
    countTotal();
}

void DlgRecipe::on_tblGoods_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
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
            l->setText(ui->tblGoods->item(currentRow, currentColumn)->text());
            l->selectAll();
            l->setFocus();
            break;
        }
        break;
    }
}

void DlgRecipe::countTotal()
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

void DlgRecipe::addRow()
{
    int row = Toolkit::newRow(ui->tblGoods);
    ui->tblGoods->setCurrentCell(row, 1);
    addGoods();
}

void DlgRecipe::addGoods()
{
    DlgFoodCache *d = new DlgFoodCache(this);
    if (d->exec() == QDialog::Accepted) {
        Food f = d->m_result;
        int row = ui->tblGoods->currentRow();
        if (row < 0)
            return;
        ui->tblGoods->item(row, 0)->setText(its(f.id));
        ui->tblGoods->item(row, 1)->setText(f.name);
        ui->tblGoods->item(row, 2)->setText("0");
        ui->tblGoods->item(row, 3)->setText(f.unit);
        ui->tblGoods->item(row, 4)->setText(f.price);
        ui->tblGoods->item(row, 5)->setText("0");
        ui->tblGoods->setCurrentCell(row, 2);
    }
    delete d;
}

void DlgRecipe::copyRecipe()
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

void DlgRecipe::pasteRecipe()
{
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

bool DlgRecipe::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->tblGoods) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *k = static_cast<QKeyEvent*>(e);
            switch (k->key()) {
            case Qt::Key_Return:
            case Qt::Key_Enter:
                addRow();
                return true;
            }
        }
    }
    return QBaseSqlWindow::eventFilter(o, e);
}


