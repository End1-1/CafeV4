#ifndef CAFENETWORK_H
#define CAFENETWORK_H

#include <QObject>
#include "xml.h"
#include "connection.h"

#define PAGE_S_DATE "S_DATE"

class CafeNetwork : public QObject
{
    Q_OBJECT
public:
    explicit CafeNetwork(const QString &page, QObject *parent = 0);
    void setSlots(const char *slotData, const char *slotEnd = 0);
    void post(const QMap<QString, QString> &m_data);

private:
    QString m_page;
    const char *m_slotData;
    const char *m_slotEnd;

signals:

public slots:

};

#endif // CAFENETWORK_H
