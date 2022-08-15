#include "grideditordialog.h"
#include "ui_grideditordialog.h"
#include "grideditorwidget.h"
#include <QCloseEvent>

GridEditorDialog::GridEditorDialog(GridEditorWidget *ge, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GridEditorDialog)
{
    ui->setupUi(this);
    mWidget = ge;
    setWindowTitle(mWidget->title());
    mWidget->prepare();
    mWidget->clearWidgets();
    mFlagNew = false;
    ui->vl->addWidget(mWidget);
    adjustSize();
}

GridEditorDialog::~GridEditorDialog()
{
    delete ui;
}

void GridEditorDialog::closeEvent(QCloseEvent *e)
{
    QDialog::closeEvent(e);
}

void GridEditorDialog::on_btnCancel_clicked()
{
    reject();
}

void GridEditorDialog::on_btnSaveAndNew_clicked()
{
    mWidget->save();
    mFlagNew = true;
//    accept();
}

void GridEditorDialog::on_btnSave_clicked()
{
    mWidget->save();
    mFlagNew = false;
//    accept();
}
