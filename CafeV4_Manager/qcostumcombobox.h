#ifndef QCOSTUMCOMBOBOX_H
#define QCOSTUMCOMBOBOX_H

#include <QComboBox>
#include <QKeyEvent>

class QCostumComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit QCostumComboBox(QWidget *parent = 0);

protected:
    virtual void keyPressEvent(QKeyEvent *e);

signals:
    void keyEnterPressed();

public slots:

};

#endif // QCOSTUMCOMBOBOX_H
