#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>
#include <QTranslator>
#include <QDir>
#include "core.h"

int main(int argc, char *argv[])
{
    QString appFullPath(argv[0]);
    appFullPath = appFullPath.mid(0, appFullPath.lastIndexOf("\\"));
    QStringList paths = QCoreApplication::libraryPaths();
    paths.append(appFullPath);
    QCoreApplication::setLibraryPaths(paths);

    Connection::readMainDatabaseFile();

    QApplication a(argc, argv);

    QTranslator t;
    t.load(":/new/prefix1/CafeV4Station.qm");
    a.installTranslator(&t);

    QFontDatabase fontDb;
    int fontId = fontDb.addApplicationFont("./ahuni.ttf");
    QString fontFamily = fontDb.applicationFontFamilies(fontId).at(0);
    QFont font = fontDb.font(fontFamily, "Normal", 11);
    a.setFont(font);

    MainWindow w;
    ___progressDialog = new DlgProgress(&w);
    w.show();
    
    return a.exec();
}
