#include "mainwindow.h"
#include "core.h"
#include <QApplication>
#include <QTranslator>
#include <QFontDatabase>
#include <QMessageBox>
#include <QDir>
#include <QStyleFactory>

#ifdef WIN32
#include <windows.h>
#endif

int main(int argc, char *argv[])
{
    QString appFullPath(argv[0]);
    appFullPath = appFullPath.mid(0, appFullPath.lastIndexOf("\\"));
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(appFullPath);
    QCoreApplication::setLibraryPaths(paths);

    QApplication a(argc, argv);

    Connection::readMainDatabaseFile();
    Connection::setUrl(Connection::serverUrl());

    QFontDatabase fontDb;
    QString fontFile = a.applicationDirPath() + "/ahuni.ttf";
    int fontId = fontDb.addApplicationFont(fontFile);
    QString fontFamily = fontDb.applicationFontFamilies(fontId).at(0);
    QFont font = fontDb.font(fontFamily, "Normal", 11);
    a.setFont(font);

    QTranslator t;
    t.load(":/new/prefix1/Cafe.qm");
    a.installTranslator(&t);



    MainWindow w;
    w.showFullScreen();
    
    return a.exec();
}
