#include "xlsxdocument.h"
#include "xlsxwriter.h"
#include "xlsxworkbook.h"
#include "xlsxstyles.h"
#include "xlsxtheme.h"
#include "xlsxdocpropsapp.h"
#include "xlsxdocpropscore.h"
#include "xlsxsharedstring.h"
#include "xlsxcontenttype.h"
#include "xlsxrels.h"
#include "xlsxsheet.h"
#include <QFileDialog>
#include <QDesktopServices>
#include <QStandardPaths>

XlsxDocument::XlsxDocument()
{
    fContentTypes = new XlsxContentType(this);
    fDocPropsApp = new XlsxDocPropsApp();
    fDocPropsCore = new XlsxDocPropsCore();
    fRels = new XlsxRels();
    fWorkBook = new XlsxWorkBook();
    fWorkBook->fDocument = this;
    fStyles = new XlsxStyles();
    fTheme = new XlsxTheme();
}

XlsxDocument::~XlsxDocument()
{
    if (fWorkBook) {
        delete fWorkBook;
    }
    delete fContentTypes;
    delete fDocPropsApp;
    delete fDocPropsCore;
    delete fStyles;
    delete fTheme;
}

XlsxWorkBook *XlsxDocument::addWorkbook()
{
    if (!fWorkBook) {
        fWorkBook = new XlsxWorkBook();
    }
    return fWorkBook;
}

XlsxWorkBook *XlsxDocument::workbook()
{
    return fWorkBook;
}

XlsxStyles *XlsxDocument::style()
{
    return fStyles;
}

bool XlsxDocument::save(QString &err, bool open)
{
    QString fileName = QFileDialog::getSaveFileName(nullptr, "Save", QStandardPaths::writableLocation(QStandardPaths::DesktopLocation), "*.xlsx");
    if (fileName.isEmpty()) {
        return false;
    }
    bool result = save(fileName, err);
    if (result) {
        if (open) {
            QDesktopServices::openUrl(QUrl(fileName));
        }
    }
    return result;
}

bool XlsxDocument::save(const QString &fileName, QString &err)
{
    if (!fWorkBook) {
        err = "Workbook missing";
        return false;
    }
    XlsxWriter xw(fileName);
    if (!xw.fError.isEmpty()) {
        err = xw.fError;
        return false;
    }

    fContentTypes->writeToFile(&xw);
    fDocPropsCore->writeToFile(&xw);
    fDocPropsApp->writeToFile(&xw);
    fRels->writeToFile(&xw);
    fStyles->writeToFile(&xw);
    //fTheme->writeToFile(&xw);
    fWorkBook->writeToFile(&xw);
    for (int i = 0; i < fWorkBook->sheetsCount(); i++) {
        fWorkBook->sheet(i)->writeToFile(&xw);
    }
    xw.writeEOD();
    return true;
}
