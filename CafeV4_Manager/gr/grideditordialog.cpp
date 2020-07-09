#include "grideditordialog.h"
#include "ui_grideditordialog.h"

GridEditorDialog::GridEditorDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridEditorDialog)
{
    ui->setupUi(this);
}

GridEditorDialog::~GridEditorDialog()
{
    delete ui;
}
