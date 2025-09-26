#include "xlsxwriter.h"
#include "crs32.h"

#include <QString>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>

const uint32_t lf_signature = 0x04034b50;
const uint32_t cdfh_signature = 0x02014b50;
const uint32_t eocd_signature = 0x06054b50;

XlsxWriter::XlsxWriter(const QString &fileName)
{
    fFhOffset = 0;
    fFirstOffsetCDFH = 0;
    fLastOffsetCDFH = 0;
    fFile = new QFile(fileName);
    if (!fFile->open(QIODevice::WriteOnly)) {
        fError = fFile->errorString();
        return;
    }
}

XlsxWriter::~XlsxWriter()
{
    delete fFile;
}

void XlsxWriter::writeData(const void *data, uint32_t size)
{
    fFile->write(reinterpret_cast<const char *>(data), size);
}

void XlsxWriter::writeData(const QString &fileName, const QString &data)
{
    writeLocalHeader(fileName, reinterpret_cast<const uint8_t *>(data.toUtf8().data()), data.toUtf8().length());
}

void XlsxWriter::writeEOD()
{
    writeCDFH();
    writeEOCD();
    fFile->close();
}

void XlsxWriter::writeLocalHeader(const QString &fileName, const void *data, uint32_t size)
{
    LocalFileHeader fh;
    memset(&fh, 0, sizeof(fh));
    fFileNames << fileName;

    fh.crc32 = crc32(reinterpret_cast<const uint8_t *>(data), size);
    fh.versionToExtract = 10;
    fh.compressedSize = size;
    fh.uncompressedSize = size;
    fh.filenameLength = fileName.toUtf8().length();

    writeData(&lf_signature, sizeof(lf_signature));
    writeData(&fh, sizeof(fh));
    writeData(fileName.toUtf8().data(), fileName.toUtf8().length());
    writeData(data, size);

    FileInfo fileInfo;
    fileInfo.compressedSize = fh.compressedSize;
    fileInfo.uncompressedSize = fh.uncompressedSize;
    fileInfo.compressionMethod = fh.compressionMethod;
    fileInfo.crc32 = fh.crc32;
    fileInfo.offset = fFhOffset;
    fFileInfoList << fileInfo;

    fFhOffset = fFile->pos();
}

void XlsxWriter::writeCDFH()
{
    fFirstOffsetCDFH = fFile->pos();
    for (int i = 0; i < fFileInfoList.count(); i++) {
        FileInfo &fi = fFileInfoList[i];
        CentralDirectoryFileHeader cdfh;
        memset(&cdfh, 0, sizeof(cdfh));
        cdfh.versionMadeBy = 0;
        cdfh.versionToExtract = 10;
        cdfh.compressedSize = fi.compressedSize;
        cdfh.uncompressedSize = fi.uncompressedSize;
        cdfh.compressionMethod = fi.compressionMethod;
        cdfh.crc32 = fi.crc32;
        cdfh.localFileHeaderOffset = fi.offset;
        cdfh.filenameLength = fFileNames[i].toUtf8().length();
        fFile->write(reinterpret_cast<const char *>(&cdfh_signature), sizeof(cdfh_signature));
        fFile->write(reinterpret_cast<const char *>(&cdfh), sizeof(cdfh));
        fFile->write(fFileNames[i].toUtf8().data(), cdfh.filenameLength);
    }
    fLastOffsetCDFH = fFile->pos() ;
}

void XlsxWriter::writeEOCD()
{
    EOCD eocd;
    memset(&eocd, 0, sizeof(eocd));
    eocd.centralDirectoryOffset = fFirstOffsetCDFH;
    eocd.numberCentralDirectoryRecord = fFileNames.size();
    eocd.totalCentralDirectoryRecord = fFileNames.size();
    eocd.sizeOfCentralDirectory = fLastOffsetCDFH - fFirstOffsetCDFH;

    fFile->write(reinterpret_cast<const char*>(&eocd_signature), sizeof(eocd_signature));
    fFile->write(reinterpret_cast<const char*>(&eocd), sizeof(eocd));
}
