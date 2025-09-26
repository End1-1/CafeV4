#ifndef XLSXCONTENTTYPE_H
#define XLSXCONTENTTYPE_H

#include "xlsx.h"

class XlsxDocument;

class XlsxContentType : public Xlsx
{
public:
    XlsxContentType(XlsxDocument *document);
    virtual void buildExcelData();

private:
    XlsxDocument *fDocument;
};

#endif // XLSXCONTENTTYPE_H
