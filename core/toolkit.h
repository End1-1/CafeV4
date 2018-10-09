#ifndef TOOLKIT_H
#define TOOLKIT_H

#include <QObject>
#include <QTableWidget>
#include <QComboBox>
#include <QDateEdit>

class Toolkit : public QObject
{
    Q_OBJECT

public:
    explicit Toolkit(QObject *parent = 0);
    static int newRow(QTableWidget *t);
    static int currentRow(QTableWidget *table, int &row);
    static void updateRow(QTableWidget *table, const int &row);
    static bool rowUp(QTableWidget *table);
    static bool rowDown(QTableWidget *table);
    static void columnLeft(QTableWidget *table);
    static void columnRight(QTableWidget *table);
    static bool comboStringData(QComboBox *comboBox, QString &result);
    static void comboSetIndexByData(QComboBox *comboBox, const QString &data);
    static int comboItemData(QComboBox *comboBox);
    static void setFirstAndLastDateEdit(QDateEdit *date1, QDateEdit *date2);

    // Фунция форматируе число с десятичной дробью в окончательный вариант в ввиде строки
    static QString formatDouble(const double &value, const int &f = 2);
    template<class C>
    static bool validRow(const int  &row, const C &container) {
        return (row > -1) && (row < container.count());
    }
};

#endif // TOOLKIT_H
