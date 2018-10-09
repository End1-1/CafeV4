#ifndef DLGFILTERLIST_H
#define DLGFILTERLIST_H

#include "core.h"

namespace Ui {
class DlgFilterList;
}

class DlgFilterList : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgFilterList(const QStringList &names, QWidget *parent = 0);
    ~DlgFilterList();
    static QList<int> &getCheckedIndexes(QList<int> &currentList, const QStringList &names, QWidget *parent = 0);
    void checkedIndexes(QList<int> &list);
    void setSingleSelection();
    
private slots:
    void on_btnCancel_clicked();
    void on_btnOk_clicked();
    void on_btnClear_clicked();
    void on_lstNames_clicked(const QModelIndex &index);

private:
    Ui::DlgFilterList *ui;
    bool singleSelection;
};

#endif // DLGFILTERLIST_H
