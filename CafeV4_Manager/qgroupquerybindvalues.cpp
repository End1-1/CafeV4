#include "qgroupquerybindvalues.h"
#include "ui_qgroupquerybindvalues.h"
#include <QLabel>
#include <QDateEdit>
#include <QLineEdit>

QGroupQueryBindValues::QGroupQueryBindValues(QList<BindValue> &values, QWidget *parent) :
    QDialog(parent),
    m_bindDst(values),
    m_bindTemp(values),
    ui(new Ui::QGroupQueryBindValues)
{
    ui->setupUi(this);
    for (QList<BindValue>::iterator it = m_bindTemp.begin(); it != m_bindTemp.end(); it++)
        addRow(*it);
}

QGroupQueryBindValues::~QGroupQueryBindValues()
{
    delete ui;
}

void QGroupQueryBindValues::txtChange(const QString &v)
{
    QWidget *w = static_cast<QWidget*>(sender());
    if (!w)
        return;
    if (!m_bindMap.contains(w))
        return;
    BindValue *bv = m_bindMap[w];
    switch (bv->value.type()) {
    case QVariant::Int:
        bv->value = v.toInt();
        break;
    case QVariant::Double:
        bv->value = v.toDouble();
        break;
    default:
        bv->value = v;
        break;
    }
}

void QGroupQueryBindValues::dateChanged(const QDate &d)
{
    QWidget *w = static_cast<QWidget*>(sender());
    if (!w)
        return;
    if (!m_bindMap.contains(w))
        return;
    BindValue *bv = m_bindMap[w];
    bv->value = d;
}

void QGroupQueryBindValues::addRow(BindValue &bv)
{
    QLabel *l = new QLabel(bv.valueTitle);
    QWidget *w;
    switch (bv.value.type()) {
    case QVariant::Date:
        w = new QDateEdit();
        static_cast<QDateEdit*>(w)->setDate(bv.value.toDate());
        static_cast<QDateEdit*>(w)->setCalendarPopup(true);
        connect(static_cast<QDateEdit*>(w), SIGNAL(dateChanged(QDate)), this,  SLOT(dateChanged(QDate)));
        break;
    case QVariant::Int:
        w = new QLineEdit();
        static_cast<QLineEdit*>(w)->setText(bv.value.toString());
        static_cast<QLineEdit*>(w)->setValidator(new QIntValidator());
        connect(static_cast<QLineEdit*>(w), SIGNAL(textChanged(QString)), this, SLOT(txtChange(QString)));
        break;
    case QVariant::Double:
        w = new QLineEdit();
        static_cast<QLineEdit*>(w)->setText(dts(bv.value.toDouble()));
        static_cast<QLineEdit*>(w)->setValidator(new QDoubleValidator(-1000000000, 1000000000, 3));
        connect(static_cast<QLineEdit*>(w), SIGNAL(textChanged(QString)), this, SLOT(txtChange(QString)));
        break;
    case QVariant::String:
        w = new QLineEdit();
        static_cast<QLineEdit*>(w)->setText(bv.value.toString());
        connect(static_cast<QLineEdit*>(w), SIGNAL(textChanged(QString)), this, SLOT(txtChange(QString)));
        break;
    default:
        return;
    }
    int row = ui->gl->rowCount();
    ui->gl->addWidget(l, row, 0);
    ui->gl->addWidget(w, row, 1);
    m_bindMap[w] = &bv;
}

void QGroupQueryBindValues::on_btnCancel_clicked()
{
    reject();
}

void QGroupQueryBindValues::on_btnOK_clicked()
{
    m_bindDst.clear();
    for (QList<BindValue>::iterator it = m_bindTemp.begin(); it != m_bindTemp.end(); it++)
        m_bindDst.append(*it);
    accept();
}
