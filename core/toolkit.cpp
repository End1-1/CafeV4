#include "toolkit.h"

#ifdef WAITER_EXE
#include "../core_gui/dlgmessagebox.h"
#else
#include <QMessageBox>
#endif

Toolkit::Toolkit(QObject *parent) :
    QObject(parent)
{
}

int Toolkit::newRow(QTableWidget *t)
{
    int row = t->rowCount();
    t->setRowCount(row + 1);

    for(int i = 0; i < t->columnCount(); i++)
        t->setItem(row, i, new QTableWidgetItem());

    return row;
}

int Toolkit::currentRow(QTableWidget *table, int& row)
{
    QModelIndexList lstRows = table->selectionModel()->selectedIndexes();

    if(lstRows.count() == 0) {
        QMessageBox::critical(table, tr("Error"), (tr("Nothing is selected")));
        row = -1;
    } else
        row = lstRows.at(0).row();

    return row;
}

void Toolkit::updateRow(QTableWidget *table, const int& row)
{
    for(int i = 0, count = table->columnCount(); i < count; i++)
        table->update(table->model()->index(row, i));
}

bool Toolkit::rowUp(QTableWidget *table)
{
    QModelIndexList lst = table->selectionModel()->selectedIndexes();

    if(!lst.count()) {
        table->setCurrentCell(0, 0);
        return false;
    }

    QModelIndex &modelIndex = lst[0];

    if(!modelIndex.row())
        return false;

    modelIndex = table->model()->index(modelIndex.row() - 1, modelIndex.column());
    table->selectionModel()->clear();
    table->selectionModel()->setCurrentIndex(modelIndex, QItemSelectionModel::Select);
    return true;
}

bool Toolkit::rowDown(QTableWidget *table)
{
    QModelIndexList lst = table->selectionModel()->selectedIndexes();

    if(!lst.count()) {
        table->setCurrentCell(0, 0);
        return false;
    }

    QModelIndex &modelIndex = lst[0];

    if(modelIndex.row() == table->rowCount() - 1)
        return false;

    modelIndex = table->model()->index(modelIndex.row() + 1, modelIndex.column());
    table->selectionModel()->clear();
    table->selectionModel()->setCurrentIndex(modelIndex, QItemSelectionModel::Select);
    return true;
}

void Toolkit::columnLeft(QTableWidget *table)
{
    QModelIndexList lst = table->selectionModel()->selectedIndexes();

    if(!lst.count())
        return;

    QModelIndex &modelIndex = lst[0];

    if(!modelIndex.column())
        return;

    table->setFocus();
    modelIndex = table->model()->index(modelIndex.row(), modelIndex.column() - 1);
    table->selectionModel()->clear();
    table->selectionModel()->setCurrentIndex(modelIndex, QItemSelectionModel::Select);
}

void Toolkit::columnRight(QTableWidget *table)
{
    QModelIndexList lst = table->selectionModel()->selectedIndexes();

    if(!lst.count())
        return;

    QModelIndex &modelIndex = lst[0];

    if(modelIndex.column() == table->columnCount() - 1)
        return;

    table->setFocus();
    modelIndex = table->model()->index(modelIndex.row(), modelIndex.column() + 1);
    table->selectionModel()->clear();
    table->selectionModel()->setCurrentIndex(modelIndex, QItemSelectionModel::Select);
}

bool Toolkit::comboStringData(QComboBox *comboBox, QString &result)
{
    result = "0";

    if(comboBox->currentIndex() > -1)
        result = comboBox->itemData(comboBox->currentIndex()).toString();

    return result != "0";
}

void Toolkit::comboSetIndexByData(QComboBox *comboBox, const QString &data)
{
    comboBox->setCurrentIndex(comboBox->findData(data));
}

int Toolkit::comboItemData(QComboBox *comboBox)
{
    if(comboBox->currentIndex() < 0)
        return 0;
    else
        return comboBox->itemData(comboBox->currentIndex()).toInt();
}

void Toolkit::setFirstAndLastDateEdit(QDateEdit *date1, QDateEdit *date2)
{
    date1->setDate(QDate::currentDate().addDays(-1 * (QDate::currentDate().day() - 1)));
    date2->setDate(QDate::currentDate());
}

QString Toolkit::formatDouble(const double& value, const int& f)
{
    return QString::number(value, 'f', f).remove(QRegularExpression("\\.0+$")).remove(QRegularExpression("\\.$"));
}
