#ifndef XLSXWRITER_H
#define XLSXWRITER_H

#include "zip.h"
#include <QtGlobal>
#include <QStringList>

class QFile;

class XlsxWriter
{
public:
    XlsxWriter(const QString &fileName);
    ~XlsxWriter();
    QString fError;

    void writeLocalHeader(const QString &fileName, const void *data, uint32_t size);
    void writeData(const QString &fileName, const QString &data);
    void writeEOD();

private:
    QFile *fFile;
    QStringList fFileNames;
    QList<FileInfo> fFileInfoList;
    uint32_t fFhOffset;
    uint32_t fFirstOffsetCDFH;
    uint32_t fLastOffsetCDFH;

    void writeData(const void *data, uint32_t size);
    void writeCDFH();
    void writeEOCD();
};

#endif // XLSXWRITER_H
