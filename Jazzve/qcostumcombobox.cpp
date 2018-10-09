#include "qcostumcombobox.h"

QCostumComboBox::QCostumComboBox(QObject *parent) :
    QComboBox(parent)
{
}

void QCostumComboBox::keyPressEvent(QKeyEvent *e)
{
    switch (e->key()) {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        emit keyEnterPressed();
        break;
    default:
        QComboBox::keyPressEvent(e);
        break;
    }
}
