#include "ctablemodel.h"
#include "cdatabase.h"
#include <QIcon>
#include <QSet>

#ifdef QT_DEBUG
#include <QDebug>
#endif

CTableModel::CTableModel()
{
    fLastSortIndex = 0;
    fFilteredColumn = -1;
}

int CTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return rowCount();
}

int CTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columnCount();
}

int CTableModel::columnCount() const
{
    return fColumnCount;
}

void CTableModel::setColumnNameMap(const QString &field, const QString &name)
{
    fHeaderCaptions[field.toUpper()] = name;
}

int CTableModel::columnIndexByName(const QString &field) const
{
    return fDatabase->columnIndexByName(field);
}

QVariant CTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        if(orientation == Qt::Vertical) {
            return section + 1;
        } else {
            QString columnName = fDatabase->columnNameByIndex(section);

            if(fHeaderCaptions.contains(columnName)) {
                return fHeaderCaptions[columnName];
            } else {
                return "UNKNOWN";
            }
        }

        break;

    case Qt::DecorationRole:
        if(orientation == Qt::Horizontal) {
            if(section == fFilteredColumn) {
                return QVariant::fromValue(QIcon(":/img/filter.png"));
            }
        }
    }

    return QVariant();
}

QVariant CTableModel::data(const QModelIndex &index, int role) const
{
    switch(role) {
    case Qt::DisplayRole: {
        return fDatabase->fRawData[fProxyRows.at(index.row())].at(index.column());
    }

    case Qt::EditRole:
        return fDatabase->fRawData[fProxyRows.at(index.row())].at(index.column());
    }

    return QVariant();
}

QVariant CTableModel::data(int row, int col, int role) const
{
    return data(index(row, col), role);
}

void CTableModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(order)
    QMap<QVariant, int> m;

    for(int i = 0, count = fProxyRows.count(); i < count; i++) {
        m.insertMulti(fDatabase->fRawData.at(fProxyRows.at(i)).at(column), fProxyRows.at(i));
    }

    beginResetModel();
    fProxyRows = m.values();

    if(fLastSortIndex == column) {
        fLastSortOrder = fLastSortOrder == Qt::AscendingOrder ? Qt::DescendingOrder : Qt::AscendingOrder;
    } else {
        fLastSortOrder = Qt::AscendingOrder;
    }

    fLastSortIndex = column;

    if(fLastSortOrder == Qt::DescendingOrder) {
        std::reverse(fProxyRows.begin(), fProxyRows.end());
    }

    endResetModel();
}

void CTableModel::search(const QString &text)
{
    fFilteredColumn = -1;
    QList<int> tempRows;

    for(int i = 0; i < fDatabase->fRawData.count(); i++) {
        for(int j = 0; j < fDatabase->fRawData[i].count(); j++) {
            if(fDatabase->fRawData[i][j].toString().contains(text, Qt::CaseInsensitive)) {
                tempRows.append(i);
                break;
            }
        }
    }

    beginResetModel();
    fProxyRows = tempRows;
    endResetModel();
}

void CTableModel::search(const QStringList &values)
{
    QList<int> tempRows;

    for(int i = 0; i < fDatabase->fRawData.count(); i++) {
        for(int j = 0; j < fDatabase->fRawData[i].count(); j++) {
            foreach(QString text, values) {
                if(fDatabase->fRawData[i][j].toString().contains(text, Qt::CaseInsensitive)) {
                    tempRows.append(i);
                    goto mark;
                }
            }
        }

mark:
        continue;
    }

    beginResetModel();
    fProxyRows = tempRows;
    endResetModel();
}

void CTableModel::search(int column, QStringList &values)
{
    QList<int> tempRows;

    if(values.empty()) {
        fFilteredColumn = -1;

        for(int i = 0, count = fDatabase->fRawData.count(); i < count; i++) {
            fProxyRows << i;
        }
    } else {
        fFilteredColumn = column;

        for(int i = 0; i < fDatabase->fRawData.count(); i++) {
            foreach(QString text, values) {
                if(fDatabase->fRawData[i][column].toString().compare(text, Qt::CaseInsensitive) == 0) {
                    tempRows.append(i);
                    goto mark;
                }
            }

mark:
            continue;
        }
    }

    beginResetModel();
    fProxyRows = tempRows;
    endResetModel();
}

void CTableModel::getUniqueValuesForColumn(int column, QStringList &values)
{
    QSet<QString> list;

    for(int i = 0, count = fDatabase->fRawData.count(); i < count; i++) {
        list << fDatabase->fRawData.at(i).at(column).toString();
    }

    std::sort(values.begin(), values.end());
}

int CTableModel::rowCount() const
{
    return fProxyRows.count();
}

void CTableModel::dataLoaded(CDatabase *db)
{
#ifdef QT_DEBUG
    CDatabase::log("Starting model data fetch");
#endif
    fDatabase = db;
    beginResetModel();
    fProxyRows.clear();
    fColumnCount = db->fFieldsMapName.count();

    for(int i = 0, count = db->fRawData.count(); i < count; i++) {
        fProxyRows << i;
    }

    endResetModel();
    emit endOfLoadData();
#ifdef QT_DEBUG
    CDatabase::log("End data fetch");
#endif
}
