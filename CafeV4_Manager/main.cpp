#include <QApplication>
#include <QFontDatabase>
#include "mainwindow.h"
#include "core.h"
#include "qsqldb.h"
#include "dlgconnection.h"
#include "qgroupquery.h"
#include <QProcess>
#include "cnfmaindb.h"

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

#ifndef QT_DEBUG
    QStringList libPath = QCoreApplication::libraryPaths();
    libPath << "./";
    libPath << "./platforms";
    libPath << "./sqldrivers";
    QCoreApplication::setLibraryPaths(libPath);
#endif

    QSystem system(argv, a.applicationName());
    Q_UNUSED(system);

    QSqlDB::setDbName(QSqlDB::addDatabase(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword), "maindb");
    QSqlDB::setDbName(QSqlDB::addDatabase(__cnfmaindb.fHost, __cnfmaindb.fDatabase, __cnfmaindb.fUser, __cnfmaindb.fPassword), "clientdb");
    QGroupQuery::m_mainDb.append(__cnfmaindb.fHost + ":" + __cnfmaindb.fDatabase);
    QGroupQuery::m_mainDb.append(__cnfmaindb.fUser);
    QGroupQuery::m_mainDb.append(__cnfmaindb.fPassword);

    QString trFile = ":/CafeV4.qm";
    QFile f(trFile);
    if (!f.exists()) {
        QMessageBox::critical(nullptr, "Translator", "Could not load the file " + trFile);
        trFile = "./CafeV4.qm";
    }
    if (!f.exists(trFile)) {
        QMessageBox::critical(nullptr, "Translator", "Could not load the file " + trFile);
    }

    QTranslator *t = new QTranslator(nullptr);
    if (!t->load(trFile)) {
        QMessageBox::critical(nullptr, "Translator", trFile);
    }
    a.installTranslator(t);


    QFontDatabase fontDb;
    int fontId = fontDb.addApplicationFont(a.applicationDirPath() + "./ahuni.ttf");
    QString fontFamily = fontDb.applicationFontFamilies(fontId).at(0);
    QFont font = fontDb.font(fontFamily, "Normal", 11);
    a.setFont(font);
    MainWindow w;
    w.setWindowIcon(QIcon(":/img/app.png"));
    w.showMaximized();
    w.on_actionLogin_triggered();
    
    a.exec();

    return 0;
}
