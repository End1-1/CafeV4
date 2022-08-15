#ifndef TABLEWIDGET_H
#define TABLEWIDGET_H


#include <QTableWidget>

class QLineEdit;
class QComboBox;
class QCheckBox;


class TableWidgetItem : public QTableWidgetItem {
public:
    explicit TableWidgetItem(int type = Type);

    explicit TableWidgetItem(const QString &text, int type = Type);

    virtual QVariant data(int role) const override;

    inline QString text() {return data(Qt::DisplayRole).toString(); }

    int fDecimals;
};

class TableWidget : public QTableWidget
{
    Q_OBJECT

public:
    TableWidget(QWidget *parent = nullptr);

    TableWidgetItem *item(int row, int column) const;

    void setColumnWidths(int count, ...);

    void fitColumnsToWidth(int dec = 5);

    void fitRowToHeight(int dec = 5);

    QLineEdit *createLineEdit(int row, int column);

    QLineEdit *lineEdit(int row, int column);

    QComboBox *createComboBox(int row, int column);

    QComboBox *comboBox(int row, int column);

    QCheckBox *createCheckbox(int row, int column);

    QCheckBox *checkBox(int row, int column);

    bool findWidget(QWidget *w, int &row, int &column);

    QVariant getData(int row, int column);

    void setData(int row, int column, const QVariant &value);

    int getInteger(int row, int column);

    void setInteger(int row, int column, int value);

    QString getString(int row, int column);

    void setString(int row, int column, const QString &str);

    double getDouble(int row, int column);

    void setDouble(int row, int column, double value);

    int addEmptyRow();

    void exportToExcel();

    void search(const QString &txt);

    double sumOfColumn(int column);

    void setColumnDecimals(int column, int decimals);

private:
    QMap<int, int> fColumnsDecimals;

private slots:
    void lineEditTextChanged(const QString arg1);

    void comboTextChanged(const QString &text);

    void checkBoxChecked(bool v);
};

#endif // TABLEWIDGET_H
