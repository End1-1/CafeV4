#ifndef XLSXDOCUMENT_H
#define XLSXDOCUMENT_H

#include <QWidget>

class XlsxContentType;
class XlsxDocPropsCore;
class XlsxDocPropsApp;
class XlsxRels;
class XlsxWorkBook;
class XlsxStyles;
class XlsxTheme;

class XlsxDocument
{
public:
    XlsxDocument();
    ~XlsxDocument();
    XlsxWorkBook *addWorkbook();
    XlsxWorkBook *workbook();
    XlsxStyles *style();
    bool save(QString &err, bool open);
    bool save(const QString &fileName, QString &err);

private:
    XlsxContentType *fContentTypes;
    XlsxDocPropsCore *fDocPropsCore;
    XlsxDocPropsApp *fDocPropsApp;
    XlsxRels *fRels;
    XlsxWorkBook *fWorkBook;
    XlsxStyles *fStyles;
    XlsxTheme *fTheme;
};

#endif // XLSXDOCUMENT_H
