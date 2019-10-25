#ifndef UNIVERSALTABLEMODEL_H
#define UNIVERSALTABLEMODEL_H

#include <QAbstractTableModel>

class UniversalTableModel : public QAbstractTableModel
{
public:
    UniversalTableModel(QObject *parent = nullptr);
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual int columnCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    QList<QList<QVariant> > fRawData;
};

#endif // UNIVERSALTABLEMODEL_H
