#ifndef DLGFOODCACHE_H
#define DLGFOODCACHE_H

#include "qdlgbase.h"

namespace Ui {
class DlgFoodCache;
}

struct Food {
    int id;
    QString name;
    QString unit;
    QString price;
};

class DlgFoodCache : public QDlgBase
{
    Q_OBJECT

public:
    explicit DlgFoodCache(QWidget *parent = 0);
    ~DlgFoodCache();
    virtual void btnOK();
    Food m_result;

private:
    Ui::DlgFoodCache *ui;
    static QList<Food> m_foodCache;
    void refreshFoodCache();
    QList<Food> m_proxyFood;
    void prepareTable();
    void foodToTable();
    void tryAccept();

    template <typename T>
    void builList(const QMap<T, Food> &data) {
        m_proxyFood.clear();
        for (typename QMap<T, Food>::const_iterator i = data.begin(); i != data.end(); i++)
            m_proxyFood.append(i.value());
        foodToTable();
    }

protected:
    virtual bool eventFilter(QObject *o, QEvent *e);

private slots:
    void textChangedCode(const QString &text);
    void textChangedName(const QString &text);
    void on_tblGoods_doubleClicked(const QModelIndex &index);
};

#endif // DLGFOODCACHE_H
