#ifndef XLSXSHEET_H
#define XLSXSHEET_H

#include "xlsx.h"
#include <QMap>
#include <QVariant>

#define xls_page_size_a4 9
#define xls_page_orientation_landscape "landscape"
#define xls_page_orientation_portrait "portrait"

class XlsxCell;
class XlsxSharedString;
class XlsxStyles;

class XlsxSheet : public Xlsx
{
public:
    XlsxSheet(const QString &name);
    virtual void buildExcelData();
    XlsxSharedString *fSharedStrings;
    XlsxStyles *fStyle;
    QString &name();
    XlsxCell *addCell(int row, int column, QVariant cellValue, int style = 0);
    void setColumnWidth(int column, int width);
    void setSpan(const QString &span);
    void setSpan(const QString &f, const QString &s, int row);
    void setSpan(int r1, int c1, int r2, int c2);
    void setupPage(int pagesize, int fittopage, const QString &pageOrientation);
    void setupMargins(int l, int r, int t, int b, int h, int f);

private:
    QMap<int, QMap<int, XlsxCell *> > fCells;
    QMap<int, int> fColumnWidths;
    QStringList fSpan;
    QString fName;
    int fFitToPage;
    int fPageSize;
    QString fPageOrientation;
    int fMarginLeft;
    int fMarginRight;
    int fMarginTop;
    int fMarginBottom;
    int fMarginHeader;
    int fMarginFooter;
};

#endif // XLSXSHEET_H
