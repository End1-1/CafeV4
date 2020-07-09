#ifndef GRIDWIDGET_H
#define GRIDWIDGET_H

#include <QWidget>

namespace Ui {
class GridWidget;
}

class GridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GridWidget(QWidget *parent = nullptr);
    ~GridWidget();

private:
    Ui::GridWidget *ui;
};

#endif // GRIDWIDGET_H
