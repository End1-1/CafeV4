#ifndef XLSXSHAREDSTRING_H
#define XLSXSHAREDSTRING_H

#include "xlsx.h"

class XlsxSharedString : public Xlsx
{
public:
    XlsxSharedString();
    int addString(const QString &string);

protected:
    virtual void buildExcelData();

private:
    QStringList fStrings;
};

#endif // XLSXSHAREDSTRING_H
