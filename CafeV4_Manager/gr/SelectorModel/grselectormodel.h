#ifndef GRSELECTORMODEL_H
#define GRSELECTORMODEL_H

#include <QObject>

class GrSelectorModel : public QObject
{
    Q_OBJECT
public:
    explicit GrSelectorModel(QObject *parent = nullptr);
    bool mResult;
    QString mSql;
    QStringList mFields;
    bool mMultiselection;

};

#endif // GRSELECTORMODEL_H
