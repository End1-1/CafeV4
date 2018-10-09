#ifndef MDIBUTTON_H
#define MDIBUTTON_H

#include <QWidget>

namespace Ui {
class MdiButton;
}

class MdiButton : public QWidget
{
    Q_OBJECT
    
public:
    explicit MdiButton(QWidget *parent, const QString &text);
    ~MdiButton();
    void setText(const QString &text);
    void setIcon(const QIcon &icon);
    
private slots:
    void widgetDestroyed(QObject *object = 0);
    void on_btnClose_clicked();

public slots:
    void on_btnClick_clicked();

private:
    Ui::MdiButton *ui;
    QWidget *m_widget;
};

#endif // MDIBUTTON_H
