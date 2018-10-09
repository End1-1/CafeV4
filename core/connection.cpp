#include "connection.h"
#include <QDir>
#include <QFile>
#include <QMessageBox>
#ifdef WIN32
#include "windows.h"
#endif
#include <QApplication>

Connection::MainDatabaseFile Connection::m_file;
QHostInfo Connection::m_hostInfo;
QString Connection::m_url;

Connection::Connection()
{
}

void Connection::encodeMainDatabaseFile() {
    char *file = reinterpret_cast<char *>(&m_file);
    for (int i = 0, len = sizeof(m_file); i < len; i++)
        file[i] = file[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
}

void Connection::decodeMainDatabaseFile() {
    char *file = reinterpret_cast<char *>(&m_file);
    for (int i = 0, len = sizeof(m_file); i < len; i++)
        file[i] = file[i] ^ ((i % 2) + (i % 3) + (i % 4) + (i % 5) + (i % 6) + (i % 7) + (i % 8) + (i % 9));
}

void Connection::readMainDatabaseFile()
{
    QDir dir;
    if (!dir.exists(dir.homePath() + "/Cafe"))
            dir.mkdir(dir.homePath() + "/Cafe");

    memset(&m_file, 0, sizeof(MainDatabaseFile));
    QFile file(dir.homePath() + "/Cafe/maindatabase");
    if (!file.exists())
    {
        file.open(QIODevice::WriteOnly);
        encodeMainDatabaseFile();
        file.write(reinterpret_cast<const char *>(&m_file), sizeof(MainDatabaseFile));
        file.close();
    }

    if (file.open(QIODevice::ReadOnly))
        file.read(reinterpret_cast<char *>(&m_file), sizeof(MainDatabaseFile));

    file.close();
    decodeMainDatabaseFile();
}

void Connection::writeMainDatabaseFile()
{
    QDir dir;
    if (!dir.exists(dir.homePath() + "/Cafe"))
        if (!dir.mkdir(dir.homePath() + "/Cafe"))
            QMessageBox::critical(0, "Error", "Cannot create home director");

    encodeMainDatabaseFile();
    QFile file(dir.homePath() + "/Cafe/maindatabase");
    if (file.open(QIODevice::WriteOnly)) {
        file.flush();
        file.write(reinterpret_cast<const char *>(&m_file), sizeof(MainDatabaseFile));
        file.close();
    }
    decodeMainDatabaseFile();
    file.close();
}

const QString Connection::serverUrl()
{
    return QString(m_file.server_url);
}

const QString Connection::url()
{
    return m_url;
}

const QString Connection::user()
{
    return QString(m_file.server_user);
}

const QString Connection::password()
{
    return QString(m_file.server_pass);
}

const QString Connection::settingsPassword()
{
    return QString(m_file.settings_pass);
}

const QString Connection::hostName()
{
    return m_hostInfo.localHostName();
}

const QString Connection::lockName()
{
    return m_hostInfo.localHostName() + ":" + userName();
}

const QString Connection::waiterSettings()
{
    return QString::fromUtf8(m_file.waiter_settings);
}

const QString Connection::lastUser()
{
    return QString(m_file.last_user);
}

int Connection::lastDbId()
{
    return QString(m_file.last_db).toInt();
}

const QString Connection::dbId()
{
    return QString(m_file.waiter_db_id);
}

const QString Connection::homeDir()
{
    QDir dir;
    return dir.homePath();
}

const QString Connection::programPath()
{
    return QApplication::applicationDirPath() + "/";
}

const QString Connection::path(const QString &alias)
{
    if (alias == "%PROGRAM_DIR%")
        return programPath();
    else
        return alias;
}

const QString Connection::server()
{
    return m_file.server_url;
}

QString Connection::userName()
{
#ifdef WIN32
    wchar_t userName[100];
    DWORD userNameSize = sizeof(userName);
    memset(&userName[0], 0, userNameSize);
    GetUserName(userName, &userNameSize);
    return QString::fromWCharArray(userName);
#endif
    return "Unknown";
}

void Connection::setServerUrl(const QString &url)
{
    memcpy(&m_file.server_url[0], url.toUtf8(), sizeof(m_file.server_url));
}

void Connection::setUrl(const QString &url)
{
    m_url = url;
}

void Connection::setUser(const QString &user)
{
    memcpy(&m_file.server_user[0], user.toUtf8(), sizeof(m_file.server_user));
}

void Connection::setPassword(const QString &password)
{
    memcpy(&m_file.server_pass[0], password.toUtf8(), sizeof(m_file.server_pass));
}

void Connection::setSettingsPassword(const QString &settingsPassword)
{
    memcpy(&m_file.settings_pass[0], settingsPassword.toUtf8(), sizeof(m_file.settings_pass));
}

void Connection::setWaiterSettings(const QString &waiterSettings)
{
    memcpy(&m_file.waiter_settings[0], waiterSettings.toUtf8(), sizeof(m_file.waiter_settings));
}

void Connection::setLastUser(const QString &lastUser)
{
    memcpy(&m_file.last_user[0], lastUser.toUtf8(), sizeof(m_file.last_user));
}

void Connection::setLastDbId(int dbId)
{
    QString id = QString("%1").arg(dbId);
    memcpy(&m_file.last_db[0], id.toUtf8(), sizeof(m_file.last_db));
}
