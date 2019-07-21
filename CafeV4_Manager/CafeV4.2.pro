#-------------------------------------------------
#
# Project created by QtCreator 2014-03-04T14:38:15
#
#-------------------------------------------------

QT       += core gui network xml printsupport sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cafe
TEMPLATE = app

RC_FILE = res.rc

INCLUDEPATH += ./table
INCLUDEPATH += C:/projects/FastF

SOURCES += main.cpp\
        mainwindow.cpp \
    dlglogin.cpp \
    dlgsettingspassword.cpp \
    core.cpp \
    ../core/connection.cpp \
    storeorder.cpp \
    dlgprogress.cpp \
    dockidname.cpp \
    ../core/toolkit.cpp \
    ../core/printing.cpp \
    printpreview.cpp \
    dlgfilterlist.cpp \
    foodgridwidget.cpp \
    mdibutton.cpp \
    dlgpassword.cpp \
    dlgrecalculation.cpp \
    dlgsalarydoc.cpp \
    dlgsimpleselect.cpp \
    dlgabout.cpp \
    dlgsalaryformula.cpp \
    dlgaddsalaryfunction.cpp \
    qcostumcombobox.cpp \
    ../../common/qsqldrv.cpp \
    ../../common/qsystem.cpp \
    ../../common/qlogwindow.cpp \
    dlgcalcselfcost.cpp \
    ../../common/qpcombobox.cpp \
    ../../common/qsqlcache.cpp \
    ../../common/qbasesqldriver.cpp \
    ../../common/SqlField.cpp \
    ../../common/qdlgbase.cpp \
    ../../common/fbtnokcancel.cpp \
    ../../common/mdiwindow.cpp \
    ../../common/qbasegrid.cpp \
    ../../common/qdlgfilter.cpp \
    ../../common/qdlgcache.cpp \
    ../../FastF/ff_user.cpp \
    gsettings.cpp \
    ../../Engine/distrimex/qgrid.cpp \
    gdsettings.cpp \
    ../../common/qplineedit.cpp \
    ../../common/qdlgquery.cpp \
    ../../common/qbasesqlwindow.cpp \
    qeditwsettings.cpp \
    qsinchronizedatabasechanges.cpp \
    qdlgemployesaccess.cpp \
    gmenu.cpp \
    magicmenu.cpp \
    qimportfromas.cpp \
    dlgvaluepair.cpp \
    ../../common/qsqldb.cpp \
    gsalerreportcommon.cpp \
    gdocinstore.cpp \
    gsalarydoc.cpp \
    gfullfinalmovement.cpp \
    gemployes.cpp \
    dlgemployee.cpp \
    qemployesgroups.cpp \
    gdemployesgroups.cpp \
    qsale.cpp \
    dlgorder.cpp \
    qmenunames.cpp \
    dlgmenuname.cpp \
    qdishestypes.cpp \
    dlgdishtype.cpp \
    qhall.cpp \
    dlghall.cpp \
    qstorages.cpp \
    dlgstorages.cpp \
    ../../common/dlgconnection.cpp \
    qfoodgroup.cpp \
    dlgfoodgroups.cpp \
    qfoodunits.cpp \
    dlggoodsunits.cpp \
    dlggoods.cpp \
    qfoods.cpp \
    qprintschema.cpp \
    dlgprintschema.cpp \
    qprintersofschema.cpp \
    dlgprinterofschema.cpp \
    qdishesgroupofgroup.cpp \
    dlgdishgroupofgroup.cpp \
    dlgrecipe.cpp \
    dlgfoodcache.cpp \
    qtotalrecipes.cpp \
    logwriter.cpp \
    gremoveorders.cpp \
    dlgsettotalmovementaddoption.cpp \
    dlgdatepair.cpp \
    dlggetdoublevalue.cpp \
    dlgselectsalereptype.cpp \
    customtablewidget.cpp \
    gmenuitem.cpp \
    gtabletcontrol.cpp \
    double.cpp \
    dlgmenucopy.cpp \
    dlgcorrectoutstoreofsales.cpp \
    ../../FastF/qnet.cpp \
    dlgviewimage.cpp \
    qgroupquery.cpp \
    qgroupqueryfilter.cpp \
    ggroupqueries.cpp \
    gdgroupquery.cpp \
    qgroupquerybindvalues.cpp \
    qdlgrecipehistory.cpp \
    gdiscountcard.cpp \
    qdishremovereason.cpp \
    dlgdishremovereason.cpp \
    dlgdiscountcard.cpp \
    dlgcardregister.cpp \
    qsinchronizedatabasethread.cpp \
    dlgorderremovereason.cpp \
    table/cdatabase.cpp \
    table/cgridwidget.cpp \
    table/ctablemodel.cpp \
    nmenu.cpp \
    nrecipes.cpp \
    table/cfilterdialog.cpp \
    nmenunames.cpp \
    ../../FastF/cnfmaindb.cpp \
    table/ceditdialog.cpp

HEADERS  += mainwindow.h \
    dlglogin.h \
    dlgsettingspassword.h \
    core.h \
    ../core/connection.h \
    storeorder.h \
    dlgprogress.h \
    dockidname.h \
    ../core/toolkit.h \
    ../core/printing.h \
    printpreview.h \
    dlgfilterlist.h \
    foodgridwidget.h \
    mdibutton.h \
    dlgpassword.h \
    ../core/DEF_SETTINGS.h \
    dlgrecalculation.h \
    dlgsalarydoc.h \
    dlgsimpleselect.h \
    dlgabout.h \
    dlgsalaryformula.h \
    dlgaddsalaryfunction.h \
    qcostumcombobox.h \
    ../../common/qsqldrv.h \
    ../../common/qsystem.h \
    ../../common/qlogwindow.h \
    dlgcalcselfcost.h \
    qbasegrid.h \
    qsqldrv.h \
    ../../common/qpcombobox.h \
    qpcombobox.h \
    ../../common/qsqlcache.h \
    ../../common/qbasesqldriver.h \
    ../../common/SqlField.h \
    qsqlcache.h \
    ../../common/qdlgbase.h \
    ../../common/fbtnokcancel.h \
    ../../common/mdiwindow.h \
    ../../common/qbasegrid.h \
    ../../common/qdlgfilter.h \
    mdiwindow.h \
    fbtnokcancel.h \
    qdlgcache.h \
    ../../common/qdlgcache.h \
    ff_user.h \
    ../../FastF/ff_user.h \
    qgrid.h \
    gsettings.h \
    ../../Engine/distrimex/qgrid.h \
    gdsettings.h \
    ../../common/qplineedit.h \
    qplineedit.h \
    qdlgquery.h \
    ../../common/qdlgquery.h \
    ../../common/qbasesqlwindow.h \
    qbasesqlwindow.h \
    qeditwsettings.h \
    qsinchronizedatabasechanges.h \
    qdlgemployesaccess.h \
    gmenu.h \
    magicmenu.h \
    qimportfromas.h \
    dlgvaluepair.h \
    qsqldb.h \
    gsalerreportcommon.h \
    gdocinstore.h \
    gsalarydoc.h \
    gfullfinalmovement.h \
    gemployes.h \
    dlgemployee.h \
    qemployesgroups.h \
    gdemployesgroups.h \
    qsale.h \
    dlgorder.h \
    qmenunames.h \
    dlgmenuname.h \
    qdishestypes.h \
    dlgdishtype.h \
    qhall.h \
    dlghall.h \
    qstorages.h \
    dlgstorages.h \
    dlgconnection.h \
    ../../common/dlgconnection.h \
    qfoodgroup.h \
    dlgfoodgroups.h \
    qfoodunits.h \
    dlggoodsunits.h \
    dlggoods.h \
    qfoods.h \
    qprintschema.h \
    dlgprintschema.h \
    qprintersofschema.h \
    dlgprinterofschema.h \
    qdlgbase.h \
    qdishesgroupofgroup.h \
    dlgdishgroupofgroup.h \
    dlgrecipe.h \
    dlgfoodcache.h \
    qtotalrecipes.h \
    logwriter.h \
    printing.h \
    gremoveorders.h \
    dlgsettotalmovementaddoption.h \
    dlgdatepair.h \
    dlggetdoublevalue.h \
    dlgselectsalereptype.h \
    customtablewidget.h \
    gmenuitem.h \
    gtabletcontrol.h \
    double.h \
    dlgmenucopy.h \
    dlgcorrectoutstoreofsales.h \
    ../../FastF/qnet.h \
    qnet.h \
    dlgviewimage.h \
    qgroupquery.h \
    qgroupqueryfilter.h \
    ggroupqueries.h \
    gdgroupquery.h \
    qgroupquerybindvalues.h \
    qdlgrecipehistory.h \
    gdiscountcard.h \
    qdishremovereason.h \
    dlgdishremovereason.h \
    dlgdiscountcard.h \
    dlgcardregister.h \
    qsinchronizedatabasethread.h \
    dlgorderremovereason.h \
    table/cdatabase.h \
    table/cgridwidget.h \
    table/ctablemodel.h \
    nmenu.h \
    nrecipes.h \
    table/cfilterdialog.h \
    nmenunames.h \
    ../../FastF/cnfmaindb.h \
    table/ceditdialog.h

FORMS    += mainwindow.ui \
    dlglogin.ui \
    dlgsettingspassword.ui \
    dlgobject.ui \
    storeorder.ui \
    dlgprogress.ui \
    dockidname.ui \
    printpreview.ui \
    dlgfilterlist.ui \
    foodgridwidget.ui \
    mdibutton.ui \
    dlgpassword.ui \
    dlgrecalculation.ui \
    dlgsalarydoc.ui \
    dlgsimpleselect.ui \
    dlgabout.ui \
    dlgsalaryformula.ui \
    dlgaddsalaryfunction.ui \
    ../../common/qlogwindow.ui \
    dlgcalcselfcost.ui \
    ../../common/qbasegrid.ui \
    ../../common/fbtnokcancel.ui \
    ../../common/qdlgfilter.ui \
    ../../common/qdlgcache.ui \
    gdsettings.ui \
    qeditwsettings.ui \
    qsinchronizedatabasechanges.ui \
    qdlgemployesaccess.ui \
    magicmenu.ui \
    qimportfromas.ui \
    dlgvaluepair.ui \
    dlgemployee.ui \
    gdemployesgroups.ui \
    dlgorder.ui \
    dlgmenuname.ui \
    dlgdishtype.ui \
    dlghall.ui \
    dlgstorages.ui \
    ../../common/dlgconnection.ui \
    dlgfoodgroups.ui \
    dlggoodsunits.ui \
    dlggoods.ui \
    dlgprintschema.ui \
    dlgprinterofschema.ui \
    dlgdishgroupofgroup.ui \
    dlgrecipe.ui \
    dlgfoodcache.ui \
    dlgsettotalmovementaddoption.ui \
    dlgdatepair.ui \
    dlggetdoublevalue.ui \
    dlgselectsalereptype.ui \
    gmenuitem.ui \
    gtabletcontrol.ui \
    dlgmenucopy.ui \
    dlgcorrectoutstoreofsales.ui \
    dlgviewimage.ui \
    qgroupquery.ui \
    qgroupqueryfilter.ui \
    gdgroupquery.ui \
    qgroupquerybindvalues.ui \
    qdlgrecipehistory.ui \
    dlgdishremovereason.ui \
    dlgdiscountcard.ui \
    dlgcardregister.ui \
    dlgorderremovereason.ui \
    table/cgridwidget.ui \
    table/cfilterdialog.ui \
    table/ceditdialog.ui

RESOURCES += img.qrc \
    ../core_gui/res.qrc

ICON = img/app.ico

DEFINES += \
    MANAGER_EXE

DEFINES += _ORGANIZATION_=\\\"Jazzve\\\"
DEFINES += _APPLICATION_=\\\"Cafe4\\\"


LIBS += -lVersion

DISTFILES += \
    img/guid \
    CafeV4.qm

