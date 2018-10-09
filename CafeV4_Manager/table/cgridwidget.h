#ifndef CGRIDWIDGET_H
#define CGRIDWIDGET_H

#include <QWidget>
#include <QMap>

class CDatabase;
class CTableModel;
class CFilterDialog;

namespace Ui {
class CGridWidget;
}

class CGridWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CGridWidget(const QString &guid, QWidget *parent = 0);
    ~CGridWidget();
    void setQuery(const QString &query);
    CGridWidget &setColumnName(const QString &field, const QString &name, int width);

    QString fePrimaryKeyName;
    QString feMasterTableName;
    QMap<int, QString> fMasterTableKey;
    QMap<int, QString> fSlaveTableName;
    QMap<int, QString> fSlaveTableKey;
    QMap<int, QString> fSlaveTableValue;
protected:

private:
    Ui::CGridWidget *ui;
    CTableModel *fTableModel;
    CDatabase *fDatabase;
    QString fQuery;
    QString fGuid;
    QMap<QString, int> fColumnWidths;
    QMap<int, CFilterDialog*> fFilters;
private slots:
    virtual void filterRequest(int i);
    void endOfLoadData();
    void on_leQuickSearch_textChanged(const QString &arg1);
    void on_tblData_doubleClicked(const QModelIndex &index);
};

extern QString __DBPATH__;
extern QString __DBUSER__;
extern QString __DBPASS__;

#endif // CGRIDWIDGET_H
