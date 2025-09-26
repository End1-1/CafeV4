#include "xlsx.h"
#include "xlsxwriter.h"

Xlsx::Xlsx()
{

}

Xlsx::~Xlsx()
{

}

void Xlsx::writeToFile(XlsxWriter *xw)
{
    buildExcelData();
    if (fExcelData.isEmpty()) {
        return;
    }
    xw->writeLocalHeader(fZipFileName, fExcelData.toUtf8().data(), fExcelData.toUtf8().length());
}

void Xlsx::buildExcelData()
{

}
