#ifndef QIMPORTFROMAS_H
#define QIMPORTFROMAS_H

#include "qbasesqlwindow.h"
#include <QDate>

namespace Ui {
class QImportFromAS;
}

struct Document {
    QString isn;
    QString num;
    int type;
    double amount;
    QString body;
    int storeIn;
    int storeOut;
    QDate date;
};

struct Record {
    int id;
    double qty;
    double amount;
    int fdbcr;
};

class QImportFromAS : public QBaseSqlWindow
{
    Q_OBJECT

public:
    explicit QImportFromAS(QWidget *parent = nullptr);
    ~QImportFromAS();
    virtual void actionSave();
    virtual void actionRefresh();

private slots:
    void on_actionAdd_triggered();
    void on_actionEdit_triggered();
    void on_actionRemove_triggered();
    void on_actionAddFood_triggered();
    void on_actionEditFood_triggered();
    void on_actionRemoveFood_triggered();
    void on_chNoPriceUPdate_clicked(bool checked);

private:
    Ui::QImportFromAS *ui;
    void editPairValues(QTableWidget *t);
    void addPairValues(QTableWidget *t);
    void delPairValues(QTableWidget *t);
    void addRow(QTableWidget *t, const QString &v1, const QString &v2);
    void log(const QString &message);

    void getDocStorages(Document &d, QMap<QString, int> &s);
};

#endif // QIMPORTFROMAS_H
