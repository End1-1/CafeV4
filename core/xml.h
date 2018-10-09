#ifndef XML_H
#define XML_H

#include <QXmlDefaultHandler>

class BaseXml : public QObject, QXmlDefaultHandler
{
    Q_OBJECT
public:
    explicit BaseXml();
    void parse();

signals:
    void startElement(const QString &qName, const QXmlAttributes &atts);
    void endXmlDocument();

public slots:

protected:
    QByteArray m_xmlSource;
    virtual bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
    virtual bool endDocument();

};

class BaseNetXml : public BaseXml
{
    Q_OBJECT
public:
    explicit BaseNetXml(const QString &url);
    void post(const QMap<QString, QString> &request, QObject *slotReceiver, const char *slotStartElement = 0, const char *slotEndDocument = 0);

private:
    QString m_url;

private slots:
    void requestFinish(int code);
};

#endif // XML_H
