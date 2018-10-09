#ifndef CONNECTION_H
#define CONNECTION_H

#include <QHostInfo>

class Connection
{
    struct MainDatabaseFile {
        char server_url[1024];
        char server_user[32];
        char server_pass[32];
        char settings_pass[32];
        char waiter_settings[128];
        char waiter_db_id[32];
        char last_user[32];
        char last_db[32];

        char s_rep_complect_visible;
        char s_rep_view_prices;

        char reserved[4062];
    };

private:
    static MainDatabaseFile m_file;
    static QHostInfo m_hostInfo;
    static QString m_url;

public:
    Connection();
    static void encodeMainDatabaseFile();
    static void decodeMainDatabaseFile();
    static void readMainDatabaseFile();
    static void writeMainDatabaseFile();
    const static QString serverUrl();
    const static QString url();
    const static QString user();
    const static QString password();
    const static QString settingsPassword();
    const static QString hostName();
    const static QString lockName();
    const static QString waiterSettings();
    const static QString lastUser();
    static int lastDbId();
    const static QString dbId();
    const static QString homeDir();
    const static QString programPath();
    const static QString path(const QString &alias);
    const static QString server();
    static QString userName();
    static void setServerUrl(const QString &url);
    static void setUrl(const QString &url);
    static void setUser(const QString &user);
    static void setPassword(const QString &password);
    static void setSettingsPassword(const QString &settingsPassword);
    static void setWaiterSettings(const QString &waiterSettings);
    static void setLastUser(const QString &lastUser);
    static void setLastDbId(int dbId);
};

#endif // CONNECTION_H
