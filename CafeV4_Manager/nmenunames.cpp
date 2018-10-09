#include "nmenunames.h"
#include "cgridwidget.h"
#include "qbasesqlwindow.h"

NMenuNames::NMenuNames(QWidget *parent) :
    MdiWindow(parent)
{
    showMaximized();
    fMdiButton = new MdiButton(this, tr("Menu names"));
    QBaseSqlWindow::m_mdiArea->addSubWindow(this);
    setToolbar(QBaseSqlWindow::m_toolBarBtn);
    fGrid = new CGridWidget("6cd21be2-1ccf-4762-a4fe-8b6a1ebf5f55");
    setCentralWidget(fGrid);
    fGrid->setColumnName("ID", tr("Code"), 80)
            .setColumnName("NAME", tr("Name"), 400);
    fGrid->setQuery("SELECT ID,NAME FROM ME_MENUS");
}
