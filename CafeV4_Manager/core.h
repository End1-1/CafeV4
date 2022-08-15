#ifndef CORE_H
#define CORE_H

#include <QLineEdit>
#include <QDateEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QGridLayout>
#include <QDialog>
#include <QMainWindow>
#include <QAction>
#include <QItemDelegate>
#include <QKeyEvent>
#include <QTextOption>
#include <QPainter>
#include <QMdiArea>
#include <QToolBar>
#include <QScrollBar>
#include <QtXml>
#include <QTime>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QtPrintSupport/QPrinter>
#include <QtPrintSupport/QPrintDialog>

#include "../core/toolkit.h"
#include "../core/printing.h"
#include "../core/definitions.h"
#include "../core_gui/dlglist.h"
#include "../core/connection.h"
#include "qsqldrv.h"

#include "printpreview.h"
#include "mainwindow.h"
#include "dockidname.h"
#include "storeorder.h"
#include "ff_user.h"

#define DOC_INPUT 1
#define DOC_OUTPUT 2
#define DOC_MOVEMENT 3
#define DOC_SALE 4
#define DOC_COMPLECT_IN 5
#define DOC_COMPLECT_OUT 6
#define DOC_INVERNTORIZATION 7
#define DOC_ARTADRAMAS 8

#define GRID_WIDTH_PRINT_SCALE 0.2
#define CHECK_VIEWER if (___ff_user->roleRead(ROLE_M_VIEWER)) {QMessageBox::warning(this, QObject::tr("Information"), QObject::tr("Inpossible in viewer mode.")); return;}
#define CHECK_VIEWER_AND_MAINDB if (___ff_user->roleRead(ROLE_M_VIEWER) || !QSystem::isMain()) {QMessageBox::warning(this, QObject::tr("Information"), QObject::tr("Inpossible in viewer mode, or this database is not master.")); return;}
#define CHECK_MAINDB if ()

extern QToolBar *___toolBar;
extern QToolBar *___toolBarBtn;
extern MainWindow *___mainWindow;
extern FF_User *___ff_user;

#endif // CORE_H
