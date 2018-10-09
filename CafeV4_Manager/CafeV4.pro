#-------------------------------------------------
#
# Project created by QtCreator 2014-03-04T14:38:15
#
#-------------------------------------------------

QT       += core gui network xml printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CafeV4
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    basegrid.cpp \
    dlglogin.cpp \
    dlgmaindbsettings.cpp \
    dlgsettingspassword.cpp \
    dlgobject.cpp \
    core.cpp \
    ../core/stationauth.cpp \
    xmlgridhandler.cpp \
    dblist.cpp \
    widgethandler.cpp \
    saveobjecthandler.cpp \
    ../core/connection.cpp \
    ../core/basenetwork.cpp \
    storeorder.cpp \
    dlgprogress.cpp \
    dockidname.cpp \
    ../core/toolkit.cpp \
    ../core/printing.cpp \
    printpreview.cpp \
    dlgfilter.cpp \
    dlgfilterlist.cpp \
    dlgrecipe.cpp \
    foodgridwidget.cpp \
    mdibutton.cpp \
    dlgpassword.cpp \
    inventoraztion.cpp \
    dlglistmembers.cpp \
    dlgstationcontrol.cpp \
    dlgordermod.cpp \
    dlgdishspecialcomment.cpp \
    dlgroles.cpp \
    dlgorder.cpp \
    ../core/order.cpp \
    ../core/menu.cpp \
    dlgrecalculation.cpp \
    ../core/orderstoredoc.cpp \
    dlgcombodishproperty.cpp \
    dlgmenuitem.cpp \
    dlgmakehall.cpp \
    dlgfoodnamesgroupchange.cpp \
    dlgdishtorecipe.cpp \
    ../core/ordermodifier.cpp \
    dlgcashdoc.cpp \
    dlgsalarydoc.cpp \
    dlgsimpleselect.cpp \
    dlgabout.cpp \
    dlgsalaryformula.cpp \
    dlgaddsalaryfunction.cpp \
    qcostumcombobox.cpp \
    ../../FastF/qsqldrv.cpp \
    dlgcalcselfcost.cpp

HEADERS  += mainwindow.h \
    basegrid.h \
    dlglogin.h \
    dlgmaindbsettings.h \
    dlgsettingspassword.h \
    dlgobject.h \
    core.h \
    ../core/stationauth.h \
    xmlgridhandler.h \
    dblist.h \
    widgethandler.h \
    saveobjecthandler.h \
    ../core/connection.h \
    ../core/basenetwork.h \
    storeorder.h \
    dlgprogress.h \
    dockidname.h \
    ../core/toolkit.h \
    ../core/printing.h \
    printpreview.h \
    dlgfilter.h \
    dlgfilterlist.h \
    dlgrecipe.h \
    foodgridwidget.h \
    mdibutton.h \
    dlgpassword.h \
    inventoraztion.h \
    dlglistmembers.h \
    dlgstationcontrol.h \
    ../core/DEF_SETTINGS.h \
    dlgordermod.h \
    dlgdishspecialcomment.h \
    dlgroles.h \
    dlgorder.h \
    ../core/order.h \
    ../core/menu.h \
    dlgrecalculation.h \
    ../core/orderstoredoc.h \
    dlgcombodishproperty.h \
    dlgmenuitem.h \
    dlgmakehall.h \
    dlgfoodnamesgroupchange.h \
    dlgdishtorecipe.h \
    ../core/ordermodifier.h \
    dlgcashdoc.h \
    dlgsalarydoc.h \
    dlgsimpleselect.h \
    dlgabout.h \
    dlgsalaryformula.h \
    dlgaddsalaryfunction.h \
    qcostumcombobox.h \
    ../../FastF/qsqldrv.h \
    dlgcalcselfcost.h

FORMS    += mainwindow.ui \
    basegrid.ui \
    dlglogin.ui \
    dlgmaindbsettings.ui \
    dlgsettingspassword.ui \
    dlgobject.ui \
    storeorder.ui \
    dlgprogress.ui \
    dockidname.ui \
    printpreview.ui \
    dlgfilter.ui \
    dlgfilterlist.ui \
    dlgrecipe.ui \
    foodgridwidget.ui \
    mdibutton.ui \
    dlgpassword.ui \
    inventoraztion.ui \
    dlglistmembers.ui \
    dlgstationcontrol.ui \
    dlgordermod.ui \
    dlgdishspecialcomment.ui \
    dlgroles.ui \
    dlgorder.ui \
    dlgrecalculation.ui \
    dlgcombodishproperty.ui \
    dlgmenuitem.ui \
    dlgmakehall.ui \
    dlgfoodnamesgroupchange.ui \
    dlgdishtorecipe.ui \
    dlgcashdoc.ui \
    dlgsalarydoc.ui \
    dlgsimpleselect.ui \
    dlgabout.ui \
    dlgsalaryformula.ui \
    dlgaddsalaryfunction.ui \
    dlgcalcselfcost.ui

RESOURCES += \
    res.qrc

ICON =res/app.ico

RC_FILE = res.rc

VERSION = \\\"'1.2.1.35'\\\"

DEFINES += \

DEFINES += \
    MANAGER_EXE \
    JAZZVE \
    "FILE_VERSION=$${VERSION}"

QMAKE_LFLAGS += -static -static-libgcc

