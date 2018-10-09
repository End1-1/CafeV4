#include "qprintersofschema.h"
#include "dlgprinterofschema.h"
#include "core.h"

QPrintersOfSchema::QPrintersOfSchema(int id, const QString &name, QWidget *parent) :
    QGrid(parent),
    m_id(id)
{
    m_actions << "actionNew"  << "actionDelete" << "actionMultiselection"
              << "actionRefersh" << "actionSave";
    setWindowTitle(tr("Printers of schema") + ": " + name);

    addField("NAME", tr("Printer"), 200, QVariant::String);
    actionBeforeSelect();
}

void QPrintersOfSchema::actionBeforeSelect()
{
    m_sqlString = "select name from me_printers where schema_id=" + its(m_id);
    actionSelect();
}

void QPrintersOfSchema::actionNew()
{
    CHECK_VIEWER_AND_MAINDB
    DlgPrinterOfSchema *d = new DlgPrinterOfSchema(m_lastWorkstation, this);
    if (d->exec() == QDialog::Accepted) {
        m_lastWorkstation = d->m_lastWorkstation;
        int newRow = Toolkit::newRow(tableWidget(0));
        setCellValue(newRow, 0, d->m_printer);
    }
    delete d;
}

void QPrintersOfSchema::actionSave()
{
    CHECK_VIEWER_AND_MAINDB
    m_sqlDriver->prepare("delete from me_printers where schema_id=:schema_id");
    m_sqlDriver->bind(":schema_id", m_id);
    m_sqlDriver->execSQL();

    m_sqlDriver->prepare("insert into me_printers values (null, :schema_id, :printer)");
    for (int i = 0; i < tableWidget(0)->rowCount(); i++) {
        m_sqlDriver->bind(":schema_id", m_id);
        m_sqlDriver->bind(":printer", cellValue(i, 0));
        m_sqlDriver->execSQL();
    }

    m_sqlDriver->close();
}

void QPrintersOfSchema::actionDelete()
{
    CHECK_VIEWER_AND_MAINDB
    QSet<int> l;
    if (!selectedRows(l)) {
        QMessageBox::warning(this, tr("Warning"), tr("Nothing is selected"));
        return;
    }

    for (QSet<int>::const_iterator it = l.begin(); it != l.end(); it++)
        removeRow(*it);
}

