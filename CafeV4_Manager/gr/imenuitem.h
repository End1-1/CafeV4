#ifndef IMENUITEM_H
#define IMENUITEM_H

#include "grideditorwidget.h"

namespace Ui {
class IMenuItem;
}

class IMenuItem : public GridEditorWidget
{
    Q_OBJECT

public:
    explicit IMenuItem(QWidget *parent = nullptr);
    ~IMenuItem();
    virtual void load(int id);
    virtual void save() override;
    virtual QString title() const;

protected:
    virtual void setTableName() override;

private:
    Ui::IMenuItem *ui;

private slots:
    virtual void sqlFinished() override;
    void menuFinished();
    void recipeFinished();

};

#endif // IMENUITEM_H
