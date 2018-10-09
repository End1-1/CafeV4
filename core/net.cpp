#include "net.h"
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QMessageBox>

BaseNet::BaseNet(const QString serverUrl, QByteArray &replyData)  :
    m_replyData(replyData)
{
    m_netMan = new QNetworkAccessManager();
    m_serverUrl = serverUrl;
}

BaseNet::~BaseNet()
{
    delete m_netMan;
}

void BaseNet::post()
{
    m_errorCode = 0;
    m_errorMessage = "";
    m_data.append("--");

    QNetworkRequest netRequest(QUrl(m_serverUrl.toLatin1()));
    netRequest.setRawHeader("Content-type", QString("multipart/form-data; charset=utf-8; boundary=" + BOUNDARY).toLatin1());
    netRequest.setRawHeader("Chache-Control", "no-cache");
    netRequest.setRawHeader("Accept", "*/*");
    netRequest.setRawHeader("Content-Length", QString::number(m_data.toUtf8().size()).toLatin1());

    QNetworkReply *reply = m_netMan->post(netRequest, m_data.toUtf8());
    connect(reply, SIGNAL(finished()), this, SLOT(slotNetReply()));

    m_data.clear();
}

void BaseNet::appendData(const QString &key, const QString &value)
{
    m_data.append("--" + BOUNDARY + "\r\n");
    m_data.append(QString("Content-disposition: form-data; name=\"%1\"\r\n\r\n").arg(key));
    m_data.append(value + "\r\n");
}

void BaseNet::slotNetReply()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());
    if (reply->error() == QNetworkReply::NoError)
        m_replyData = reply->readAll();
    else
        QMessageBox::critical(0, tr("Network error"), reply->errorString());
    emit finish(reply->error());
    reply->deleteLater();
}

