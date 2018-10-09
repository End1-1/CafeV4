#include "xml.h"
#include "net.h"
#include <QMessageBox>

BaseXml::BaseXml()
{
}

void BaseXml::parse()
{
    QXmlInputSource xmlInput;
    xmlInput.setData(m_xmlSource);
    QXmlSimpleReader xmlReader;
    xmlReader.setContentHandler(this);
    xmlReader.setErrorHandler(this);
    if (!xmlReader.parse(xmlInput))
        QMessageBox::critical(0, tr("Xml parse error"), xmlReader.errorHandler()->errorString());
}

bool BaseXml::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts)
{
    Q_UNUSED(namespaceURI)
    Q_UNUSED(localName)
    emit startElement(qName, atts);
    return true;
}

bool BaseXml::endDocument()
{
    emit endXmlDocument();
    return true;
}


BaseNetXml::BaseNetXml(const QString &url)
{
    m_url = url;
}

void BaseNetXml::post(const QMap<QString, QString> &request, QObject *slotReceiver, const char *slotStartElement, const char *slotEndDocument)
{
    if (slotStartElement)
        connect(this, SIGNAL(startElement(QString,QXmlAttributes)), slotReceiver, slotStartElement);
    if (slotEndDocument)
        connect(this, SIGNAL(endXmlDocument()), slotReceiver, slotEndDocument);
    BaseNet *net = new BaseNet(m_url, m_xmlSource);
    connect(net, SIGNAL(finish(int)), this, SLOT(requestFinish(int)));
    for (QMap<QString, QString>::const_iterator it = request.begin(); it != request.end(); it++)
        net->appendData(it.key(), it.value());
    net->post();
}

void BaseNetXml::requestFinish(int code)
{
    sender()->deleteLater();
    if (!code)
        parse();
    deleteLater();
}
