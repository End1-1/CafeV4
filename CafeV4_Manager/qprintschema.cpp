#include "qprintschema.h"
#include "dlgprintschema.h"
#include "qprintersofschema.h"

QPrintSchema::QPrintSchema(QWidget *parent) :
    QGrid(parent)
{
    m_editDlgTable = "ME_PRINTER_SCHEMA";
    m_actions << "actionNew" << "actionEdit" << "actionDelete"
              << "actionRefresh" << "actionExport_to_Excel" << "actionMenu_2";
    setWindowTitle(tr("Schemas of printers"));
    createSimpleIdNameTable("ME_PRINTER_SCHEMA");
}

void QPrintSchema::actionNew()
{
    createEditorDlg<DlgPrintSchema>();
}

void QPrintSchema::actionEdit()
{
    createEditorDlg<DlgPrintSchema>(false);
}

void QPrintSchema::actionDelete()
{
    QSet<int> l ;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    if (QMessageBox::warning(this, tr("Warning"), tr("Confirm deletion"), QMessageBox::Yes | QMessageBox::No) != QMessageBox::Yes)
        return;

    m_sqlDriver->prepare("delete from me_printers_schema where id=:id");
    m_sqlDriver->bind(":id", cellValue(l.toList().at(0), 0));
    m_sqlDriver->execSQL();

    m_sqlDriver->prepare("delete from me_printers where schema_id=:schema_id");
    m_sqlDriver->bind(":schema_id", cellValue(l.toList().at(0), 0));
    m_sqlDriver->execSQL();

    m_sqlDriver->close();

    removeRow(l.toList().at(0));
}

void QPrintSchema::actionCostum(int action)
{
    switch (action) {
    case ACTION_MENU:
        editPrintersList();
        break;
    }
}

void QPrintSchema::editPrintersList()
{
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    QPrintersOfSchema *p = new QPrintersOfSchema(cellValue(l.toList().at(0), 0).toInt(), cellValue(l.toList().at(0), 1).toString(), this);
    p->show();
}
