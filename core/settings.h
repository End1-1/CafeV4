#ifndef SETTINGS_H
#define SETTINGS_H

#include "basenetwork.h"

#define SETTINGS_MENU_NAME "1"
#define SETTINGS_CASH_DATE_CHANGE "2"
#define SETTINGS_HALL_ID "3"
#define SETTINGS_CHECKOUT_PRINTER "4"
#define SETTINGS_DB "5"
#define SETTINGS_AVAILABLE_HALLS "6"

class Settings : public BaseNetwork
{
    Q_OBJECT

public:
    Settings(const QString &settingsName);
    void uploadPrinters(const QString &session);
    void resetVersion();
    void loadSettings(const QString &session, const QString module);
    QString toString(const QString &name);
    int toInt(const QString &name);
    QDate toDate(const QString &name);
    double toDouble(const QString &name);
    const QVariant toVariant(const QString &name);
    const QString cashDate();

public slots:
    void setSystemDateTime(const QString &dateTime);

private:
    int m_versionCurrent;
    int m_versionNew;
    QString m_settingsName;
    QMap<QString, QVariant> m_settings;

private slots:
    void endOfQuery(int code, const QString &message);
    void endOfUploadPrinters(int code, const QString &message);

protected:
    bool continueStartElement(const QString &qName, const QXmlAttributes &atts);

signals:
    void newVersion();
    void endWithError(const QString &message);
    void endOfSettingsQuery(int code, const QString &message);
};

#endif // SETTINGS_H
