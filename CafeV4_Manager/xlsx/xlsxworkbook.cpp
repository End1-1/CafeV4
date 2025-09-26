#include "xlsxworkbook.h"
#include "xlsxsheet.h"
#include "xlsxwriter.h"
#include "xlsxsharedstring.h"
#include "xlsxdocument.h"

XlsxWorkBook::XlsxWorkBook() :
    Xlsx()
{
    fZipFileName = "xl\\workbook.xml";
    fSharedStrings = new XlsxSharedString();
}

void XlsxWorkBook::writeToFile(XlsxWriter *xw)
{
    Xlsx::writeToFile(xw);
    QString relFileName = "xl/_rels/workbook.xml.rels";
    QString relData =
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<Relationships xmlns=\"http://schemas.openxmlformats.org/package/2006/relationships\">"
            "<Relationship Id=\"rId1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/sharedStrings\" Target=\"sharedStrings.xml\"/>"
            "<Relationship Id=\"rId2\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/styles\" Target=\"styles.xml\"/>";
    for (int i = 0; i < fSheetsOrder.count(); i++) {
        relData += QString("<Relationship Id=\"rId%1\" Type=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships/worksheet\" Target=\"worksheets/%2.xml\"/>")
                .arg(i + 1 + 2)
                .arg(fSheets[fSheetsOrder.at(i)]->name().toLower());
    }
    relData += "</Relationships>";
    xw->writeLocalHeader(relFileName, relData.toUtf8().data(), relData.toUtf8().length());
    fSharedStrings->writeToFile(xw);
}

void XlsxWorkBook::buildExcelData()
{
    fExcelData =
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<workbook "
            "xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" "
            "xmlns:r=\"http://schemas.openxmlformats.org/officeDocument/2006/relationships\"> "
            "<sheets>";
    for (int i = 0; i < fSheetsOrder.count(); i++) {
        fExcelData += QString("<sheet name=\"%1\" sheetId=\"%2\" r:id=\"rId%2\"/>")
                .arg(fSheets[fSheetsOrder.at(i)]->name())
                .arg(i + 1 + 2);
    }
    fExcelData += "</sheets> "
                  "</workbook>";
}

XlsxSheet *XlsxWorkBook::addSheet(const QString &name)
{
    if (fSheets.contains(name)) {
        return fSheets[name];
    }
    XlsxSheet *s = new XlsxSheet(name);
    s->fSharedStrings = fSharedStrings;
    s->fStyle = fDocument->style();
    fSheets[name] = s;
    fSheetsOrder << name;
    return s;
}

XlsxSheet *XlsxWorkBook::sheet(int index)
{
    XlsxSheet *s = nullptr;
    if (index > fSheets.count() - 1) {
        s = addSheet(QString("NewSheet%1").arg(index));
        return s;
    }
    return fSheets[fSheetsOrder[index]];
}

int XlsxWorkBook::sheetsCount()
{
    return fSheets.count();
}
