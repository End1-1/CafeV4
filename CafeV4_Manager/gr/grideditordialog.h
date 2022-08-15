#ifndef GRIDEDITORDIALOG_H
#define GRIDEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class GridEditorDialog;
}

class GridEditorWidget;

class GridEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridEditorDialog(GridEditorWidget *ge, QWidget *parent = nullptr);
    ~GridEditorDialog();
    bool mFlagNew;
    virtual void closeEvent(QCloseEvent *e) override;

private slots:
    void on_btnCancel_clicked();
    void on_btnSaveAndNew_clicked();
    void on_btnSave_clicked();

private:
    Ui::GridEditorDialog *ui;
    GridEditorWidget *mWidget;
};

#endif // GRIDEDITORDIALOG_H
