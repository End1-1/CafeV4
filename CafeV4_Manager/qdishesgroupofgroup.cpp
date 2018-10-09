#include "qdishesgroupofgroup.h"
#include "dlgdishgroupofgroup.h"
#include "qdlgcache.h"
#include "dlgviewimage.h"

QDishesGroupOfGroup::QDishesGroupOfGroup(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "ME_GROUP_TYPES";
    m_actions << "actionNew" << "actionEdit" << "actionDelete"
              << "actionRefresh" << "actionExport_to_Excel" << "actionMenu_2" << "actionImage";
    setWindowTitle(tr("Groups of dishes groups"));
    addField("ID", tr("Id"), 0, QVariant::Int);
    addField("MENU_ID", tr("Menu id"), 0, QVariant::Int);
    addField("MENU_NAME", tr("Menu"), 300, QVariant::String);
    addField("NAME", tr("Name"), 300, QVariant::String);
    addField("IMG_LINK", tr("Path to image"), 100, QVariant::String);
    actionRefresh();
}

void QDishesGroupOfGroup::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgDishGroupOfGroup>();
}

void QDishesGroupOfGroup::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgDishGroupOfGroup>(false);
}

void QDishesGroupOfGroup::actionDelete()
{
    CHECK_VIEWER_AND_MAINDB
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (!confirmDeletion())
        return;

    m_sqlDriver->prepare("DELETE from me_group_types where id=:id");
    m_sqlDriver->bind(":id", cellValue(l.toList().at(0), 0));
    m_sqlDriver->execSQL();

    m_sqlDriver->prepare("delete from me_group_types_members where group_id=:group_id");
    m_sqlDriver->bind(":group_id", cellValue(l.toList().at(0), 0));
    m_sqlDriver->execSQL();

    m_sqlDriver->close();
}

void QDishesGroupOfGroup::actionCostum(int action)
{
    CHECK_VIEWER_AND_MAINDB
    switch(action) {
    case ACTION_MENU:
        editMembersList();
        break;
    case ACTION_IMG:
        editImage();
        break;
    }
}

void QDishesGroupOfGroup::actionBeforeSelect()
{
    m_sqlString = "select mg.id, mg.menu_id, mm.name as menu_name, mg.name, mg.img_link "
            "from me_group_types mg, me_menus mm "
            "where mg.menu_id=mm.id "
            "order by mm.name, mg.name";
    actionSelect();
}

void QDishesGroupOfGroup::editMembersList()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    QString selection;
    bool first = true;

    m_sqlDriver->prepare("select mt.name "
                         "from me_group_types_members mgt, me_types mt "
                         "where mgt.member_id=mt.id and mgt.group_id=:group_id");
    m_sqlDriver->bind(":group_id", cellValue(l.toList().at(0), 0));
    m_sqlDriver->execSQL();
    while (m_sqlDriver->next()) {
        if (first)
            first = false;
        else
            selection += ",";
        selection += m_sqlDriver->val().toString();
    }
    m_sqlDriver->close();

    QDlgCache *d = new QDlgCache("ME_TYPES", selection, this);
    if (d->exec() == QDialog::Accepted) {
        QString names, values;
        d->getValues(names, values);
        QStringList lst = values.split(",");

        m_sqlDriver->prepare("delete from me_group_types_members where group_id=:group_id");
        m_sqlDriver->bind(":group_id", cellValue(l.toList().at(0), 0));
        m_sqlDriver->execSQL();

        for (QStringList::const_iterator i = lst.begin(); i != lst.end(); i++) {
            m_sqlDriver->prepare("insert into me_group_types_members (group_id, member_id) values (:group_id, :member_id)");
            m_sqlDriver->bind(":group_id", cellValue(l.toList().at(0), 0));
            m_sqlDriver->bind(":member_id", *i);
            m_sqlDriver->execSQL();
        }

        m_sqlDriver->close();
    }
    delete d;
}

void QDishesGroupOfGroup::editImage()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }
    int id = cellValue(l.toList().at(0), fieldIndexInGrid("ID")).toInt();
    QString link = cellValue(l.toList().at(0), fieldIndexInGrid("IMG_LINK")).toString();
    DlgViewImage *d = new DlgViewImage(id, link, 1, this);
    d->exec();
    setCellValue(l.toList().at(0), fieldIndexInGrid("IMG_LINK"), d->m_imgLink);
    delete d;
}
