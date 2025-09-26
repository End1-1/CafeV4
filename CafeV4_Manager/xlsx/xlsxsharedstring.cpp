#include "xlsxsharedstring.h"

XlsxSharedString::XlsxSharedString() :
    Xlsx()
{
    fZipFileName = "xl\\sharedStrings.xml";
}

int XlsxSharedString::addString(const QString &string)
{
    fStrings.append(string);
    return fStrings.count() - 1;
}

void XlsxSharedString::buildExcelData()
{
    fExcelData = QString("<sst xmlns=\"http://schemas.openxmlformats.org/spreadsheetml/2006/main\" count=\"%1\" uniqueCount=\"%1\">").arg(fStrings.count());
    for (int i = 0, count = fStrings.count(); i < count; i++) {
        fExcelData += QString("<si><t>%1</t></si>").arg(fStrings.at(i));
    }
    fExcelData += "</sst>";
}
