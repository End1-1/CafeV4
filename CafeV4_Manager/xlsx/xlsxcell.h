#ifndef XLSXCELL_H
#define XLSXCELL_H

#include <QVariant>
#include <QString>

class XlsxSharedString;

class XlsxCell
{
public:
    XlsxCell(int row, int column, const QVariant &cellValue);
    QVariant &value();
    const QString &address();
    static QString calculateAddress(int row, int col);
    int style();
    void setStyle(int style);
    QString fValueType;

private:
    int fRow;
    int fColumn;
    int fStyle;
    QString fAddress;
    QVariant fCellValue;
};

#endif // XLSXCELL_H
