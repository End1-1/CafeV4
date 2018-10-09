#ifndef DLGTABLES_H
#define DLGTABLES_H

#include "core.h"

namespace Ui {
class DlgTables;
}

class DlgTables : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgTables(QWidget *parent = 0);
    ~DlgTables();
    static bool getTableId(QWidget *parent, Hall2::Table &table);
    
private slots:
    void on_btnCancel_clicked();
    void on_tblTable_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);
    void on_btnOk_clicked();

    void on_tblTable_clicked(const QModelIndex &index);

private:
    Ui::DlgTables *ui;
};

#endif // DLGTABLES_H
