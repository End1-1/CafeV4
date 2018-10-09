#include "wdtcalculator.h"
#include "ui_wdtcalculator.h"

WdtCalculator::WdtCalculator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdtCalculator)
{
    ui->setupUi(this);
    ui->leDisplay->installEventFilter(this);
    ui->leDisplay->setValidator(new EditValidator());
}

WdtCalculator::~WdtCalculator()
{
    delete ui;
}

void WdtCalculator::calculate()
{
    if (ui->leDisplay->text() == "ERROR")
        return;

    ui->lbHistory->setText(ui->leDisplay->text() + "=");

    QStringList data;
    QString tempStr;
    for (int i = 0, count = ui->leDisplay->text().length(); i < count; i++) {
        QChar chr = ui->leDisplay->text().at(i);
        if (EditValidator::charOp(chr)) {
            if (!i) {
                tempStr += chr;
                continue;
            }
            data.append(tempStr);
            data.append(chr);
            tempStr = "";
        } else
            tempStr += chr;
    }
    if (tempStr.length())
        data.append(tempStr);
    if (EditValidator::charOp(data.last().at(0)))
        data.removeLast();

    double result = 0;
    QString str = "0";
    QChar op = '?';
    for (QStringList::const_iterator i = data.begin(); i != data.end(); i++) {
        QChar chr = (*i).at(0);
        if (EditValidator::charOp(chr))
            op = chr;
        else {
            str = *i;
            switch (op.toLatin1()) {
            case '+':
                result += str.toDouble();
                break;
            case '-':
                result -= str.toDouble();
                break;
            case '*':
                result *= str.toDouble();
                break;
            case '/':
                if (str.toDouble() == 0) {
                    ui->leDisplay->setText("ERROR");
                    return;
                }
                else
                    result /= str.toDouble();
                break;
            case '?':
                result = str.toDouble();
                break;
            }
        }
    }

    ui->leDisplay->setText(Toolkit::formatDouble(result));
    ui->lbHistory->setText(ui->lbHistory->text() + ui->leDisplay->text());
}

void WdtCalculator::clear()
{
    ui->leDisplay->clear();
}

bool WdtCalculator::eventFilter(QObject *o, QEvent *e)
{
    if (o == ui->leDisplay) {
        if (e->type() == QEvent::KeyPress) {
            QKeyEvent *ke = static_cast<QKeyEvent*>(e);
            if (ke->key() == Qt::Key_Backspace)
                if (ui->leDisplay->text() == "ERROR")
                    ui->leDisplay->clear();
        }
    }
    return QWidget::eventFilter(o, e);
}

void WdtCalculator::btnClick()
{
    QString str = ui->leDisplay->text() + (qobject_cast<QPushButton*>(sender()))->text();
    ui->leDisplay->setText(str);
}

QValidator::State EditValidator::validate(QString &str, int &pos) const
{
    if (!str.length())
        return Acceptable;

    if (!pos)
        return Invalid;

    QChar chr = str.at(pos - 1);
    if (chr == ',')
        return Invalid;

    if (chr == '.')
        if (!decimalPoint(str, pos))
            return Invalid;

    if (charOp(chr)) {
        if (pos > 2)
            if (str.at(pos - 2) == chr)
                return Invalid;
        if (pos < str.length() - 2)
            if (str.at(pos + 2) == chr)
                return Invalid;
    } else
        return Acceptable;

    str = str.replace(QRegExp("[^0-9,\\.,\\/,\\*,\\+,\\-]"), "");

    return Acceptable;
}

bool EditValidator::decimalPoint(QString &str, int pos)
{
    int right = 0, left = 0;
    for (int i = pos - 1; i < str.length(); i++) {
        QChar chr = str.at(i);
        if (charOp(chr)) {
            right = i;
            break;
        }
        right = i + 1;
    }

    for (int i = pos - 1; i > -1; i--) {
        QChar chr = str.at(i);
        if (charOp(chr)) {
            left = i;
            break;
        }
        left = i;
    }

    QString num = str.mid(left, right);
    qDebug() << num.indexOf('.', pos -1)  << num.indexOf('.',  pos);
    if (((num.indexOf('.', 0) < pos - 1) && (num.indexOf('.', pos - 1) > -1)) || (num.indexOf('.', pos) > pos - 1))
        return false;
    else
        return true;
}

bool EditValidator::charOp(const QChar &chr)
{
    return (chr == '+') || (chr == '-') || (chr == '*') || (chr == '/');
}

void WdtCalculator::on_btnClear_clicked()
{
    clear();
}

void WdtCalculator::on_btn2_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn3_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn4_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn5_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn6_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn7_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn8_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn9_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn0_clicked()
{
    btnClick();
}

void WdtCalculator::on_btn1_clicked()
{
    btnClick();
}

void WdtCalculator::on_btnPlus_clicked()
{
    btnClick();
}

void WdtCalculator::on_btnMinus_clicked()
{
    btnClick();
}

void WdtCalculator::on_btnMult_clicked()
{
    btnClick();
}

void WdtCalculator::on_btnDiv_clicked()
{
    btnClick();
}


void WdtCalculator::on_btnCalc_clicked()
{
    calculate();
}
