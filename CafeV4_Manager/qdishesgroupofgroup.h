#ifndef QDISHESGROUPOFGROUP_H
#define QDISHESGROUPOFGROUP_H

#include "qgrid.h"

class QDishesGroupOfGroup : public QGrid
{
    Q_OBJECT
public:
    explicit QDishesGroupOfGroup(QWidget *parent = 0);
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionCostum(int action);
    virtual void actionBeforeSelect();

private:
    void editMembersList();
    void editImage();

};

#endif // QDISHESGROUPOFGROUP_H
