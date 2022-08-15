#ifndef GRSELECTOR_H
#define GRSELECTOR_H

#include <QDialog>

namespace Ui {
class GrSelector;
}

class GrSelectorModel;

class GrSelector : public QDialog
{
    Q_OBJECT

public:
    explicit GrSelector(QWidget *parent = nullptr);
    ~GrSelector();
    static bool get(GrSelectorModel *sm);

private:
    Ui::GrSelector *ui;
    GrSelectorModel *mSelectorModel;

private slots:
    void sqlFinished();
};

#endif // GRSELECTOR_H
