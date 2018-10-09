#include "qdishestypes.h"
#include "dlgdishtype.h"
#include "dlgviewimage.h"

QDishesTypes::QDishesTypes(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "ME_TYPES";
    m_actions << "actionNew" << "actionEdit" << "actionRefresh" << "actionExport_to_Excel" << "actionImage" << "actionSearch";
    setWindowTitle(tr("Dishes types"));

    addField("ID", tr("Id"), 100, QVariant::Int);
    addField("NAME", tr("Name"), 300, QVariant::String);
    addField("ADGCODE", tr("ADG code"), 100, QVariant::String);
    addField("IMG_LINK", tr("Image link"), 50, QVariant::String);

    actionBeforeSelect();
}

void QDishesTypes::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgDishType>();
}

void QDishesTypes::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgDishType>(false);
}

void QDishesTypes::actionBeforeSelect()
{
    /* m_sqlString = "select id, name, img_link from me_types "
            "where id in (select distinct(type_id) from me_dishes where id in (select distinct(dish_id) from me_dishes_menu where state_id=1)) "
            "order by name";
            */
    m_sqlString = "select id, name, adgcode, img_link from me_types order by name";
    actionSelect();
}

void QDishesTypes::actionCostum(int action)
{
    CHECK_VIEWER_AND_MAINDB
    switch (action) {
    case ACTION_IMG:
        editImage();
        break;
    }
}

void QDishesTypes::editImage()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }
    int id = cellValue(l.toList().at(0), fieldIndexInGrid("ID")).toInt();
    QString link = cellValue(l.toList().at(0), fieldIndexInGrid("IMG_LINK")).toString();
    DlgViewImage *d = new DlgViewImage(id, link, 2, this);
    if (d->exec() == QDialog::Accepted) {
        setCellValue(l.toList().at(0), fieldIndexInGrid("IMG_LINK"), d->m_imgLink);
        m_sqlDriver->openDB();
        m_sqlDriver->prepare("update me_types set img_link=:img_link where id=:id");
        m_sqlDriver->bind(":img_link", d->m_imgLink);
        m_sqlDriver->bind(":id", cellValue(l.toList().at(0), fieldIndexInGrid("ID")).toInt());
        m_sqlDriver->execSQL();
        m_sqlDriver->close();
    }
    delete d;
}
