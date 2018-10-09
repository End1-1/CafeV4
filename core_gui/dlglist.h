#ifndef DLGLIST_H
#define DLGLIST_H

#include <QDialog>

namespace Ui {
class DlgList;
}

class DlgList : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgList(QWidget *parent = 0);
    ~DlgList();
    void setMenu(const QStringList &menu);
    QString selectedItem();
    
private slots:
    void on_btnCancel_clicked();
    void on_btnOk_clicked();

private:
    Ui::DlgList *ui;
};

#endif // DLGLIST_H
