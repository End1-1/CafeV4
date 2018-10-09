#ifndef GDSETTINGS_H
#define GDSETTINGS_H

#include "qdlgquery.h"

namespace Ui {
class GDSettings;
}

class GDSettings : public QDlgQuery
{
    Q_OBJECT

public:
    explicit GDSettings(QMap<QString, QList<QVariant> > &values, QWidget *parent = 0);
    ~GDSettings();

private:
    Ui::GDSettings *ui;
};

#endif // GDSETTINGS_H
