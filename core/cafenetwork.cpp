#include "cafenetwork.h"

CafeNetwork::CafeNetwork(const QString &page, QObject *parent) :
    QObject(parent)
{
    m_page = page;
    m_slotData = 0;
    m_slotEnd = 0;
}

void CafeNetwork::setSlots(const char *slotData, const char *slotEnd)
{
    m_slotData = slotData;
    m_slotEnd = slotEnd;
}

void CafeNetwork::post(const QMap<QString, QString> &m_data)
{
    BaseNetXml *b = new BaseNetXml("http://" + Connection::serverUrl() + "/engine/" + m_page + ".php");
    connect(b, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    b->post(m_data, parent(),  m_slotData, m_slotEnd);
}
