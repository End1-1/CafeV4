#ifndef XLSXBORDER_H
#define XLSXBORDER_H

#include <QString>

#define border_weight_thin "thin"
#define border_weight_medium "medium"
#define border_weight_thick "thick"

class XlsxBorder
{
public:
    XlsxBorder();
    QString toString() const;

private:
    QString fLeft;
    QString fRight;
    QString fTop;
    QString fBottom;
    QString fDiagonal;

    QString fLeftColor;
    QString fRightColor;
    QString fTopColor;
    QString fBottomColor;
    QString fDiagonalColor;

};

#endif // XLSXBORDER_H
