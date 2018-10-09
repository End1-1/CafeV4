#ifndef QDLGRECIPEHISTORY_H
#define QDLGRECIPEHISTORY_H

#include <QDialog>
#include <QDateTime>
#include "qsqldrv.h"

namespace Ui {
class QDlgRecipeHistory;
}

typedef struct {
    int historyId;
    QDateTime changeTime;
    QString userName;
} History;

class QDlgRecipeHistory : public QDialog
{
    Q_OBJECT

public:
    explicit QDlgRecipeHistory(int dishId, QWidget *parent = 0);
    ~QDlgRecipeHistory();

private slots:
    void on_btnPrev_clicked();

    void on_btnNext_clicked();

private:
    Ui::QDlgRecipeHistory *ui;
    QSqlDrv *m_sqlDrv;
    int m_dish;
    QList<History> m_history;
    int m_historyIndex;
    void loadHistory();
};

#endif // QDLGRECIPEHISTORY_H
