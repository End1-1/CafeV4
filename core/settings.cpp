#include "settings.h"
#include <QDate>
#include <QDateTime>
#include <QtPrintSupport/QPrinterInfo>
#include "definitions.h"
#include "connection.h"
#ifdef WIN32
#include <windows.h>
#endif

Settings::Settings(const QString &settingsName)
{
    m_versionNew = 0;
    m_versionCurrent = -1;
    m_settingsName = settingsName;
    connect(this, SIGNAL(endOfQuery(int,QString)), SLOT(endOfQuery(int,QString)));
}

void Settings::uploadPrinters(const QString &session)
{
    QList<QPrinterInfo> printers = QPrinterInfo::availablePrinters();
    BaseNetwork *b = new BaseNetwork();
    b->appendData("sysop", "WORKSTATIONPRINTERS");
    b->appendData("cafe", "1");
    b->appendData("session", session);
    b->appendData("workstation", Connection::hostName() + ":" + Connection::userName());
    for (QList<QPrinterInfo>::const_iterator p = printers.begin(); p != printers.end(); p++)
        b->appendData("printers[]", (*p).printerName());
    disconnect(b, SIGNAL(endOfQuery(int,QString)), this, SLOT(endOfUploadPrinters(int,QString)));
    connect(b, SIGNAL(endOfQuery(int,QString)), SLOT(endOfUploadPrinters(int,QString)));
    b->post();
}

void Settings::resetVersion()
{
    m_versionCurrent = -1;
}

void Settings::loadSettings(const QString &session, const QString module)
{
    m_settings.clear();
    appendData("sysop", "OPTIONS");
    appendData("name", m_settingsName);
    appendData("module", module);
    appendData("session", session);
    appendData("unlock", Connection::hostName() + ":" + Connection::userName());
    post();
}

QString Settings::toString(const QString &name)
{
    return m_settings[name].toString();
}

int Settings::toInt(const QString &name)
{
    return m_settings[name].toInt();
}

QDate Settings::toDate(const QString &name)
{
    return QDate::fromString(m_settings[name].toString(), DATE_FORMAT);
}

double Settings::toDouble(const QString &name)
{
    return m_settings[name].toDouble();
}

const QVariant Settings::toVariant(const QString &name)
{
    return m_settings[name];
}

const QString Settings::cashDate()
{
    if (QTime::currentTime() > QTime::fromString(toString(SETTINGS_CASH_DATE_CHANGE), TIME_FORMAT))
        return QDate::currentDate().toString(DATE_FORMAT);
    else
        return QDate::currentDate().addDays(-1).toString(DATE_FORMAT);
}

void Settings::setSystemDateTime(const QString &dateTime)
{
    //Datetime string format dd.MM.YYYY hh:mm:ss
#ifdef WIN32
    QDateTime dt = QDateTime::fromString(dateTime, DATETIME_FORMAT);
    SYSTEMTIME st;
    st.wDay = dt.date().day();
    st.wDayOfWeek = dt.date().dayOfWeek();
    st.wMonth = dt.date().month();
    st.wYear = dt.date().year();
    st.wHour = dt.time().hour();
    st.wMinute = dt.time().minute();
    st.wSecond = dt.time().second();
    SetLocalTime(&st);
#endif
}

void Settings::endOfQuery(int code, const QString &message)
{
    if (code) {
        emit endWithError(message);
        return;
    }

    if (m_versionCurrent != m_versionNew) {
        m_versionCurrent = m_versionNew;
        emit newVersion();
        return;
    }

    emit endOfSettingsQuery(code, message);
}

void Settings::endOfUploadPrinters(int code, const QString &message)
{
    Q_UNUSED(code)
    Q_UNUSED(message)
    BaseNetwork::removeInstance(sender());
}

bool Settings::continueStartElement(const QString &qName, const QXmlAttributes &atts)
{
    if (qName == "version") {
        m_versionNew = atts.value("value").toInt();
        return true;
    }

    if (qName == "option") {
        m_settings.insert(atts.value("key_name"), atts.value("key_value"));
        return true;
    }

    return true;
}
