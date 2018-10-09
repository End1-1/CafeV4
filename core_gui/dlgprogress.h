#ifndef DLGPROGRESS_H
#define DLGPROGRESS_H

#include <QDialog>

namespace Ui {
class DlgProgress;
}

class DlgProgress : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgProgress(QWidget *parent = 0);
    ~DlgProgress();
    void show(const QString &message = tr("Running query"));
    
private:
    Ui::DlgProgress *ui;

public slots:
    void slotHide();
};

#endif // DLGPROGRESS_H
