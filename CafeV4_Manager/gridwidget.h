#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include "sqlthread.h"
#include <QWidget>
#include <QMenu>
#include <QAbstractTableModel>

namespace Ui {
class GridWidget;
}

class GridEditorWidget;

class GridTableModel : public QAbstractTableModel {

public:
    explicit GridTableModel(QList<QList<QVariant> > &data, QMap<QString, int> &indexOfField);
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int rowCount() const;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual int columnCount() const;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QVariant data(int row, int column, int role) const;
    virtual void sort(int column, Qt::SortOrder order) override;
    virtual void sort(int column);
    QString nameForColumnIndex(int index) const;
    void setTranslation(const QMap<QString, QString> &translation);
    void setFilter(int column, const QString &pattern);
    void reset();

private:
    QList<int> mProxyRows;
    QMap<int, QString> mFilters;
    QList<QList<QVariant> > &mData;
    QMap<QString, QString> mTranslation;
    QMap<QString, int> &mIndexOfField;
    QMap<int, QString> mFieldIndex;
    bool mSortAsc;
    int mLastSortedColumn;
    void filterData();
};

struct GridColumn {
public:
    QString mField;
    QString mTranslation;
    QString mTable;
    QString mLeftJoinTable;
    QString mLeftJoinRelation;
    bool mSelect;
};

class GridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridWidget(QWidget *parent = nullptr);
    ~GridWidget();
    virtual void setTitle(const QString &title);
    virtual void prepare();
    virtual void go();
    void hideTotalTable(bool hidden);
    void showMenu();
    void search();
    void escapeKey();

public slots:
    void queryFinished();
    void mainTableSectionResized(int logicalIndex, int oldSize, int newSize);
    void mainTableSectionClicked(int logicalIndex);
    void mainTableValueChanged(int value);
    virtual void newItem();
    virtual void editItem();

protected:
    QString mMainTable;
    GridEditorWidget *mEditorWidget;
    QMenu mMenu;
    QMap<QString, QString> mLeftJoinTables;
    QMap<QString, QString> mLeftJoinRelations;
    virtual void restoreColumnsWidths();
    virtual int getItemID();
    void addColumn(const QString &field, const QString &translation, bool select, const QString &leftjoin, const QString &leftJoinRelation);

private slots:
    void on_leQuickSearch_textChanged(const QString &arg1);
    void on_tblData_doubleClicked(const QModelIndex &index);

private:
    Ui::GridWidget *ui;
    GridTableModel *mModel;
    SqlThread mDb;
    QString mReportTitle;
    QList<GridColumn> mColumns;
};

#endif // GRIDWIDGET_H
