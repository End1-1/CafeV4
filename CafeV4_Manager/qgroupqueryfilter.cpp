#include "qgroupqueryfilter.h"
#include "ui_qgroupqueryfilter.h"
#include <QDate>

QGroupQueryFilter::QGroupQueryFilter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QGroupQueryFilter)
{
    ui->setupUi(this);
    m_sqlDrv = new QSqlDrv("admin", "clientdb");
    if (!m_sqlDrv->openDB())
        return;
    if (!m_sqlDrv->prepare("select title, sqltext, params from group_query order by title"))
        return;
    if (!m_sqlDrv->execSQL())
        return;
    while (m_sqlDrv->next()) {
        QListWidgetItem *item = new QListWidgetItem(ui->lstQueries);
        item->setText(m_sqlDrv->val().toString());
        item->setData(Qt::UserRole, m_sqlDrv->val());
        item->setData(Qt::UserRole + 1, m_sqlDrv->val());
        ui->lstQueries->addItem(item);
    }
}

QGroupQueryFilter::~QGroupQueryFilter()
{
    delete ui;
}

void QGroupQueryFilter::on_btnCancel_clicked()
{
    reject();
}

void QGroupQueryFilter::on_btnOk_clicked()
{
    QList<QListWidgetItem*> items = ui->lstQueries->selectedItems();
    if (!items.count())
        return;
    QListWidgetItem *item = items[0];
    m_sqlText = item->data(Qt::UserRole).toString();
    m_queryTitle = item->text();
    QStringList bindValuesList = item->data(Qt::UserRole + 1).toString().split(";");
    QSettings s("Jazzve", "Cafe4\\GroupQuery\\" + m_queryTitle);
    for (QStringList::const_iterator it = bindValuesList.begin(); it != bindValuesList.end(); it++) {
        QStringList l = it->split("=", QString::SkipEmptyParts);
        BindValue bv;
        bv.valueTitle = l.at(1);
        bv.valueName = l.at(0);
        if (l.at(2) == "date") {
            bv.value = s.value(bv.valueName, QDate::currentDate()).toDate();
        } else if (l.at(2) == "int") {
            bv.value = s.value(bv.valueName, 0).toInt();
        } else if (l.at(2) == "string") {
            bv.value = s.value(bv.valueName, "").toString();
        }
        m_bindValues.append(bv);
    }
    accept();
}
