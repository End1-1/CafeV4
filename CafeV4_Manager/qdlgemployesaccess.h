#ifndef QDLGEMPLOYESACCESS_H
#define QDLGEMPLOYESACCESS_H

#include "qbasesqlwindow.h"

namespace Ui {
class QDlgEmployesAccess;
}

class QDlgEmployesAccess : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit QDlgEmployesAccess(const QString &groupId, QWidget *parent = 0);
    ~QDlgEmployesAccess();
    virtual void actionSave();
    virtual void actionCostum(int action);

private:
    Ui::QDlgEmployesAccess *ui;
    QString m_groupId;
    void checkForNewRole();
    void initNewOldRoles(QStringList roles, int module_id, const QString &groupId);
    bool load();
    void addRole(QTableWidget *t, const QString &key, const QString &val);
    void saveRole(QTableWidget *t, int module);
};

#endif // QDLGEMPLOYESACCESS_H
