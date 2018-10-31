#include "dlggoods.h"
#include "ui_dlggoods.h"
#include "qsqlcache.h"
#include "core.h"

DlgGoods::DlgGoods(QMap<QString, QList<QVariant> > &values, QWidget *parent) :
    QDlgQuery(values, parent),
    ui(new Ui::DlgGoods)
{
    ui->setupUi(this);
    ui->lePrice->setValidator(new QDoubleValidator(0, 1000000, 2));
    ui->leAutoout->setValidator(new QIntValidator());
    ui->leAutoWaste->setValidator(new QDoubleValidator(0, 10, 2));
    SETTINGS
    ui->cbGroup->setCurrentIndex(s.value("dlggoods_cbgroup").toInt());
    ui->cbUnit->setCurrentIndex(s.value("dlggoods_cbunit").toInt());
}

DlgGoods::~DlgGoods()
{
    delete ui;
}

void DlgGoods::setNext(bool next, const QString &code)
{
    mNext = next;
    mCurrentCode = code;
    if (mNext) {
        if (mCurrentCode.length() > 0)
            ui->leId->setText(QString::number(mCurrentCode.toInt() + 1));
        if (mCurrentCode1.length() > 0) {
            ui->cbGroup->setCurrentIndex(ui->cbGroup->findData(mCurrentCode1));
        }
        if (mCurrentCode2.length() > 0) {
            ui->cbUnit->setCurrentIndex(ui->cbUnit->findData(mCurrentCode2));
        }
        mNext = false;
    }
}

void DlgGoods::btnOK()
{
    QSqlCache::removeChache("FOOD_NAMES");
    m_sqlDriver->openDB();
    QString query("update or insert into food_names (id, group_id, unit_id, name, price, auto_out, auto_waste) "
                  "values (:id, :group_id, :unit_id, :name, :price, :auto_out, :auto_waste) "
                  "matching(id)");
    m_sqlDriver->prepare(query);
    m_sqlDriver->bind(":id", ui->leId->text());
    m_sqlDriver->bind(":group_id", ui->cbGroup->currentItemData());
    m_sqlDriver->bind(":unit_it", ui->cbUnit->currentItemData());
    m_sqlDriver->bind(":name", ui->leName->text());
    m_sqlDriver->bind(":auto_out", ui->leAutoout->text());
    m_sqlDriver->bind(":auto_waste", ui->leAutoWaste->text().toDouble());
    m_sqlDriver->execSQL();
    m_sqlDriver->close();
    QDlgQuery::btnOK();
}

void DlgGoods::on_chNewIdRezim_clicked(bool checked)
{
    ui->leId->setEnabled(checked);
}

void DlgGoods::on_leName_textChanged(const QString &arg1)
{
    QSqlCache sc;
    QMap<int, QString> c = sc.getCache("FOOD_NAMES");
    QStringList g = c.values();
    QStringList founded;
    for (int i = 0; i < g.count(); i++) {
        if (g[i].contains(arg1, Qt::CaseInsensitive))
            founded.append(g[i]);
    }
    ui->lstSimGoods->clear();
    for (QStringList::const_iterator it = founded.begin(); it != founded.end(); it++) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstSimGoods);
        item->setText(*it);
        ui->lstSimGoods->addItem(item);
    }
}

void DlgGoods::on_pushButton_3_clicked()
{
    reject();
}

void DlgGoods::on_pushButton_clicked()
{
    btnOK();
}

void DlgGoods::on_pushButton_2_clicked()
{
    mNext = true;
    mCurrentCode = ui->leId->text();
    mCurrentCode1 = ui->cbGroup->currentData().toString();
    mCurrentCode2 = ui->cbUnit->currentData().toString();
    btnOK();
}

void DlgGoods::on_cbGroup_currentIndexChanged(int index)
{
    SETTINGS
    s.setValue("dlggoods_cbgroup", index);
}

void DlgGoods::on_cbUnit_currentIndexChanged(int index)
{
    SETTINGS
    s.setValue("dlggoods_cbunit", index);
}
