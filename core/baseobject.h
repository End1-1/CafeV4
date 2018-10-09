#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <QMap>
#include <QXmlAttributes>
#include <QDateTime>
#include <QTime>

class BaseObject
{
private:
    QMap<QString, QString> m_values;

public:
    BaseObject();
    BaseObject(BaseObject const &obj);
    ~BaseObject();
    void fromXml(const QXmlAttributes &attr);
    QString toString(const QString &attrName) const;
    int toInt(const QString &attrName) const;
    double toDouble(const QString &attrName) const;
    QDateTime toDateTime(const QString &attrName) const;
    QTime toTime(const QString &attrName) const;
    BaseObject &operator=(const BaseObject &obj);
    bool compare(const QMap<QString, QString> &filter);
};

#endif // BASEOBJECT_H
