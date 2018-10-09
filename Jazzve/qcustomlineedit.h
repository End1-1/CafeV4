#ifndef QCUSTOMLINEEDIT_H
#define QCUSTOMLINEEDIT_H

#include <QLineEdit>

class QCustomLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit QCustomLineEdit(QWidget *parent = 0);

protected:
    virtual void focusOutEvent(QFocusEvent *e);

signals:
    void focusOut();

public slots:

};

#endif // QCUSTOMLINEEDIT_H
