#ifndef DLGVALUEPAIR_H
#define DLGVALUEPAIR_H

#include <QDialog>

namespace Ui {
class DlgValuePair;
}

class DlgValuePair : public QDialog
{
    Q_OBJECT

public:
    explicit DlgValuePair(QWidget *parent = 0);
    ~DlgValuePair();
    static bool getValuePair(QString &v1, QString &v2, QWidget *parent = 0);

private slots:

private:
    Ui::DlgValuePair *ui;
};

#endif // DLGVALUEPAIR_H
