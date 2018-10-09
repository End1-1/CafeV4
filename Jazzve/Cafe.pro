#-------------------------------------------------
#
# Project created by QtCreator 2014-04-04T11:00:17
#
#-------------------------------------------------

QT       += core gui network xml printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cafe
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ../core/connection.cpp \
    ../core/order.cpp \
    core.cpp \
    ../core/stationauth.cpp \
    ../core/basenetwork.cpp \
    dlgorder.cpp \
    ../core/menu.cpp \
    ../core_gui/dlgmessagebox.cpp \
    ../core_gui/dlgprogress.cpp \
    ../core/toolkit.cpp \
    ../CafeV4_Station/dlgdishcomment.cpp \
    ../core/printing.cpp \
    dlgchangepassword.cpp \
    wdtnumpad.cpp \
    dlggetpassword.cpp \
    dlgtables.cpp \
    ../core/settings.cpp \
    ../core/orderstoredoc.cpp \
    ../CafeV4_Manager/dlgmaindbsettings.cpp \
    ../core/ordermodifier.cpp \
    ../core_gui/dlglist.cpp \
    ../core_gui/dlginput.cpp \
    ../core_gui/dlgorders.cpp \
    ../core/hall2.cpp \
    ../core/baseobject.cpp \
    dlgdebtcode.cpp \
    dlgorderhistory.cpp \
    qcustomlineedit.cpp \
    ../core/net.cpp \
    ../core/xml.cpp \
    ../core/cafenetwork.cpp

HEADERS  += mainwindow.h \
    core.h \
    ../core/connection.h \
    ../core/order.h \
    ../core/definitions.h \
    ../core/stationauth.h \
    ../core/basenetwork.h \
    dlgorder.h \
    ../core/menu.h \
    ../core_gui/dlgmessagebox.h \
    ../core_gui/dlgprogress.h \
    ../core/toolkit.h \
    ../CafeV4_Station/dlgdishcomment.h \
    ../core/printing.h \
    dlgchangepassword.h \
    wdtnumpad.h \
    dlggetpassword.h \
    dlgtables.h \
    ../core/settings.h \
    ../core/orderstoredoc.h \
    ../CafeV4_Manager/dlgmaindbsettings.h \
    ../core/ordermodifier.h \
    ../core_gui/dlglist.h \
    ../core_gui/dlginput.h \
    ../core_gui/dlgorders.h \
    ../core/baseobject.h \
    ../core/hall2.h \
    dlgdebtcode.h \
    dlgorderhistory.h \
    qcustomlineedit.h \
    ../core/net.h \
    ../core/xml.h \
    ../core/cafenetwork.h

FORMS    += mainwindow.ui \
    dlgorder.ui \
    ../core_gui/dlgmessagebox.ui \
    ../core_gui/dlgprogress.ui \
    ../CafeV4_Station/dlgdishcomment.ui \
    dlgchangepassword.ui \
    wdtnumpad.ui \
    dlggetpassword.ui \
    dlgtables.ui \
    ../CafeV4_Manager/dlgmaindbsettings.ui \
    ../core_gui/dlglist.ui \
    ../core_gui/dlginput.ui \
    ../core_gui/dlgorders.ui \
    dlgdebtcode.ui \
    dlgorderhistory.ui

RESOURCES += \
    ../core_gui/res.qrc

DEFINES += \
    WAITER_EXE \
    FITNES


ICON = ../core_gui/res/icons/app.ico

RC_FILE = res.rc

VERSION = 4.2.1.58

CONFIG += static

QMAKE_LFLAGS += -static-libgcc -static-libstdc++ -static -lpthread
