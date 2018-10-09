#ifndef DLGPROGRESS_H
#define DLGPROGRESS_H

#include <QDialog>
#include <QTimer>
#include <QTime>

namespace Ui {
class DlgProgress;
}

class DlgProgress : public QDialog
{
    Q_OBJECT
    
public:
    explicit DlgProgress(QWidget *parent = 0);
    ~DlgProgress();
    void show(const QString &text = tr("Running query"));
    
private:
    Ui::DlgProgress *ui;
    QTimer *m_timer;
    QTime m_time;

private slots:
    void timer();
};

#endif // DLGPROGRESS_H
