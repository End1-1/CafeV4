#ifndef GRIDEDITORDIALOG_H
#define GRIDEDITORDIALOG_H

#include <QDialog>

namespace Ui {
class GridEditorDialog;
}

class GridEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GridEditorDialog(QWidget *parent = nullptr);
    ~GridEditorDialog();

private:
    Ui::GridEditorDialog *ui;
};

#endif // GRIDEDITORDIALOG_H
