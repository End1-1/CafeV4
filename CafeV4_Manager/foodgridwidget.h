#ifndef FOODGRIDWIDGET_H
#define FOODGRIDWIDGET_H

#include <QWidget>
#include "core.h"

namespace Ui {
class FoodGridWidget;
}

class FoodGridWidget : public QWidget
{
    Q_OBJECT
    
public:
    DockIdName *m_dockIdName;
    explicit FoodGridWidget(QWidget *parent = 0);
    ~FoodGridWidget();
    void setTableTitle(const QStringList &labels, const QList<int> &widths);
    void setTableColumnValidator(const int &column, int validator);
    void setTableModel(QMap<QString, QStringList> *model);
    void setDockIdNameColumns(const int &id, const int &name);
    void setDockIdNameLists(QStringList *ids, QStringList *names);
    void clearRows();
    int newTableRow();
    void updateRow(const int &row);
    int tableCurrentRow() const;
    QTableWidget *table();
    void setRowColor(const int &col, const QString &match, const QColor &color);
    void removeRows(const int &col, const QString &match);
    void removeRow(const int &row);
    void setCurrentRow(const int &row);
    int rowCount();
    void setData(const int &row, const int &col, const QVariant &data, int role);
    QPushButton *btnNew();

protected:
    bool eventFilter(QObject *object, QEvent *event);
    
private slots:
    void tableCellWidgetTextChanged(const QString &str);
    void filteredRow(int row);
    void on_pushButton_clicked();
    void on_tableGrid_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_pushButton_2_clicked();
    void on_tableGrid_clicked(const QModelIndex &index);

private:
    Ui::FoodGridWidget *ui;
    QMap<QString, QStringList> *m_model;
    QStringList *m_idsList;
    QStringList *m_namesList;
    QLineEdit *m_tableLineedit;
    QMap<int, int> m_columnValidator; // 0 - no validator, 1 - int, 2 - double, 3 - no edit needed
    int m_columnForId; //тут откроктся список наименований с ключем id
    int m_columnForName; //тут откроется список наименования с ключем name
    int m_columnForQty;
    void makeLineEdit(const int &row, const int &column);
    void removeLineEdit();

signals:
    void cellTextChanged(const QString &column, const int &row, const QString &str);
    void selectedItem(int row);
    void rowRemoved(int row);
};

#endif // FOODGRIDWIDGET_H
