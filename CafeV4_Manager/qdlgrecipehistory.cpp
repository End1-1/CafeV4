#include "qdlgrecipehistory.h"
#include "ui_qdlgrecipehistory.h"

QDlgRecipeHistory::QDlgRecipeHistory(int dishId, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QDlgRecipeHistory)
{
    ui->setupUi(this);
    m_sqlDrv = new QSqlDrv("admin", "clientdb");
    ui->tblRecipe->setColumnWidth(0, 300);
    ui->tblRecipe->setColumnWidth(1, 100);
    m_dish = dishId;
    m_sqlDrv->openDB();
    m_sqlDrv->prepare("select h.id, e.fname || ' ' || e.lname, changed from me_recipes_change h, employes e where h.staff_id=e.id and dish_id=:dish_id");
    m_sqlDrv->bind(":dish_id", dishId);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        History h;
        h.historyId = m_sqlDrv->val().toInt();
        h.userName = m_sqlDrv->val().toString();
        h.changeTime = m_sqlDrv->val().toDateTime();
        m_history.append(h);
    }
    m_sqlDrv->prepare("select name from me_dishes where id=:id");
    m_sqlDrv->bind(":id", dishId);
    m_sqlDrv->execSQL();
    m_sqlDrv->next();
    ui->leDish->setText(m_sqlDrv->val().toString());
    m_sqlDrv->close();
    m_historyIndex = 0;
    loadHistory();
}

QDlgRecipeHistory::~QDlgRecipeHistory()
{
    delete ui;
}

void QDlgRecipeHistory::on_btnPrev_clicked()
{
    m_historyIndex--;
    if (m_historyIndex < 0)
        m_historyIndex = m_history.count() - 1;
    if (m_historyIndex < 0)
        m_historyIndex = 0;
    loadHistory();
}

void QDlgRecipeHistory::on_btnNext_clicked()
{
    m_historyIndex++;
    if (m_historyIndex > m_history.count() - 1)
        m_historyIndex = 0;
    loadHistory();
}

void QDlgRecipeHistory::loadHistory()
{
    if (!m_history.count())
        return;
    ui->tblRecipe->clearContents();
    ui->tblRecipe->setRowCount(0);
    History h = m_history[m_historyIndex];
    ui->leDate->setText(h.changeTime.toString(DATETIME_FORMAT));
    ui->leUser->setText(h.userName);
    m_sqlDrv->openDB();
    m_sqlDrv->prepare("select fn.name, h.qty from food_names fn, me_recipes_history h where h.food_id=fn.id and h.history_id=:id");
    m_sqlDrv->bind(":id", h.historyId);
    m_sqlDrv->execSQL();
    while (m_sqlDrv->next()) {
        int row = ui->tblRecipe->rowCount();
        ui->tblRecipe->setRowCount(row + 1);
        ui->tblRecipe->setItem(row, 0, new QTableWidgetItem(m_sqlDrv->val().toString()));
        ui->tblRecipe->setItem(row, 1, new QTableWidgetItem(m_sqlDrv->val().toString()));
    }
    m_sqlDrv->close();
    ui->lbIndex->setText(QString("%1 / %2").arg(m_historyIndex + 1).arg(m_history.count()));
}
