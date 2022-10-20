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
INCLUDEPATH += ./gr
INCLUDEPATH += ./gr/SelectorModel
INCLUDEPATH += C:/projects/FastF
INCLUDEPATH += C:/Projects/FastFServer
INCLUDEPATH += C:/Projects/FastFServer/phonejson
INCLUDEPATH += C:/Projects/XLSX/src

SOURCES += main.cpp\
    ../../FastFServer/phonejson/threadobject.cpp \
    ../../FastFServer/sqlthread.cpp \
    ../../XLSX/src/xlsx.cpp \
    ../../XLSX/src/xlsxborder.cpp \
    ../../XLSX/src/xlsxcell.cpp \
    ../../XLSX/src/xlsxcontenttype.cpp \
    ../../XLSX/src/xlsxdocpropsapp.cpp \
    ../../XLSX/src/xlsxdocpropscore.cpp \
    ../../XLSX/src/xlsxdocument.cpp \
    ../../XLSX/src/xlsxrels.cpp \
    ../../XLSX/src/xlsxsharedstring.cpp \
    ../../XLSX/src/xlsxsheet.cpp \
    ../../XLSX/src/xlsxstyles.cpp \
    ../../XLSX/src/xlsxtheme.cpp \
    ../../XLSX/src/xlsxworkbook.cpp \
    ../../XLSX/src/xlsxwriter.cpp \
    dlgratewaiters.cpp \
    gr/SelectorModel/grmetypes.cpp \
    gr/SelectorModel/grselectormodel.cpp \
    gr/grdb.cpp \
    gr/grideditordialog.cpp \
    gr/grideditorwidget.cpp \
    gr/grmenu.cpp \
    gr/grselector.cpp \
    gr/imenuitem.cpp \
    gridwidget.cpp \
        mainwindow.cpp \
    dlglogin.cpp \
    dlgsettingspassword.cpp \
    core.cpp \
    ../core/connection.cpp \
    stafffood.cpp \
    storeorder.cpp \
    dockidname.cpp \
    ../core/toolkit.cpp \
    ../core/printing.cpp \
    printpreview.cpp \
    dlgfilterlist.cpp \
    foodgridwidget.cpp \
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
    table/cfilterdialog.cpp \
    ../../FastF/cnfmaindb.cpp \
    table/ceditdialog.cpp \
    tablewidget.cpp

HEADERS  += mainwindow.h \
    ../../FastFServer/phonejson/threadobject.h \
    ../../FastFServer/sqlthread.h \
    ../../XLSX/src/crs32.h \
    ../../XLSX/src/xlsx.h \
    ../../XLSX/src/xlsxall.h \
    ../../XLSX/src/xlsxborder.h \
    ../../XLSX/src/xlsxcell.h \
    ../../XLSX/src/xlsxcontenttype.h \
    ../../XLSX/src/xlsxdocpropsapp.h \
    ../../XLSX/src/xlsxdocpropscore.h \
    ../../XLSX/src/xlsxdocument.h \
    ../../XLSX/src/xlsxrels.h \
    ../../XLSX/src/xlsxsharedstring.h \
    ../../XLSX/src/xlsxsheet.h \
    ../../XLSX/src/xlsxstyles.h \
    ../../XLSX/src/xlsxtheme.h \
    ../../XLSX/src/xlsxworkbook.h \
    ../../XLSX/src/xlsxwriter.h \
    ../../XLSX/src/zip.h \
    dlglogin.h \
    dlgratewaiters.h \
    dlgsettingspassword.h \
    core.h \
    ../core/connection.h \
    gr/SelectorModel/grmetypes.h \
    gr/SelectorModel/grselectormodel.h \
    gr/grdb.h \
    gr/grideditordialog.h \
    gr/grideditorwidget.h \
    gr/grmenu.h \
    gr/grselector.h \
    gr/imenuitem.h \
    gridwidget.h \
    stafffood.h \
    storeorder.h \
    dockidname.h \
    ../core/toolkit.h \
    ../core/printing.h \
    printpreview.h \
    dlgfilterlist.h \
    foodgridwidget.h \
    dlgpassword.h \
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
    table/cfilterdialog.h \
    ../../FastF/cnfmaindb.h \
    table/ceditdialog.h \
    tablewidget.h

FORMS    += mainwindow.ui \
    dlglogin.ui \
    dlgratewaiters.ui \
    dlgsettingspassword.ui \
    dlgobject.ui \
    gr/grideditordialog.ui \
    gr/grselector.ui \
    gr/imenuitem.ui \
    gridwidget.ui \
    stafffood.ui \
    storeorder.ui \
    dockidname.ui \
    printpreview.ui \
    dlgfilterlist.ui \
    foodgridwidget.ui \
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
DEFINES += _MODULE_=\\\"CAFE4\\\"


LIBS += -lVersion

DISTFILES += \
    img/guid \
    CafeV4.qm

