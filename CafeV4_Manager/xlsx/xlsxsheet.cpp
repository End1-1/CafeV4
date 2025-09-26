#include "xlsxsheet.h"
#include "xlsxcell.h"
#include "xlsxsharedstring.h"
#include "xlsxstyles.h"
#include <QString>
#include <QDate>

static const int NUMFMID_DATE = 14;
static const int NUMFMID_DATETIME = 22;

XlsxSheet::XlsxSheet(const QString &name) :
    Xlsx()
{
    fName = name;
    fZipFileName = "xl/worksheets/" + name + ".xml";
    fFitToPage = 0;
    fPageSize = xls_page_size_a4;
    fPageOrientation = xls_page_orientation_portrait;
    setupMargins(0,0,0,0,0,0);
}

void XlsxSheet::buildExcelData()
{
    fExcelData =
            "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>"
            "<worksheet xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\">";

    if (fFitToPage > 0){
        fExcelData += QString("<sheetPr><pageSetUpPr fitToPage=\"%1\"/></sheetPr>").arg(fFitToPage);
    }

    fExcelData += "<cols>";
    for (QMap<int, int>::const_iterator it = fColumnWidths.constBegin(); it != fColumnWidths.constEnd(); it++) {
        fExcelData += QString("<col min=\"%1\" max=\"%1\" width=\"%2\" %3costumWidth=\"1\"/>")
                .arg(it.key())
                .arg(it.value())
                .arg(it.value() == 0 ? "hidden=\"1\" " : "");
    }
    fExcelData += "</cols><sheetData>";
    for (QMap<int, QMap<int, XlsxCell *> >::const_iterator ir = fCells.constBegin(); ir != fCells.constEnd(); ir++) {
        fExcelData += QString("<row r=\"%1\">").arg(ir.key());
        for (QMap<int, XlsxCell *>::const_iterator ic = ir.value().begin(); ic != ir.value().end(); ic++) {
            fExcelData += QString("<c s=\"%1\" r=\"%2\"%3><v>%4</v></c>")
                    .arg(ic.value()->style())
                    .arg(ic.value()->address())
                    .arg(ic.value()->fValueType.isEmpty() ? "" : ic.value()->fValueType)
                    .arg(ic.value()->value().toString());
        }
        fExcelData += "</row>";
    }
    fExcelData += "</sheetData>";
    if (fSpan.count() > 0) {
        fExcelData += QString("<mergeCells count=\"%1\">").arg(fSpan.count());
        for (const QString &s: fSpan) {
            fExcelData += QString("<mergeCell ref=\"%1\"/>").arg(s);
        }
        fExcelData += "</mergeCells>";
    }

    fExcelData += QString("<pageMargins left=\"%1\" right=\"%2\" top=\"%3\" bottom=\"%4\" header=\"%5\" footer=\"%6\"/>")
            .arg(fMarginLeft).arg(fMarginRight).arg(fMarginTop).arg(fMarginBottom).arg(fMarginHeader).arg(fMarginFooter);

    fExcelData += QString("<pageSetup paperSize=\"%1\" fitToHeight=\"0\" orientation=\"%2\"/>")
            .arg(fPageSize).arg(fPageOrientation);
    fExcelData += "</worksheet>";
}

QString &XlsxSheet::name()
{
    return fName;
}

XlsxCell *XlsxSheet::addCell(int row, int column, QVariant cellValue, int style)
{
    QString vt;
    switch (cellValue.type()) {
    case QVariant::String:
        cellValue = fSharedStrings->addString(cellValue.toString()
                                              .replace("#", "===|||+++patternchik")
                                              .replace("&", "&#38;")
                                              .replace("===|||+++patternchik", "&#35;")
                                              .replace("*", "&#42;")
                                              .replace("/", "&#47;")
                                              .replace("<", "&lt;")
                                              .replace(">", "&gt;")
                                              .replace("\"", "&quot;")
                                              .replace("'", "&apos;"));
        vt = " t=\"s\"";
        break;
    case QVariant::Date: {
        cellValue = QDate::fromString("1900-01-01", "yyyy-MM-dd").daysTo(cellValue.toDate()) + 2;
        QString newStyle = fStyle->styleName(style) + "withdate";
        int newStyleNum = fStyle->copyStyle(fStyle->styleName(style), newStyle);
        fStyle->addNumFmtId(newStyle, NUMFMID_DATE);
        style = newStyleNum;
        break;
    }
    case QVariant::DateTime: {
        /*
        cellValue = QDate::fromString("1900-01-01", "yyyy-MM-dd").daysTo(cellValue.toDate()) + 1 + (QTime::fromString("00:00:00").secsTo(cellValue.toDateTime().time()) * 0.00001574074074074073);
        QString newStyle = fStyle->styleName(style) + "withdatetime";
        int newStyleNum = fStyle->copyStyle(fStyle->styleName(style), newStyle);
        fStyle->addNumFmtId(newStyle, NUMFMID_DATETIME);
        style = newStyleNum;
        break;
        */
        cellValue = fSharedStrings->addString(cellValue.toDateTime().toString("dd/MM/yyyy HH:mm:ss").replace("/", "&#47;"));
        vt = " t=\"s\"";
        break;
    }
    default:
        break;
    }
    XlsxCell *c = new XlsxCell(row, column, cellValue);
    c->fValueType = vt;
    if (style > 0) {
        c->setStyle(style);
    }
    fCells[row][column] = c;
    return c;
}

void XlsxSheet::setColumnWidth(int column, int width)
{
    fColumnWidths[column] = width;
}

void XlsxSheet::setSpan(const QString &span)
{
    fSpan.append(span);
}

void XlsxSheet::setSpan(const QString &f, const QString &s, int row)
{
    fSpan.append(QString("%1%3:%2%3").arg(f, s, QString::number(row)));
}

void XlsxSheet::setSpan(int r1, int c1, int r2, int c2)
{
    fSpan.append(QString("%1:%2").arg(XlsxCell::calculateAddress(r1, c1), XlsxCell::calculateAddress(r2, c2)));
}

void XlsxSheet::setupPage(int pagesize, int fittopage, const QString &pageOrientation)
{
    fPageSize = pagesize;
    fFitToPage = fittopage;
    fPageOrientation = pageOrientation;
}

void XlsxSheet::setupMargins(int l, int r, int t, int b, int h, int f)
{
    fMarginLeft = l;
    fMarginRight = r;
    fMarginTop = t;
    fMarginBottom = b;
    fMarginHeader = h;
    fMarginFooter = f;
}
