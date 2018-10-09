#ifndef MAGICMENU_H
#define MAGICMENU_H

#include <QDialog>
#include "qsqldrv.h"

namespace Ui {
class MagicMenu;
}

class MagicMenu : public QDialog
{
    Q_OBJECT

public:
    explicit MagicMenu(QWidget *parent = 0);
    ~MagicMenu();
    QMap<QString, QVariant> values();

private slots:
    void on_pushButton_clicked();
    void on_Cancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::MagicMenu *ui;
    QSqlDrv *m_sqlDrv;
};

#endif // MAGICMENU_H
