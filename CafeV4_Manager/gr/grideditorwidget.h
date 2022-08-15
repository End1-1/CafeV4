#ifndef GRIDEDITORWIDGET_H
#define GRIDEDITORWIDGET_H

#include "sqlthread.h"
#include "grdb.h"
#include <QWidget>

class QLineEdit;

class GridEditorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GridEditorWidget(QWidget *parent = nullptr);
    ~GridEditorWidget();
    virtual void clearWidgets();
    virtual void load(int id) = 0;
    virtual bool isOk();
    virtual void save();
    virtual QString title() const = 0;
    virtual void prepare();

protected:
    QString mTableName;
    QMap<QString, QWidget*> mDbWidgets;
    QLineEdit *mCodeLineEdit;
    void fillFields();
    virtual void setTableName() = 0;

protected slots:
    virtual void sqlFinished();
    virtual void saveFinished();

private:
    void enumDbWidgets(QWidget *parent);
    void setDbWidgetValue(QWidget *w, const QVariant &v);
    QVariant getDbWidgetValue(QWidget *w) const;
    void clearWidget(QWidget *w);

private slots:
    void saveError(const QString &err);
};

#endif // GRIDEDITORWIDGET_H
