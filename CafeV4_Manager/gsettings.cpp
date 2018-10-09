#include "gsettings.h"
#include "gdsettings.h"
#include "qeditwsettings.h"

void GSettings::showSalesSettings()
{
    QModelIndexList m = selectedRows();
    if (!m.count()) {
        QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
        return;
    }

    QString id = itemData(m.at(0).row(), 0).toString();
    QEditWSettings *d = new QEditWSettings(id, qobject_cast<QWidget*>(this->parent()));
    d->show();
}

GSettings::GSettings(QWidget *parent) :
    QGrid(parent)
{
    m_actions << "actionNew" << "actionEdit" << "actionRefresh" << "actionDelete"
              << "actionSales_settings";
    m_editDlgTable = "SYS_SETTINGS";

    setWindowTitle(tr("Station settings"));

    SqlField *fId = new SqlField("ID", tr("Id"), 150, 0, QVariant::Int, 0, false);
    m_fields.append(fId);
    SqlField *fName = new SqlField("NAME", tr("Name"), 250, 0, QVariant::String, 0, false);
    m_fields.append(fName);
    actionBeforeSelect();
}

void GSettings::actionBeforeSelect()
{
    m_sqlString = "select id, name from sys_settings";
    actionSelect();
}

void GSettings::actionNew()
{
    createEditorDlg<GDSettings>();
}

void GSettings::actionEdit()
{
    createEditorDlg<GDSettings>(false);
}

void GSettings::actionDelete()
{
   QModelIndexList rows = selectedRows();
   if (!rows.count()) {
       QMessageBox::warning(this, tr("Warning"), tr("No object is selected"));
       return;
   }

   if (!getYesNo(tr("Confirm deletion of selected rows")))
       return;

   QVariant id = itemData(rows.at(0).row(), fieldIndexByName("ID"));
   m_sqlDriver->prepare("delete from sys_settings_values where settings_id=:settings_id");
   m_sqlDriver->bind(":settings_id", id);
   m_sqlDriver->execSQL();
   m_sqlDriver->prepare("delete from sys_settings where id=:id");
   m_sqlDriver->bind(":id", id);
   m_sqlDriver->execSQL();
   m_sqlDriver->close();
   removeRow(rows.at(0).row());

}

void GSettings::actionCostum(int action)
{
    switch (action) {
    case ACTION_SALES_SETTINGS:
        showSalesSettings();
        break;
    }
}
