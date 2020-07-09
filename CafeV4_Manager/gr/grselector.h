#ifndef GRSELECTOR_H
#define GRSELECTOR_H

#include <QDialog>

namespace Ui {
class GrSelector;
}

class GrSelector : public QDialog
{
    Q_OBJECT

public:
    explicit GrSelector(QWidget *parent = nullptr);
    ~GrSelector();

private:
    Ui::GrSelector *ui;
};

#endif // GRSELECTOR_H
