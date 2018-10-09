#ifndef DOCKIDNAME_H
#define DOCKIDNAME_H

#include <QDockWidget>
#include <QMap>
#include <QList>

namespace Ui {
class DockIdName;
}

class DockIdName : public QDockWidget
{
    Q_OBJECT

private:
    QMap<QString, int> m_codeList;
    QMap<QString, int> m_namesList;
    void showDock();
    void makeIndexesList(const QMap<QString, int> &container, const QString &filter);
    
public:
    QList<int> m_filteredItems;

    explicit DockIdName(const QString &title, QWidget *parent);
    ~DockIdName();
    void appendIdName(const QString &id, const QString &name, const int &row);
    void setCodeNameList(const QStringList &ids, const QStringList &names);
    void setCodeFilter(const QString &filter);
    void setNameFilter(const QString &filter);
    void setPos(QWidget *widget);
    void rowUp();
    void rowDown();
    bool tryClick();
    
private slots:
    void on_tableWidget_doubleClicked(const QModelIndex &index);
    void on_actionActionKeyEnter_triggered();

private:
    Ui::DockIdName *ui;

protected:
    bool eventFilter(QObject *object, QEvent *event);

signals:
    void clickedRow(const int &row);
};

#endif // DOCKIDNAME_H
