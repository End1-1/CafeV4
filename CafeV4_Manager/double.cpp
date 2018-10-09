#include "double.h"
#include <math.h>

Double::Double()
{
    l = 0;
    r = 0;
}

Double::Double(double v)
{
    l = trunc(v);
}

Double::Double(float v)
{

}

Double::Double(const QString &v)
{
    int p = v.indexOf(".");
    QString ls = v.mid(0, p - 1);
    QString rs = v.mid(p + 1, v.length());
    l = ls.toInt();
    r = rs.toInt();
}

double Double::toDouble()
{
    return 0;
}

QString Double::toString()
{
    return QString("%1").arg(0, 0, 10, QChar('0'));
}
