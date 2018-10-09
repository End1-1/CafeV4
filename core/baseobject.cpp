#include "baseobject.h"
#include "definitions.h"

BaseObject::BaseObject()
{
}

BaseObject::BaseObject(BaseObject const &obj)
{
    this->m_values = obj.m_values;
}

BaseObject::~BaseObject()
{
}

void BaseObject::fromXml(const QXmlAttributes &attr)
{
    for (int i = 0, count = attr.count(); i < count; i++)
        m_values.insert(attr.qName(i), attr.value(i));
}

QString BaseObject::toString(const QString &attrName) const
{
    return m_values[attrName];
}

int BaseObject::toInt(const QString &attrName) const
{
    return m_values[attrName].toInt();
}

double BaseObject::toDouble(const QString &attrName) const
{
    return m_values[attrName].toDouble();
}

QDateTime BaseObject::toDateTime(const QString &attrName) const
{
    return QDateTime::fromString(m_values[attrName], DATETIME_FORMAT);
}

QTime BaseObject::toTime(const QString &attrName) const
{
    return QTime::fromString(m_values[attrName], TIME_FORMAT);
}

BaseObject &BaseObject::operator =(const BaseObject &obj)
{
    this->m_values = obj.m_values;
    return *this;
}

bool BaseObject::compare(const QMap<QString, QString> &filter)
{
    QList<bool> result;
    for (QMap<QString, QString>::const_iterator it = filter.begin(); it != filter.end(); it++) {
        bool found = false;
        for (;;);
    }
    return true;
}
