#ifndef GMENU_H
#define GMENU_H

#include "qgrid.h"
#include "qplineedit.h"

class GMenu : public QGrid
{
    Q_OBJECT
public:
    explicit GMenu(QWidget *parent = 0);
    virtual void actionBeforeSelect();
    virtual void actionNew();
    virtual void actionEdit();
    virtual void actionDelete();
    virtual void actionMultiSelect(QToolBar *tb);
    virtual void actionCostum(int action);

private:

    class ID : public QItemDelegate {
    private:
        GMenu *m_gmenu;
    public:
        ID(GMenu *gmenu);
    protected:
        virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };

    bool m_rowDeleted;
    QPLineEdit *m_lePrice;
    void editMagic();
    void editRecipe();
    void editImage();

private slots:
    void gridCellChanged(int new_row, int new_col, int previous_row, int previous_col);
    void gridPriceChanged();
    virtual void on_grid_doubleClicked(const QModelIndex &index);

};

#endif // GMENU_H
