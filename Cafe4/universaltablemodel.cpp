#include "universaltablemodel.h"

UniversalTableModel::UniversalTableModel(QObject *parent) :
    QAbstractTableModel(parent)
{

}


int UniversalTableModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int UniversalTableModel::columnCount(const QModelIndex &parent) const
{
    return 0;
}

QVariant UniversalTableModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}
