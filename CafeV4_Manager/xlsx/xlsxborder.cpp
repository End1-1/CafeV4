#include "xlsxborder.h"

XlsxBorder::XlsxBorder()
{
    fLeft = border_weight_thin;
    fRight = border_weight_thin;
    fTop = border_weight_thin;
    fBottom = border_weight_thin;
    fDiagonal = border_weight_thin;
    fLeftColor = "ff000000";
    fRightColor = "ff000000";
    fTopColor = "ff000000";
    fBottomColor = "ff000000";
    fDiagonalColor = "ff000000";
}

QString XlsxBorder::toString() const
{
    QString b("<border>");
    if (fLeft.isEmpty()) {
        b += "<left/>";
    } else {
        b += QString("<left style=\"%1\">").arg(fLeft);
        b += QString("<color rgb=\"%1\"/>").arg(fLeftColor);
        b += "</left>";
    }
    if (fRight.isEmpty()) {
        b += "<right/>";
    } else {
        b += QString("<right style=\"%1\">").arg(fRight);
        b += QString("<color rgb=\"%1\"/>").arg(fRightColor);
        b += "</right>";
    }
    if (fTop.isEmpty()) {
        b += "<top/>";
    } else {
        b += QString("<top style=\"%1\">").arg(fTop);
        b += QString("<color rgb=\"%1\"/>").arg(fTopColor);
        b += "</top>";
    }
    if (fBottom.isEmpty()) {
        b += "<bottom/>";
    } else {
        b += QString("<bottom style=\"%1\">").arg(fBottom);
        b += QString("<color rgb=\"%1\"/>").arg(fBottomColor);
        b += "</bottom>";
    }
    if (fDiagonal.isEmpty()) {
        b += "<diagonal/>";
    } else {
        b += QString("<diagonal style=\"%1\">").arg(fDiagonal);
        b += QString("<color rgb=\"%1\"/>").arg(fDiagonalColor);
        b += "</diagonal>";
    }
    b += "</border>";
    return b;
}
