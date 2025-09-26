#include "xlsxcell.h"

static const char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

XlsxCell::XlsxCell(int row, int column, const QVariant &cellValue)
{
    fRow = row;
    fColumn = column;
    fCellValue = cellValue;
    fAddress = calculateAddress(row, column);
    fStyle = 0;
}

QVariant &XlsxCell::value()
{
    return fCellValue;
}

const QString &XlsxCell::address()
{
    return fAddress;
}

QString XlsxCell::calculateAddress(int row, int col)
{
    QString a;
    do  {
        int m = (col) / 27;
        if (m == 0) {
            m = col % 27;
        }
        col -= 26;
        a += alphabet[m - 1];
    } while (col > 0);
    return a + QString::number(row);
}

int XlsxCell::style()
{
    return fStyle;
}

void XlsxCell::setStyle(int style)
{
    fStyle = style;
}
