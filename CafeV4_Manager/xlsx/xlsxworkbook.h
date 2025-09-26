#ifndef XLSXWORKBOOK_H
#define XLSXWORKBOOK_H

#include "xlsx.h"

class XlsxSheet;
class XlsxSharedString;
class XlsxDocument;

class XlsxWorkBook : public Xlsx
{
public:
    XlsxWorkBook();
    virtual void writeToFile(XlsxWriter *xw);
    virtual void buildExcelData();
    XlsxSheet *addSheet(const QString &name);
    XlsxSheet *sheet(int index);
    int sheetsCount();
    XlsxDocument *fDocument;

private:
    QMap<QString, XlsxSheet *> fSheets;
    QStringList fSheetsOrder;
    XlsxSharedString *fSharedStrings;
};

#endif // XLSXWORKBOOK_H
