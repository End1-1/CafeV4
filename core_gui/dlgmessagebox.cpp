#include "dlgmessagebox.h"
#include "ui_dlgmessagebox.h"

DlgMessageBox::DlgMessageBox(const QString &message, const int &level, QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
    ui(new Ui::DlgMessageBox)
{
    ui->setupUi(this);
    QFont font(qApp->font());
    font.setPointSize(20);
    ui->lbText->setFont(font);

    QString label = "<div style=\"vertical-align: middle;\">";
    switch (level) {
    case 0:
        label = "<img style=\"margin-top: 0px; margin-right:25px;\" src=\":/new/prefix1/res/icons/messagebox_information.png\"/>";;
        break;
    case 1:
        label = "<img style=\"margin-top: 0px; margin-right:25px;\" src=\":/new/prefix1/res/icons/messagebox_warning.png\"/>";;
        break;
    }

    label += message + "</div>";
    ui->lbText->setText(label);
}

DlgMessageBox::~DlgMessageBox()
{
    delete ui;
}

int DlgMessageBox::show(const QString &message, const int &level, QWidget *parent)
{
    DlgMessageBox *messageBox = new DlgMessageBox(message, level, parent);
    int result = messageBox->exec();
    delete messageBox;
    return result;
}

void DlgMessageBox::on_pushButton_clicked()
{
    reject();
}

void DlgMessageBox::on_pushButton_2_clicked()
{
    accept();
}
