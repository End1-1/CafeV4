#include "xlsxcontenttype.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"
#include "xlsxsheet.h"

XlsxContentType::XlsxContentType(XlsxDocument *document) :
    Xlsx()
{
    fDocument = document;
    fZipFileName = "[Content_Types].xml";
}

void XlsxContentType::buildExcelData()
{
    fExcelData =
           "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
           "<Types xmlns=\"http://schemas.openxmlformats.org/package/2006/content-types\">"
           "<Default Extension=\"rels\" ContentType=\"application/vnd.openxmlformats-package.relationships+xml\"/>"
           "<Default Extension=\"xml\" ContentType=\"application/xml\"/>"
           "<Override PartName=\"/xl/workbook.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet.main+xml\"/>";
    for (int i = 0; i < fDocument->workbook()->sheetsCount(); i++) {
        fExcelData += "<Override PartName=\"/xl/worksheets/" + fDocument->workbook()->sheet(i)->name().toLower() + ".xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.worksheet+xml\"/>";
    }
    fExcelData +=
           "<Override PartName=\"/xl/styles.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.styles+xml\"/>"
           "<Override PartName=\"/xl/sharedStrings.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.spreadsheetml.sharedStrings+xml\"/>"
           "<Override PartName=\"/docProps/core.xml\" ContentType=\"application/vnd.openxmlformats-package.core-properties+xml\"/>"
           "<Override PartName=\"/docProps/app.xml\" ContentType=\"application/vnd.openxmlformats-officedocument.extended-properties+xml\"/>"
           "</Types>";
}
