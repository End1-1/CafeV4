#ifndef GSALERREPORTCOMMON_H
#define GSALERREPORTCOMMON_H

#include "qgrid.h"

class GSalerReportCommon : public QGrid
{
    Q_OBJECT
public:
    explicit GSalerReportCommon(QWidget *parent = 0);
    virtual void actionBeforeSelect();

signals:

public slots:

};

#endif // GSALERREPORTCOMMON_H
