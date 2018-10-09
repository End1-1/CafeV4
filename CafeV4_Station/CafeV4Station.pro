#-------------------------------------------------
#
# Project created by QtCreator 2014-03-16T11:21:47
#
#-------------------------------------------------

QT       += core gui network xml printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CafeV4Station
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    dlgworkspace.cpp \
    dlgdishcomment.cpp \
    ../core/connection.cpp \
    core.cpp \
    ../core/order.cpp \
    ../core/menu.cpp \
    ../core/hall.cpp \
    ../core/stationauth.cpp \
    ../core/basenetwork.cpp \
    ../core/toolkit.cpp \
    ../core_gui/dlgmessagebox.cpp \
    ../core_gui/dlgprogress.cpp \
    ../core/orderstoredoc.cpp \
    ../core/orderprinting.cpp \
    ../core/printing.cpp \
    ../CafeV4_Manager/dlgmaindbsettings.cpp \
    ../core/settings.cpp \
    ../core/ordermodifier.cpp \
    wdtcalculator.cpp \
    dlgcalculator.cpp \
    dlgchange.cpp \
    ../core_gui/dlglist.cpp \
    ../core_gui/dlginput.cpp \
    ../Jazzve/dlggetpassword.cpp \
    ../Jazzve/wdtnumpad.cpp

HEADERS  += mainwindow.h \
    dlgworkspace.h \
    dlgdishcomment.h \
    ../core/connection.h \
    core.h \
    ../core/order.h \
    ../core/menu.h \
    ../core/definitions.h \
    ../core/hall.h \
    ../core/stationauth.h \
    ../core/basenetwork.h \
    ../core/toolkit.h \
    ../core_gui/dlgmessagebox.h \
    ../core_gui/dlgprogress.h \
    ../core/orderstoredoc.h \
    ../core/orderprinting.h \
    ../core/printing.h \
    ../CafeV4_Manager/dlgmaindbsettings.h \
    ../core/settings.h \
    ../core/ordermodifier.h \
    wdtcalculator.h \
    dlgcalculator.h \
    dlgchange.h \
    ../core_gui/dlglist.h \
    ../core_gui/dlginput.h \
    ../Jazzve/dlggetpassword.h \
    ../Jazzve/wdtnumpad.h

FORMS    += mainwindow.ui \
    dlgworkspace.ui \
    dlgdishcomment.ui \
    ../core_gui/dlgmessagebox.ui \
    ../core_gui/dlgprogress.ui \
    ../CafeV4_Manager/dlgmaindbsettings.ui \
    wdtcalculator.ui \
    dlgcalculator.ui \
    dlgchange.ui \
    ../core_gui/dlglist.ui \
    ../core_gui/dlginput.ui \
    ../Jazzve/dlggetpassword.ui \
    ../Jazzve/wdtnumpad.ui

RESOURCES += \
    ../core_gui/res.qrc

DEFINES += WAITER_EXE \
    JOSE

ICON = ../core_gui/res/icons/app2.ico

VERSION = 1.1.1.21

RC_FILE = ../core_gui/res.rc
