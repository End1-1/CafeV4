#ifndef QEDITWSETTINGS_H
#define QEDITWSETTINGS_H

#include "qbasesqlwindow.h"

namespace Ui {
class QEditWSettings;
}

class QEditWSettings : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit QEditWSettings(QWidget *parent = 0);
    ~QEditWSettings();
    void setId(const QString &id);
    virtual void actionCostum(int action);
    virtual void actionSave();

private:
    Ui::QEditWSettings *ui;
    QString m_id;
    void copy();
    void paste();
};

#endif // QEDITWSETTINGS_H
