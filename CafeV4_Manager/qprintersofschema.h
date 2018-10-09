#ifndef QPRINTERSOFSCHEMA_H
#define QPRINTERSOFSCHEMA_H

#include "qgrid.h"

class QPrintersOfSchema : public QGrid
{
    Q_OBJECT
public:
    explicit QPrintersOfSchema(int id, const QString &name, QWidget *parent = 0);
    virtual void actionBeforeSelect();
    virtual void actionNew();
    virtual void actionSave();
    virtual void actionDelete();

private:
    int m_id;
    QString m_lastWorkstation;
};

#endif // QPRINTERSOFSCHEMA_H
