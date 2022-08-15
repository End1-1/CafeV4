#include "qeditwsettings.h"
#include "ui_qeditwsettings.h"
#include <QClipboard>

QEditWSettings::QEditWSettings(QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::QEditWSettings)
{
    ui->setupUi(this);

    m_actions << "actionSave" << "actionCopy" << "actionPaste";
    ui->grid->setColumnWidth(0, 350);
    ui->grid->setColumnWidth(1, 200);
}

QEditWSettings::~QEditWSettings()
{
    delete ui;
}

void QEditWSettings::setId(const QString &id)
{
    m_id = id;
    m_sqlDrv->prepare("select key_name, key_value from sys_settings_values where settings_id=:settings_id order by 1");
    m_sqlDrv->bind(":settings_id", m_id);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->m_query->next()) {
        int row = ui->grid->rowCount();
        ui->grid->setRowCount(ui->grid->rowCount() + 1);
        QTableWidgetItem *i1 = new QTableWidgetItem(m_sqlDrv->valStr("KEY_NAME"));
        i1->setFlags(i1->flags() ^ Qt::ItemIsEditable);
        ui->grid->setItem(row, 0, i1);
        QTableWidgetItem *i2 = new QTableWidgetItem(m_sqlDrv->valStr("KEY_VALUE"));
        ui->grid->setItem(row, 1, i2);
    }

    m_sqlDrv->prepare("select name from sys_settings where id=:id");
    m_sqlDrv->bind(":id", m_id);
    m_sqlDrv->execSQL();
    m_sqlDrv->m_query->next();
    setWindowTitle(tr("Sales settings") + ": " + m_sqlDrv->valStr("NAME"));
    m_sqlDrv->close();
}

void QEditWSettings::actionCostum(int action)
{
    switch (action) {
    case ACTION_COPY:
        copy();
        break;
    case ACTION_PASTE:
        paste();
        break;
    }
}

void QEditWSettings::actionSave()
{
    m_sqlDrv->prepare("delete from sys_settings_values where settings_id=:settings_id");
    m_sqlDrv->bind(":settings_id", m_id);
    m_sqlDrv->execSQL();
    m_sqlDrv->prepare("insert into sys_settings_values (settings_id, key_name, key_value) values (:settings_id, :key_name, :key_value)");
    for (int i = 0; i < ui->grid->rowCount(); i++) {
        m_sqlDrv->bind(":settings_id", m_id);
        m_sqlDrv->bind(":key_name", ui->grid->item(i, 0)->text());
        m_sqlDrv->bind(":key_value", ui->grid->item(i, 1)->text());
        m_sqlDrv->execSQL();
    }
    m_sqlDrv->close();

    QMessageBox::information(this, tr("Information"), tr("Saved"));
}

void QEditWSettings::copy()
{
    QString row = "";
    for (int i = 0; i < ui->grid->rowCount(); i++)
         row += ui->grid->item(i, 0)->text() + ";" + ui->grid->item(i, 1)->text() + "\n";

    QClipboard *clipboard = qApp->clipboard();
    clipboard->setText(row);
}

void QEditWSettings::paste()
{
    QClipboard *clipboard = qApp->clipboard();
    QStringList data = clipboard->text().split("\n", QString::SkipEmptyParts);
    for (QStringList::const_iterator it = data.begin(); it != data.end(); it++) {
        QStringList row = it->split(";");
        for (int i = 0; i < ui->grid->rowCount(); i++) {
            if (ui->grid->item(i, 0)->text() == row[0]) {
                ui->grid->item(i, 1)->setData(Qt::DisplayRole, row[1]);
                break;
            }
        }
        int newRow = ui->grid->rowCount();
        ui->grid->setRowCount(newRow + 1);
        ui->grid->setItem(newRow, 0, new QTableWidgetItem(row[0]));
        ui->grid->setItem(newRow, 1, new QTableWidgetItem(row[1]));
    }
}

