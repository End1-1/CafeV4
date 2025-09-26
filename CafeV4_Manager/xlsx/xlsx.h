#ifndef XLSX_H
#define XLSX_H

#include <QString>
#include <QMap>

class XlsxWriter;

class Xlsx {
public:
    Xlsx();
    virtual ~Xlsx();
    virtual void writeToFile(XlsxWriter *xw);

protected:
    QString fZipFileName;
    QString fExcelData;
    virtual void buildExcelData();
};

#endif // XLSX_H
