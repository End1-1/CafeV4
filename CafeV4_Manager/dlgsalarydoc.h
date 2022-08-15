#ifndef DLGSALARYDOC_H
#define DLGSALARYDOC_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QLineEdit>
#include "core.h"
#include "qbasesqlwindow.h"

namespace Ui {
class DlgSalaryDoc;
}

struct Salary {
    int function;
    QString params;
    int cell;
};

class QTableWidgetLineEdit : public QLineEdit {
    Q_OBJECT

    QTableWidgetItem *m_tableWidgetItem;

public:
    QTableWidgetLineEdit(QTableWidgetItem *item);
private slots:
    void textChanged(const QString &text);
protected:
    virtual void focusOutEvent(QFocusEvent *e);
signals:
    void focusOut();
};

class DlgSalaryDoc : public QBaseSqlWindow
{
    Q_OBJECT
    
public:
    explicit DlgSalaryDoc(const QString &docid, QWidget *parent = 0);
    explicit DlgSalaryDoc(QWidget *parent = 0);
    ~DlgSalaryDoc();
    void setDoc(const QString &docid);
    virtual void actionPrintPreview();
    virtual void actionPrint();
    virtual void actionSave();
    virtual void actionCostum(int a);
    virtual void actionRefresh();
    
private:
    bool m_loadedAutomaticaly;
    QMap<int, QList<Salary> > m_salaries;
    QString m_hallNames;
    QString m_hallValues;
    QMap<int, QMap<QString, QString> > m_employes;
    QMap<QString, QString> m_post;
    Ui::DlgSalaryDoc *ui;
    void addEmployee(QList<QVariant> &data);
    void getPrintable(XmlPrintMaker &x);
    void getEmployesList();
    void getCashList();
    void getDoc();
    void changeSavedState(bool saved);
    void countSalaries();
    void getSalariesFormulas();
    float getSalaryOfGroup(int groupId, int currentstaff);

private slots:
    void cbEmployeeKeyEnterPressed();
    void on_btnAdd_clicked();
    void on_tblList_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn);
    void on_btnDel_clicked();
    void countTotalAmount();
    void countTotalAmount(const QString &text);
    void on_btnChangePost_clicked();
    void on_deDate_dateChanged(const QDate &date);
    void on_deStart_dateChanged(const QDate &date);
    void on_deEnd_dateChanged(const QDate &date);
    void on_btnHall_clicked();
    void on_cbCash_currentIndexChanged(int index);
};

#endif // DLGSALARYDOC_H
