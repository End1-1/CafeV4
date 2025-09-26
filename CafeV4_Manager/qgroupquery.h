#ifndef QGROUPQUERY_H
#define QGROUPQUERY_H

#include "mdiwindow.h"
#include "qgroupqueryfilter.h"
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QThread>
#include <QTableWidgetItem>
#include <QMutex>
#include <QListWidgetItem>

typedef QList<QList<QVariant> > DATA;

namespace Ui {
class QGroupQuery;
}

class QDbThread : public QObject {
    Q_OBJECT
public:
    QDbThread(const QString &dbName, const QString &dbUser, const QString &dbPass);
    int row;
    QString sqlText;
    QMap<QString, QVariant> bindValue;
    QString alias;
public slots:
    virtual void run();
private:
    static QMutex m_mutex;
    QString m_dbName;
    QString m_userName;
    QString m_password;
    static QString m_dbNum;
    QString getDbId();
signals:
    void status(int row, const QString &msg);
    void fetchResult(DATA data);
    void fetchColumns(QStringList colNames);
    void finish();
};

class QGroupQuery : public MdiWindow
{
    Q_OBJECT

public:
    static QToolBar *m_toolBarBtn;
    static QToolBar *m_toolBarWnd;
    static QStringList m_mainDb;
    explicit QGroupQuery(QWidget *parent = 0);
    ~QGroupQuery();
    virtual void actionExcel(QString fileName = "");
    virtual void actionRefresh();
    virtual bool actionFilter();

private:
    Ui::QGroupQuery *ui;
    QMutex m_mutex;
    QList<BindValue> m_bindValues;
    QString m_queryTitle;
    void init();
    void saveHistory();
    void loadHistory();

private slots:
    void dbthreadStatus(int row, const QString &msg);
    void dbthreadFetchResult(DATA data);
    void dbthreadFetchColumns(QStringList colNames);
    void on_lstHistory_itemDoubleClicked(QListWidgetItem *item);
};

#endif // QGROUPQUERY_H
