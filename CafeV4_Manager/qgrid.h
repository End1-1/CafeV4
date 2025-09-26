#ifndef QGRID_H
#define QGRID_H

#include "../qbasegrid.h"
#include "core.h"
#include <QMessageBox>

class QGrid : public QBaseGrid
{
    Q_OBJECT

public:
    explicit QGrid(QWidget *parent = 0);

signals:

public slots:

protected:

    template <class T>
    void createEditorDlg(bool isNew = true)
    {
            QMap<QString, QList<QVariant> > values;
            QModelIndexList rows = selectedRows();
            if (!isNew) {
                if (!rows.count()) {
                    QMessageBox::critical(this, tr("Error"), tr("No object selected"));
                    return;
                }
                else {
                    for (QModelIndexList::const_iterator it = rows.begin(); it != rows.end(); it++)
                        for (int i = 0; i < m_fields.count(); i++)
                            if (m_fields.at(i)->isIncluded())
                                values[m_fields.at(i)->name].append(itemData(it->row(), fieldIndexInGrid(m_fields.at(i)->name)));
                }
            }
            bool next = false;
            QString code = "";
            do {
                T *t = new T(values, this);
                t->setNext(next, code);
                t->setWindowTitle(windowTitle());
                t->setTable(m_editDlgTable, m_fields);
                t->mapValuesToWidget();
                if (t->exec() == QDialog::Accepted) {
                    if (isNew) {
                        int row = appendRow();
                        for (QMap<QString, QList<QVariant> >::const_iterator it = values.begin(); it != values.end(); it++) {
                            int col = fieldIndexInGrid(it.key());
                            if (col < 0)
                                continue;
                            for (int i = 0, count = it.value().count(); i < count; i++)
                                setItemData(row, col, it.value().at(i));
                        }
                        values.clear();
                    } else {
                        for (QMap<QString, QList<QVariant> >::const_iterator it = values.begin(); it != values.end(); it++) {
                            int col = fieldIndexInGrid(it.key());
                            if (col < 0)
                                continue;
                            for (int i = 0, count = it.value().count(); i < count; i++)
                                setItemData(rows.at(i).row(), col, it.value().at(i));
                        }
                    }
                } else
                    next = false;
                next = t->mNext;
                code = t->mCurrentCode;
                delete t;
            } while (next);
        }
};

#endif // QGRID_H
