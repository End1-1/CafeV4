#include "dockidname.h"
#include "ui_dockidname.h"
#include <QDesktopServices>
#include <QScreen>
#include "../core/toolkit.h"
#include <QKeyEvent>

DockIdName::DockIdName(const QString &title, QWidget *parent) :
    QDockWidget(title, parent, Qt::WindowStaysOnTopHint),
    ui(new Ui::DockIdName)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0, 100);
    ui->tableWidget->setColumnWidth(1, 300);
    setMinimumWidth(410);
    QStringList tableTitles;
    tableTitles << tr("Code") << tr("Name");
    ui->tableWidget->setHorizontalHeaderLabels(tableTitles);
    ui->tableWidget->installEventFilter(this);
}

DockIdName::~DockIdName()
{
    delete ui;
}

void DockIdName::appendIdName(const QString &id, const QString &name, const int& row)
{
    m_codeList.insert(id, row);
    m_namesList.insert(name, row);
}

void DockIdName::setCodeNameList(const QStringList &ids, const QStringList &names)
{
    for(int i = 0, count = ids.count(); i < count; i++)
        appendIdName(ids[i], names[i], i);
}

void DockIdName::setCodeFilter(const QString &filter)
{
    if(!filter.length()) {
        hide();
        return;
    }

    setWindowTitle(filter);
    makeIndexesList(m_codeList, filter);
    showDock();
}

void DockIdName::setNameFilter(const QString &filter)
{
    if(!filter.length()) {
        hide();
        return;
    }

    setWindowTitle(filter);
    makeIndexesList(m_namesList, filter);
    showDock();
}

void DockIdName::setPos(QWidget *widget)
{
    QPoint point(0, widget->height() + 1);
    QScreen *w = QApplication::screens().at(0);
    point = widget->mapToGlobal(point);

    if(point.y() + height() > w->geometry().height())
        point.setY(point.y() - height() - 50);

    move(point);
}

void DockIdName::rowUp()
{
    Toolkit::rowUp(ui->tableWidget);
}

void DockIdName::rowDown()
{
    Toolkit::rowDown(ui->tableWidget);
}

bool DockIdName::tryClick()
{
    int row = Toolkit::currentRow(ui->tableWidget, row);

    if(row < 0)
        return false;

    emit clickedRow(ui->tableWidget->item(row, 0)->text().toInt());
    hide();
    return true;
}

void DockIdName::showDock()
{
    ui->tableWidget->clearContents();
    int row = 0;
    ui->tableWidget->setRowCount(m_filteredItems.count());

    for(QList<int>::const_iterator it = m_filteredItems.begin(); it != m_filteredItems.end(); it++) {
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(m_codeList.key(*it)));
        ui->tableWidget->setItem(row++, 1, new QTableWidgetItem(m_namesList.key(*it)));
    }

    ui->tableWidget->setCurrentCell(0, 1, QItemSelectionModel::Select);
    ui->tableWidget->setCurrentCell(0, 0, QItemSelectionModel::Select);
    show();
}

void DockIdName::makeIndexesList(const QMap<QString, int>& container, const QString &filter)
{
    m_filteredItems.clear();

    for(QMap<QString, int>::const_iterator it = container.begin(); it != container.end(); it++) {
        const QString &s = it.key();

        if(!s.mid(0, filter.length()).compare(filter, Qt::CaseInsensitive))
            m_filteredItems.append(it.value());
    }
}

void DockIdName::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    if(!index.isValid())
        return;

    QTableWidgetItem *item = ui->tableWidget->item(index.row(), 0);

    if(item) {
        hide();
        emit clickedRow(item->text().toInt());
        return;
    }
}

void DockIdName::on_actionActionKeyEnter_triggered()
{
    int row = Toolkit::currentRow(ui->tableWidget, row);

    if(row < 0)
        return;

    emit clickedRow(row);
}

bool DockIdName::eventFilter(QObject *object, QEvent *event)
{
    if(object == ui->tableWidget) {
        if(event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);

            switch(keyEvent->key()) {
            case Qt::Key_Up:
                Toolkit::rowUp(ui->tableWidget);
                break;

            case Qt::Key_Down:
                Toolkit::rowDown(ui->tableWidget);
                break;

            case Qt::Key_Return:
                tryClick();
                break;
            }
        }

        return false;
    }

    return QDockWidget::eventFilter(object, event);
}
