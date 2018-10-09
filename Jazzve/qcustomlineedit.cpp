#include "qcustomlineedit.h"

QCustomLineEdit::QCustomLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void QCustomLineEdit::focusOutEvent(QFocusEvent *e)
{
    QLineEdit::focusOutEvent(e);
    emit focusOut();
}
