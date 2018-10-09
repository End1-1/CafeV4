#ifndef DOUBLE_H
#define DOUBLE_H

#include <QString>

class Double
{
    int l;
    int r;

public:
    Double();
    Double(double v);
    Double(float v);
    Double(const QString &v);
    double toDouble();
    QString toString();
};

#endif // DOUBLE_H
