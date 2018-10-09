#ifndef CTABLEMODEL_H
#define CTABLEMODEL_H

#include <QAbstractTableModel>

#define float_str(value, f) QString::number(value, 'f', f).remove(QRegExp("\\.0+$")).remove(QRegExp("\\.$"))

class CDatabase;

class CTableModel : public QAbstractTableModel
{
    Q_OBJECT
private:
    QList<int> fProxyRows;
    int fColumnCount;
    int fLastSortIndex;
    int fFilteredColumn;
    Qt::SortOrder fLastSortOrder;
    CDatabase *fDatabase;
    QMap<QString, QString> fHeaderCaptions;
public:
    CTableModel();
    virtual int rowCount(const QModelIndex &parent) const;
    int rowCount() const;
    virtual int columnCount(const QModelIndex &parent) const;
    int columnCount() const;
    void setColumnNameMap(const QString &field, const QString &name);
    int columnIndexByName(const QString &field) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant data(const QModelIndex &index, int role) const;
    virtual QVariant data(int row, int col, int role = Qt::EditRole) const;
    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    void search(const QString &text);
    void search(const QStringList &values);
    void search(int column, QStringList &values);
    void getUniqueValuesForColumn(int column, QStringList &values);
public slots:
    void dataLoaded(CDatabase *db);
signals:
    void endOfLoadData();
};

#endif // CTABLEMODEL_H
