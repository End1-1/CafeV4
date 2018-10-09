#include "rep.h"
#include <QWidget>

QString actionName() {
    return QObject::tr("Goods movement withour amounts");
}

int call(QWidget *parent) {
    QWidget *w = new QWidget(parent);
    w->resize(100, 100);
    w->show();
    return 0;
}
