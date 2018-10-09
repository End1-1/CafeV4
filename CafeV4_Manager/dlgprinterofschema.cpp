#include "dlgprinterofschema.h"
#include "ui_dlgprinterofschema.h"

DlgPrinterOfSchema::DlgPrinterOfSchema(QString lastWorkstation, QWidget *parent) :
    QDlgBase(parent),
    ui(new Ui::DlgPrinterOfSchema)
{
    ui->setupUi(this);
    m_sqlDriver->execSQL("select workstation, printer from sys_workstation_printers order by 1, 2 ");
    while (m_sqlDriver->next()) {
        QString ws = m_sqlDriver->val().toString();
        QString prn = m_sqlDriver->val().toString();
        if (!m_printers.contains(ws))
            m_printers[ws] = QStringList();
        m_printers[ws].append(prn);
    }

    QStringList workstations = m_printers.keys();
    for (QStringList::const_iterator i = workstations.begin(); i != workstations.end(); i++)
        ui->cbWorkstation->addItem(*i);

    if (lastWorkstation.length())
        ui->cbWorkstation->setCurrentIndex(ui->cbWorkstation->findText(lastWorkstation));
    else
        ui->cbWorkstation->setCurrentIndex(-1);
}

DlgPrinterOfSchema::~DlgPrinterOfSchema()
{
    delete ui;
}

void DlgPrinterOfSchema::btnOK()
{
    if (ui->cbPrinter->currentIndex() < 0)
        return;
    m_printer = ui->cbPrinter->currentText();
    accept();
}

void DlgPrinterOfSchema::on_cbWorkstation_currentTextChanged(const QString &arg1)
{
    ui->cbPrinter->clear();
    if (!m_printers.contains(arg1))
        return;
    QStringList &printers = m_printers[arg1];
    m_lastWorkstation = arg1;
    for (QStringList::const_iterator i = printers.begin(); i != printers.end(); i++)
        ui->cbPrinter->addItem(*i);

    ui->cbPrinter->setCurrentIndex(-1);
}
