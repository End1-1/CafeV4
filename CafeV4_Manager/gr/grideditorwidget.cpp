#include "grideditorwidget.h"
#include "sqlthread.h"
#include <QGroupBox>
#include <QMessageBox>
#include <QLineEdit>

GridEditorWidget::GridEditorWidget(QWidget *parent) :
    QWidget(parent)
{
    mCodeLineEdit = nullptr;
}

GridEditorWidget::~GridEditorWidget()
{

}

void GridEditorWidget::clearWidgets()
{
    for (QWidget *w: mDbWidgets) {
        clearWidget(w);
    }
}

bool GridEditorWidget::isOk()
{
    return true;
}

void GridEditorWidget::save()
{
    if (!isOk()) {
        return;
    }
    int id = mCodeLineEdit->text().toInt();
    SqlThread *st = new SqlThread(_gr_host_, _gr_path_, _gr_user_, _gr_pass_, "lc_ctype=utf8");
    connect(st, SIGNAL(finished()), this, SLOT(saveFinished()));
    connect(st, SIGNAL(error(QString)), this, SLOT(saveError(QString)));
    for (QWidget *w: mDbWidgets) {
        if (w->property("id").toBool()) {
            continue;
        }
        if (!w->property("field").toString().isEmpty()) {
            if (w->property("ro").toBool()) {
                continue;
            }
            st->bind(":" + w->property("field").toString(), getDbWidgetValue(w));
        }
    }
    if (id == 0) {
        st->insert(mTableName);
    } else {
        st->update(mTableName, id);
    }
}

void GridEditorWidget::prepare()
{
    mDbWidgets.clear();
    setTableName();
    enumDbWidgets(this);
}

void GridEditorWidget::fillFields()
{
    SqlThread *st = static_cast<SqlThread*>(sender());
    if (!st->nextRow()) {
        return;
    }
    QStringList cols = st->mColumns.keys();
    for (const QString &c: cols) {
        if (mDbWidgets.contains(c)) {
            setDbWidgetValue(mDbWidgets[c], st->getValue(c));
        }
    }
}

void GridEditorWidget::sqlFinished()
{
    fillFields();
    sender()->deleteLater();
}

void GridEditorWidget::saveFinished()
{
    SqlThread *st = static_cast<SqlThread*>(sender());
    if (mCodeLineEdit->text().toInt() == 0) {
        if (st->nextRow()) {
            mCodeLineEdit->setText(st->getString("id"));
        }
    }
    st->deleteLater();
}

void GridEditorWidget::enumDbWidgets(QWidget *parent)
{
    const QObjectList &ol = parent->children();
    for (QObject *o: ol) {
        QLineEdit *le = dynamic_cast<QLineEdit*>(o);
        if (le) {
            if (le->property("id").toBool()) {
                mCodeLineEdit = le;
            }
            if (!le->property("field").toString().isEmpty()) {
                mDbWidgets.insert(le->property("field").toString(), le);
            }
            continue;
        }
        QGroupBox *gb = dynamic_cast<QGroupBox*>(o);
        if (gb) {
            enumDbWidgets(gb);
            continue;
        }
        QWidget *w = dynamic_cast<QWidget*>(o);
        if (w) {
            enumDbWidgets(w);
            continue;
        }
    }
}

void GridEditorWidget::setDbWidgetValue(QWidget *w, const QVariant &v)
{
    QLineEdit *le = dynamic_cast<QLineEdit*>(w);
    if (le) {
        le->setText(v.toString());
        return;
    }
}

QVariant GridEditorWidget::getDbWidgetValue(QWidget *w) const
{
    QLineEdit *le = dynamic_cast<QLineEdit*>(w);
    if (le) {
        switch (le->property("type").toInt()) {
        case 0:
            return le->text();
        case 1:
            return le->text().toInt();
        default:
            return le->text();
        }
    }
    return QVariant();
}

void GridEditorWidget::clearWidget(QWidget *w)
{
    QLineEdit *le = dynamic_cast<QLineEdit*>(w);
    if (le) {
        le->clear();
        return;
    }
}

void GridEditorWidget::saveError(const QString &err)
{
    sender()->deleteLater();
    QMessageBox::critical(this, tr("Sql error"), err);
}
