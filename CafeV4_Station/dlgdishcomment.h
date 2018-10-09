#ifndef DLGDISHCOMMENT_H
#define DLGDISHCOMMENT_H

#include "core.h"

namespace Ui {
class DlgDishComment;
}

class DlgDishComment : public QDialog
{
    Q_OBJECT

    class QLetterDelegate : public QItemDelegate {
    protected:
        void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    };
    
public:
    explicit DlgDishComment(const QStringList &comment, QWidget *parent = 0);
    ~DlgDishComment();
    QString result();
    void setSingleSelection();
    QMap<int,QStringList> m_letters;
    
private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_tableWidget_clicked(const QModelIndex &index);
    void on_pushButton_5_clicked();
    void on_tblLetter_clicked(const QModelIndex &index);
    void on_pushButton_6_clicked();

private:
    Ui::DlgDishComment *ui;
};

#endif // DLGDISHCOMMENT_H
