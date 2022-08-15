#include "gdiscountcard.h"
#include "dlgdiscountcard.h"
#include "dlgcardregister.h"

GDiscountCard::GDiscountCard(QWidget *parent) :
    QGrid(parent)
{
    setWindowTitle(tr("Discount card"));
    m_actions << "actionNew" << "actionEdit" << "actionExport_to_Excel" << "actionRefresh" << "actionFilter"
              << "actionMagic" << "actionSearch";
    m_editDlgTable = "costumers_names";
    SqlField *fId = addField("ID", tr("Id"), 100, QVariant::Int);
    fId->addFilter(Filter::ftEdit, false, "");
    SqlField *fState = addField("STATE_ID", tr("State id"), 0, QVariant::Int);
    fState->addFilter(Filter::ftEdit, true, "COSTUMERS_STATES");
    addField("STATE_NAME", tr("State"), 100, QVariant::String);
    addField("NAME", tr("Costumer name"), 200, QVariant::String);
    addField("CARD_VALUE", tr("Discount"), 100, QVariant::String);
    addField("INFO", tr("Info"), 300, QVariant::String);
    addField("CODE", tr("Card code"), 150, QVariant::String);
    actionBeforeSelect();
}

void GDiscountCard::actionBeforeSelect()
{
    m_sqlString = "select cn.id, cn.state_id, cs.name as state_name, cn.name, "
            "cn.card_value, cn.info, cn.code "
            "from costumers_names cn, costumers_states cs ";
    QString where = "where cn.state_id=cs.id and";

    SqlField *f = fieldByName("ID");
    f->haveFilter(where, "cn.ID");
    f = fieldByName("STATE_ID");
    f->haveFilter(where, "cn.state_id");

    remove_and(where);
    m_sqlString += where;
    actionSelect();
}

void GDiscountCard::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgDiscountCard>();
}

void GDiscountCard::actionEdit()
{
    CHECK_VIEWER_AND_MAINDB
    createEditorDlg<DlgDiscountCard>(false);
}

void GDiscountCard::actionCostum(int a)
{
    switch (a) {
    case ACTION_MAGIC:
        DlgCardRegister *dc = new DlgCardRegister(this);
        dc->exec();
        delete dc;
        break;
    }
}
