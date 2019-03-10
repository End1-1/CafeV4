#include "dlgfoodcache.h"
#include "ui_dlgfoodcache.h"
#include "core.h"
#include <QMenu>

QList<Food> DlgFoodCache::m_foodCache;

DlgFoodCache::DlgFoodCache(QWidget *parent) :
    QDlgBase(parent),
    ui(new Ui::DlgFoodCache)
{
    ui->setupUi(this);

    QList<int> widths;
    widths << 120 << 300 << 80 << 80;
    for (int i = 0; i < ui->tblGoods->columnCount(); i++)
        ui->tblGoods->setColumnWidth(i, widths.at(i));

    if (!m_foodCache.count())
        refreshFoodCache();

    prepareTable();
    ui->tblGoods->installEventFilter(this);
    textChangedCode("");
}

DlgFoodCache::~DlgFoodCache()
{
    delete ui;
}

void DlgFoodCache::btnOK()
{
    accept();
}

void DlgFoodCache::refreshFoodCache()
{
    m_foodCache.clear();
    m_sqlDriver->execSQL("select fn.id, fn.name, fu.name, fn.price "
                         "from food_names fn, food_units fu "
                         "where fn.unit_id=fu.id ");
    while (m_sqlDriver->next()) {
        Food f;
        f.id = m_sqlDriver->val().toInt();
        f.name = m_sqlDriver->val().toString();
        f.unit = m_sqlDriver->val().toString();
        f.price = m_sqlDriver->val().toString();
        m_foodCache.append(f);
    }
    m_sqlDriver->close();
}

void DlgFoodCache::prepareTable()
{
    Toolkit::newRow(ui->tblGoods);
    QLineEdit *leCode = new QLineEdit(ui->tblGoods);
    leCode->setFrame(false);
    connect(leCode, SIGNAL(textChanged(QString)), this, SLOT(textChangedCode(QString)));
    ui->tblGoods->setCellWidget(0, 0, leCode);

    QLineEdit *leName = new QLineEdit(ui->tblGoods);
    leName->setFrame(false);
    connect(leName, SIGNAL(textChanged(QString)), this, SLOT(textChangedName(QString)));
    ui->tblGoods->setCellWidget(0, 1, leName);
    leName->setFocus();
}

void DlgFoodCache::foodToTable()
{
    ui->tblGoods->setRowCount(m_proxyFood.count() + 1);
    for (int i = 0; i < m_proxyFood.count(); i++) {
        ui->tblGoods->setItem(i + 1, 0, new QTableWidgetItem(its(m_proxyFood.at(i).id)));
        ui->tblGoods->setItem(i + 1, 1, new QTableWidgetItem(m_proxyFood.at(i).name));
        ui->tblGoods->setItem(i + 1, 2, new QTableWidgetItem(m_proxyFood.at(i).unit));
        QTableWidgetItem *item = new QTableWidgetItem();
        item->setData(Qt::DisplayRole, m_proxyFood.at(i).price.toDouble());
        ui->tblGoods->setItem(i + 1, 3, item);
    }
}

void DlgFoodCache::tryAccept()
{
    QModelIndexList l = ui->tblGoods->selectionModel()->selectedRows();
    if (!l.count())
        return;
    if (l.at(0).row() == 0)
        return;

    m_result.id = ui->tblGoods->item(l.at(0).row(), 0)->text().toInt();
    m_result.name = ui->tblGoods->item(l.at(0).row(), 1)->text();
    m_result.unit = ui->tblGoods->item(l.at(0).row(), 2)->text();
    m_result.price = ui->tblGoods->item(l.at(0).row(), 3)->text();

    accept();
}

bool DlgFoodCache::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->tblGoods) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *k = static_cast<QKeyEvent*>(e);
            switch (k->key()) {
            case Qt::Key_Return:
            case Qt::Key_Enter:
                tryAccept();
                return true;
            }
        }
    }

    return QDlgBase::eventFilter(o, e);
}

void DlgFoodCache::textChangedCode(const QString &text)
{
    QMap<int, Food> temp;
    for (QList<Food>::const_iterator i = m_foodCache.begin(); i != m_foodCache.end(); i++) {
        QString code = its(i->id);
        if ((code.left(text.length()) == text) || !text.length())
            temp[i->id] = *i;
    }
    builList<int>(temp);
}

void DlgFoodCache::textChangedName(const QString &text)
{
    QMap<QString, Food> temp;
    for (QList<Food>::const_iterator i = m_foodCache.begin(); i != m_foodCache.end(); i++) {
        if ((!i->name.left(text.length()).compare(text, Qt::CaseInsensitive)) || !text.length())
            temp[i->name] = *i;
    }
    builList<QString>(temp);
}

void DlgFoodCache::on_tblGoods_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index)
    tryAccept();
}

void DlgFoodCache::on_tblGoods_customContextMenuRequested(const QPoint &pos)
{
    QMenu *m = new QMenu(this);
    m->addAction(tr("Refresh"), this, SLOT(refreshFoodCache()));
    m->popup(ui->tblGoods->mapToGlobal(pos));
}
