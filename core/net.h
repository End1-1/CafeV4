#ifndef NET_H
#define NET_H

#include <QObject>

#define BOUNDARY QString("123456789000A")

#define CODE_SUCCESS 0
#define CODE_NET_ERROR 1
#define CODE_XML_PARSE_ERROR 2

class QNetworkAccessManager;

class BaseNet : public QObject
{
    Q_OBJECT

private:
    QString m_serverUrl;
    QString m_data;
    QByteArray &m_replyData;
    QNetworkAccessManager *m_netMan;

protected:
    int m_errorCode;
    QString m_errorMessage;



protected slots:
    void slotNetReply();

public:
    explicit BaseNet(const QString serverUrl, QByteArray &replyData);
    ~BaseNet();
    void appendData(const QString &key, const QString &value);
    void post();
    
signals:
    void finish(int code);
    
public slots:
    
};

#endif // NET_H
