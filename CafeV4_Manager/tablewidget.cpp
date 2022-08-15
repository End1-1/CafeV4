#include "tablewidget.h"
#include "grdb.h"
#include "xlsxall.h"
#include <QLineEdit.h>
#include <QComboBox.h>
#include <QCheckBox.h>
#include <QHeaderView>
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>

TableWidget::TableWidget(QWidget *parent) :
    QTableWidget(parent)
{
    setEditTriggers(NoEditTriggers);
}

TableWidgetItem *TableWidget::item(int row, int column) const
{
    return static_cast<TableWidgetItem*>(QTableWidget::item(row, column));
}

void TableWidget::setColumnWidths(int count, ...)
{
    setColumnCount(count);
    va_list vl;
    va_start(vl, count);
    for (int i = 0; i < count; i++) {
        setColumnWidth(i, va_arg(vl, int));
    }
    va_end(vl);
}

void TableWidget::fitColumnsToWidth(int dec)
{
    int colWidths = 0;
    int hiddenColumns = 0;
    for (int i = 0; i < columnCount(); i++) {
        if (columnWidth(i) == 0) {
            hiddenColumns++;
        }
        colWidths += columnWidth(i);
    }
    int freeSpace = width() - colWidths - dec;
    int delta = 0;
    if (columnCount() - hiddenColumns > 0) {
        delta = freeSpace / (columnCount() - hiddenColumns);
    }
    for (int i = 0; i < columnCount(); i++) {
        if (columnWidth(i) == 0) {
            continue;
        }
        setColumnWidth(i, columnWidth(i) + delta);
    }
}

void TableWidget::fitRowToHeight(int dec)
{
    int rh = verticalHeader()->defaultSectionSize();
    int visibleRows = (height() - dec) / rh;
    int delta = (height() - dec) - (visibleRows * rh);
    verticalHeader()->setDefaultSectionSize(rh + (delta / visibleRows));
}

QLineEdit *TableWidget::createLineEdit(int row, int column)
{
    QLineEdit *l = new QLineEdit(this);
    l->setProperty("row", row);
    l->setProperty("column", column);
    l->setFrame(false);
    connect(l, SIGNAL(textChanged(QString)), this, SLOT(lineEditTextChanged(QString)));
    setCellWidget(row, column, l);
    return l;
}

QLineEdit *TableWidget::lineEdit(int row, int column)
{
    return static_cast<QLineEdit*>(cellWidget(row, column));
}

QComboBox *TableWidget::createComboBox(int row, int column)
{
    QComboBox *c = new QComboBox(this);
    c->setProperty("row", row);
    c->setProperty("column", column);
    c->setFrame(false);
    setCellWidget(row, column, c);
    connect(c, SIGNAL(currentTextChanged(QString)), this, SLOT(comboTextChanged(QString)));
    return c;
}

QComboBox *TableWidget::comboBox(int row, int column)
{
    return static_cast<QComboBox*>(cellWidget(row, column));
}

QCheckBox *TableWidget::createCheckbox(int row, int column)
{
    QCheckBox *c = new QCheckBox(this);
    c->setProperty("row", row);
    c->setProperty("column", column);
    setCellWidget(row, column, c);
    connect(c, SIGNAL(clicked(bool)), this, SLOT(checkBoxChecked(bool)));
    return c;
}

QCheckBox *TableWidget::checkBox(int row, int column)
{
    return static_cast<QCheckBox*>(cellWidget(row, column));
}

bool TableWidget::findWidget(QWidget *w, int &row, int &column)
{
    for (int r = 0; r < rowCount(); r++) {
        for (int c = 0; c < columnCount(); c++) {
            if (cellWidget(r, c) == w) {
                row = r;
                column = c;
                return true;
            }
        }
    }
    return false;
}

QVariant TableWidget::getData(int row, int column)
{
    TableWidgetItem *i = item(row, column);
    return i->data(Qt::EditRole);
}

void TableWidget::setData(int row, int column, const QVariant &value)
{
    TableWidgetItem *i = item(row, column);
    if (!i) {
        i = new TableWidgetItem();
        QTableWidget::setItem(row, column, i);
        if (fColumnsDecimals.contains(column)) {
            i->fDecimals = fColumnsDecimals[column];
        }
    }
    i->setData(Qt::EditRole, value);
}

int TableWidget::getInteger(int row, int column)
{
    return getData(row, column).toInt();
}

void TableWidget::setInteger(int row, int column, int value)
{
    setData(row, column, value);
}

QString TableWidget::getString(int row, int column)
{
    TableWidgetItem *i = item(row, column);
    if (!i) {
        return "NO ITEM!";
    }
    return i->text();
}

void TableWidget::setString(int row, int column, const QString &str)
{
    setData(row, column, str);
}

double TableWidget::getDouble(int row, int column)
{
    return getData(row, column).toDouble();
}

void TableWidget::setDouble(int row, int column, double value)
{
    setData(row, column, value);
}

int TableWidget::addEmptyRow()
{
    int row = rowCount();
    setRowCount(row + 1);
    for (int i = 0; i < columnCount(); i++) {
        setItem(row, i, new TableWidgetItem());
    }
    return row;
}

void TableWidget::exportToExcel()
{
    if (columnCount() == 0 || rowCount() == 0) {
        QMessageBox::information(this, tr("Information"), tr("Empty report"));
        return;
    }
    XlsxDocument d;
    XlsxSheet *s = d.workbook()->addSheet("Sheet1");
    /* HEADER */
    QColor color = QColor::fromRgb(200, 200, 250);
    QFont headerFont(qApp->font());
    headerFont.setBold(true);
    d.style()->addFont("header", headerFont);
    d.style()->addBackgrounFill("header", color);
    for (int i = 0; i < columnCount(); i++) {
        s->addCell(1, i + 1, horizontalHeaderItem(i)->data(Qt::DisplayRole).toString(), d.style()->styleNum("header"));
        s->setColumnWidth(i + 1, columnWidth(i) / 7);
    }
    //e.setHorizontalAlignment(e.address(0, 0), e.address(0, colCount - 1), Excel::hCenter);
    /* BODY */
    QFont bodyFont(qApp->font());
    d.style()->addFont("body", bodyFont);
    for (int j = 0; j < rowCount(); j++) {
        for (int i = 0; i < columnCount(); i++) {
            s->addCell(j + 2, i + 1,  item(j, i)->data(Qt::EditRole), d.style()->styleNum("body"));
        }
    }
    QString err;
    if (!d.save(err, true)) {
        if (!err.isEmpty()) {
            QMessageBox::critical(this, tr("Error"), err);
        }
    }
}

void TableWidget::search(const QString &txt)
{
    for (int i = 0; i < rowCount(); i++) {
        bool hidden = true;
        for (int j = 0; j < columnCount(); j++) {
            if (getString(i, j).contains(txt, Qt::CaseInsensitive)) {
                hidden = false;
            }
        }
        setRowHidden(i, hidden);
    }
}

double TableWidget::sumOfColumn(int column)
{
    double total = 0;
    for (int i = 0; i < rowCount(); i++) {
        total += getDouble(i, column);
    }
    return total;
}

void TableWidget::setColumnDecimals(int column, int decimals)
{
    fColumnsDecimals[column] = decimals;
}

void TableWidget::lineEditTextChanged(const QString arg1)
{
    QLineEdit *l = static_cast<QLineEdit*>(sender());
    setString(l->property("row").toInt(), l->property("column").toInt(), arg1);
}

void TableWidget::comboTextChanged(const QString &text)
{
    QComboBox *c = static_cast<QComboBox*>(sender());
    setString(c->property("row").toInt(), c->property("column").toInt(), text);
}

void TableWidget::checkBoxChecked(bool v)
{
    QCheckBox *c = static_cast<QCheckBox*>(sender());
    setString(c->property("row").toInt(), c->property("column").toInt(), (v ? "1" : "0"));
}

TableWidgetItem::TableWidgetItem(int type) :
    QTableWidgetItem (type)
{
    fDecimals = 2;
}

TableWidgetItem::TableWidgetItem(const QString &text, int type) :
    QTableWidgetItem (text, type)
{

}

QVariant TableWidgetItem::data(int role) const
{
    QVariant v = QTableWidgetItem::data(role);
    if (role == Qt::DisplayRole) {
       switch (v.type()) {
       case QVariant::Int:
           return v.toString();
       case QVariant::Date:
           return v.toDate().toString(FORMAT_DATE_TO_STR);
       case QVariant::DateTime:
           return v.toDateTime().toString(FORMAT_DATETIME_TO_STR);
       case QVariant::Time:
           return v.toTime().toString(FORMAT_TIME_TO_STR);
       case QVariant::Double:
           return double_str(v.toDouble(), fDecimals);
       default:
           return v.toString();
       }
    }
    return v;
}
