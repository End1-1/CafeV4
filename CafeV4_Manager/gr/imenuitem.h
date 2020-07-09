#ifndef IMENUITEM_H
#define IMENUITEM_H

#include <QWidget>

namespace Ui {
class IMenuItem;
}

class IMenuItem : public QWidget
{
    Q_OBJECT

public:
    explicit IMenuItem(QWidget *parent = nullptr);
    ~IMenuItem();

private:
    Ui::IMenuItem *ui;
};

#endif // IMENUITEM_H
