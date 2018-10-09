#include "dlgsalaryformula.h"
#include "ui_dlgsalaryformula.h"
#include "mdibutton.h"
#include "core.h"

DlgSalaryFormula::DlgSalaryFormula(const QString &postId, const QString &postName, QWidget *parent) :
    QBaseSqlWindow(parent),
    ui(new Ui::DlgSalaryFormula),
    m_postId(postId)
{
    m_actions << "actionNew" << "actionEdit" << "actionDelete" << "actionSave"
              << "actionUp" << "actionDown";

    ui->setupUi(this);
    setWindowTitle(windowTitle() + " - " + postName);
    QList<int> tableColWidths;
    tableColWidths << 0 << 300 << 0 << 300 << 150;
    for (int i = 0, count = tableColWidths.count(); i < count; i++)
        ui->table->setColumnWidth(i, tableColWidths[i]);

    getFormulaData();
}

DlgSalaryFormula::~DlgSalaryFormula()
{
    delete ui;
}

void DlgSalaryFormula::actionNew()
{
    DlgAddSalaryFunction *d = new DlgAddSalaryFunction(m_functions, this);
    if (d->exec() == QDialog::Accepted) {
        DlgAddSalaryFunction::Function f;
        d->getData(f);
        int row = Toolkit::newRow(ui->table);
        functionToTable(row, f);
    }
    delete d;
}

void DlgSalaryFormula::actionEdit()
{
    QModelIndexList l = ui->table->selectionModel()->selectedRows();
    if (!l.count())
        return;

    DlgAddSalaryFunction::Function f;
    f.id = ui->table->item(l.at(0).row(), 0)->data(Qt::DisplayRole).toInt();
    f.name = ui->table->item(l.at(0).row(), 1)->data(Qt::DisplayRole).toString();
    f.desc = ui->table->item(l.at(0).row(), 2)->data(Qt::DisplayRole).toString();
    f.params = ui->table->item(l.at(0).row(), 3)->data(Qt::DisplayRole).toString();
    f.cell = ui->table->item(l.at(0).row(), 4)->data(Qt::DisplayRole).toString();

    DlgAddSalaryFunction *d = new DlgAddSalaryFunction(m_functions, this);
    d->setData(f);
    if (d->exec() == QDialog::Accepted) {
        d->getData(f);
        functionToTable(l.at(0).row(), f);
    }
    delete d;
}

void DlgSalaryFormula::actionDelete()
{
    int row = Toolkit::currentRow(ui->table, row);
    if (row < 0)
        return;
    ui->table->removeRow(row);
}

void DlgSalaryFormula::actionCostum(int action)
{
    switch (action) {
    case ACTION_UP:
        rowUp();
        break;
    case ACTION_DOWN:
        rowDown();
        break;
    }
}

void DlgSalaryFormula::actionSave()
{
    m_sqlDrv->prepare("delete from employes_salary where group_id=:group_id");
    m_sqlDrv->bind(":group_id", m_postId);
    m_sqlDrv->execSQL();

    for (int i = 0; i < ui->table->rowCount(); i++) {
        m_sqlDrv->prepare("insert into employes_salary values (null, :group_id, :func_id, :params, :cell)");
        m_sqlDrv->bind(":group_id", m_postId);
        m_sqlDrv->bind(":func_id", ui->table->item(i, 0)->text());
        m_sqlDrv->bind(":params", ui->table->item(i, 3)->text());
        m_sqlDrv->bind(":cell", ui->table->item(i, 4)->text());
        m_sqlDrv->execSQL();
    }

    m_sqlDrv->close();
}

void DlgSalaryFormula::getFormulaData()
{
    m_sqlDrv->execSQL("select id, name, description from employes_salary_functions");
    while (m_sqlDrv->next()) {
        DlgAddSalaryFunction::Function f;
        f.id = m_sqlDrv->val().toInt();
        f.name = m_sqlDrv->val().toString();
        f.desc = m_sqlDrv->val().toString();
        m_functions[f.id] = f;
    }

    if (m_postId.toInt()) {
        m_sqlDrv->prepare("select es.func, ef.name, ef.description, es.params, es.cell "
                          "from employes_salary es, employes_salary_functions ef "
                          "where es.func=ef.id and es.group_id=:group_id");
        m_sqlDrv->bind(":group_id", m_postId);
        m_sqlDrv->execSQL();
        while (m_sqlDrv->next()) {
            int row = Toolkit::newRow(ui->table);
            for (int i = 0; i < ui->table->columnCount(); i++)
                ui->table->item(row, i)->setText(m_sqlDrv->val().toString());
        }
    }

    m_sqlDrv->close();
}

void DlgSalaryFormula::functionToTable(int row, const DlgAddSalaryFunction::Function &f)
{
    ui->table->item(row, 0)->setData(Qt::DisplayRole, f.id);
    ui->table->item(row, 1)->setData(Qt::DisplayRole, f.name);
    ui->table->item(row, 2)->setData(Qt::DisplayRole, f.desc);
    ui->table->item(row, 3)->setData(Qt::DisplayRole, f.params);
    ui->table->item(row, 4)->setData(Qt::DisplayRole, f.cell);
}

void DlgSalaryFormula::replaceRows(int r1, int r2)
{
    QString s;
    for(int i = 0; i < ui->table->columnCount(); i++) {
        s = ui->table->item(r1, i)->text();
        ui->table->item(r1, i)->setText(ui->table->item(r2, i)->text());
        ui->table->item(r2, i)->setText(s);
    }
}

void DlgSalaryFormula::rowUp()
{
    int row = Toolkit::currentRow(ui->table, row);
    if (row <= 0)
        return;
    replaceRows(row, row - 1);
    ui->table->setCurrentCell(row - 1, 0);
}

void DlgSalaryFormula::rowDown()
{
    int row = Toolkit::currentRow(ui->table, row);
    if (row < 0)
        return;
    if (row == ui->table->rowCount() - 1)
        return;

    replaceRows(row, row + 1);
    ui->table->setCurrentCell(row + 1, 0);
}

void DlgSalaryFormula::on_table_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid())
        return;
    actionEdit();
}
