#include "foodgridwidget.h"
#include "ui_foodgridwidget.h"

FoodGridWidget::FoodGridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FoodGridWidget)
{
    ui->setupUi(this);
    ui->tableGrid->installEventFilter(this);

    m_tableLineedit = 0;
    m_dockIdName = new DockIdName(tr("Goods"), this);
    m_dockIdName->hide();
    connect(m_dockIdName, SIGNAL(clickedRow(int)), SLOT(filteredRow(int)));

    m_columnForQty = 2;
}

FoodGridWidget::~FoodGridWidget()
{
    delete ui;
}

void FoodGridWidget::setTableTitle(const QStringList &labels, const QList<int> &widths)
{
    ui->tableGrid->setColumnCount(labels.count());
    ui->tableGrid->setHorizontalHeaderLabels(labels);
    int col = 0;
    for (QList<int>::const_iterator it = widths.begin(); it != widths.end(); it++)
        ui->tableGrid->setColumnWidth(col++, *it);
}

void FoodGridWidget::setTableColumnValidator(const int &column, int validator)
{
    m_columnValidator[column] = validator;
}

void FoodGridWidget::setTableModel(QMap<QString, QStringList> *model)
{
    m_model = model;
    ui->tableGrid->setEnabled(false);
    ui->tableGrid->clearContents();
    ui->tableGrid->setEnabled(true);
    if (model->values().count())
        ui->tableGrid->setRowCount(model->values().at(0).count());
    else
        ui->tableGrid->setRowCount(0);
    for (int i = 0, rowCount = ui->tableGrid->rowCount(); i < rowCount; i++)
        for (int j = 0, columnCount = ui->tableGrid->columnCount(); j < columnCount; j++) {
            ui->tableGrid->setItem(i, j, new QTableWidgetItem((*m_model)[ui->tableGrid->horizontalHeaderItem(j)->text()].at(i)));
        }
}

void FoodGridWidget::setDockIdNameColumns(const int &id, const int &name)
{
    m_columnForId = id;
    m_columnForName = name;
}

void FoodGridWidget::setDockIdNameLists(QStringList *ids, QStringList *names)
{
    m_idsList = ids;
    m_namesList = names;
    m_dockIdName->setCodeNameList(*m_idsList, *m_namesList);
}

void FoodGridWidget::clearRows()
{
    ui->tableGrid->setRowCount(0);
}

void FoodGridWidget::updateRow(const int &row)
{
    for (int i = 0, count = m_model->count(); i < count; i++) {
        QMap<QString, QStringList>::iterator it = m_model->find(ui->tableGrid->horizontalHeaderItem(i)->data(Qt::DisplayRole).toString());
        ui->tableGrid->item(row, i)->setData(Qt::DisplayRole, it.value().at(row));
    }

    if (m_tableLineedit) {
        QModelIndex index = ui->tableGrid->indexAt(m_tableLineedit->pos());
        bool m_dockState = m_dockIdName->isVisible();
        //disconnect(m_tableLineedit, SIGNAL(textChanged(QString)), this, SLOT(tableCellWidgetTextChanged(QString)));
        //m_tableLineedit->setText(ui->tableGrid->item(index.row(), index.column())->data(Qt::DisplayRole).toString());
        //connect(m_tableLineedit, SIGNAL(textChanged(QString)), SLOT(tableCellWidgetTextChanged(QString)));
        m_dockIdName->setVisible(m_dockState);
    }
}

int FoodGridWidget::tableCurrentRow() const
{
    int row = Toolkit::currentRow(ui->tableGrid, row);
    return row;
}

QTableWidget *FoodGridWidget::table()
{
    return ui->tableGrid;
}

void FoodGridWidget::setRowColor(const int &col, const QString &match, const QColor &color)
{
    if (match == "") {
        for (int i = 0, colCount = ui->tableGrid->rowCount(); i < colCount; i++)
            for (int j = 0, colCount = ui->tableGrid->columnCount(); j < colCount; j++)
                ui->tableGrid->item(i, j)->setData(Qt::BackgroundColorRole, QColor::fromRgb(255, 255, 255));
        return;
    }

    for (int i = 0, count = ui->tableGrid->rowCount(); i < count; i++)
        if (ui->tableGrid->item(i, col)->data(Qt::DisplayRole).toString() == match) {
                for (int j = 0, colCount = ui->tableGrid->columnCount(); j < colCount; j++)
                    ui->tableGrid->item(i, j)->setData(Qt::BackgroundColorRole, color);
        }
}

void FoodGridWidget::removeRows(const int &col, const QString &match)
{
    for (int i = ui->tableGrid->rowCount() - 1; i > -1; i--)
        if (ui->tableGrid->item(i, col)->data(Qt::DisplayRole).toString() == match)
            removeRow(i);
}

void FoodGridWidget::removeRow(const int &row)
{
    ui->tableGrid->removeRow(row);
    for (int i = 0, count = ui->tableGrid->columnCount(); i < count; i++)
        (*m_model)[ui->tableGrid->horizontalHeaderItem(i)->text()].removeAt(row);
}

void FoodGridWidget::setCurrentRow(const int &row)
{
    ui->tableGrid->setCurrentCell(row, 0);
}

int FoodGridWidget::rowCount()
{
    return ui->tableGrid->rowCount();
}

void FoodGridWidget::setData(const int &row, const int &col, const QVariant &data, int role)
{
    QTableWidgetItem *item = ui->tableGrid->item(row, col);
    if (!item)
        return;
    item->setData(role, data);
}

QPushButton *FoodGridWidget::btnNew()
{
    return ui->btnNewObject;
}

bool FoodGridWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (object == m_tableLineedit) {
           switch (keyEvent->key()) {
            case Qt::Key_Up:
                if (m_dockIdName->isVisible())
                    m_dockIdName->rowUp();
                else
                    Toolkit::rowUp(ui->tableGrid);
                return true;
            case Qt::Key_Down:
                if (m_dockIdName->isVisible())
                    m_dockIdName->rowDown();
                else
                    Toolkit::rowDown(ui->tableGrid);
                return true;
            case Qt::Key_Left:
                m_dockIdName->hide();
                Toolkit::columnLeft(ui->tableGrid);
                return true;
            case Qt::Key_Right:
                m_dockIdName->hide();
                Toolkit::columnRight(ui->tableGrid);
                return true;
            case Qt::Key_Return:
            case Qt::Key_Enter:
                if (m_dockIdName->isVisible()) {
                    m_dockIdName->tryClick();
                } else {
                    QModelIndex index = ui->tableGrid->indexAt(m_tableLineedit->pos());
                    if (m_tableLineedit) {
                        ui->tableGrid->removeCellWidget(index.row(), index.column());
                        removeLineEdit();
                        m_dockIdName->hide();
                    } else {
                        makeLineEdit(index.row(), index.column());
                    }
                }
                return true;
            }
        } else if (object == ui->tableGrid) {
            if (event->type() == QEvent::KeyPress) {
                QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
                switch (keyEvent->key()) {
                    Toolkit::rowUp(ui->tableGrid);
                    return true;
                case Qt::Key_Down:
                    Toolkit::rowDown(ui->tableGrid);
                    return true;
                case Qt::Key_Left:
                    Toolkit::columnLeft(ui->tableGrid);
                    return true;
                case Qt::Key_Right:
                    Toolkit::columnRight(ui->tableGrid);
                    return true;
                case Qt::Key_Return:
                case Qt::Key_Enter: {
                    int row = ui->tableGrid->currentRow();
                    int col = ui->tableGrid->currentColumn();
                    if (col == m_columnForQty) {
                        if (row == ui->tableGrid->rowCount() - 1) {
                            newTableRow();
                            ui->tableGrid->setCurrentCell(row + 1, m_columnForName);
                            ui->tableGrid->setFocus();
                        }
                    } else {
                        if (m_tableLineedit) {
                            ui->tableGrid->removeCellWidget(row, col);
                            removeLineEdit();
                            m_dockIdName->hide();
                        } else
                            makeLineEdit(row, col);
                    }
                    return true;
                    }
                }
            }
        }
    }
    return QWidget::eventFilter(object, event);
}

void FoodGridWidget::on_pushButton_clicked()
{
    newTableRow();
    ui->tableGrid->setCurrentCell(ui->tableGrid->rowCount() - 1, m_columnForName);
}

int FoodGridWidget::newTableRow()
{
    int row = ui->tableGrid->rowCount();
    ui->tableGrid->setRowCount(row + 1);
    for (int i = 0, count = ui->tableGrid->columnCount(); i < count; i++) {
        (*m_model)[ui->tableGrid->horizontalHeaderItem(i)->text()].append("");
        ui->tableGrid->setItem(row, i, new QTableWidgetItem(""));
    }
    return row;
}

void FoodGridWidget::makeLineEdit(const int &row, const int &column)
{
    if (row < 0 || column < 0)
        return;

    QTableWidgetItem *item = ui->tableGrid->item(row, column);
    if (item) {
        if (item->data(Qt::UserRole + 1).toInt() == 1)
            return;
    } else
        return;

    m_tableLineedit = new QLineEdit(ui->tableGrid);
    m_tableLineedit->setFrame(false);
    m_tableLineedit->installEventFilter(this);
    int validator = 0;
    if (m_columnValidator.contains(column))
        validator = m_columnValidator[column];

    switch (validator) {
    case 0:
        break;
    case 1:
        m_tableLineedit->setValidator(new QIntValidator());
        break;
    case 2: {
        QDoubleValidator *d = new QDoubleValidator(0, 1000000, 3, m_tableLineedit);
        d->setNotation(QDoubleValidator::StandardNotation);
        m_tableLineedit->setValidator(d);
        break;
    }
    case 3:
        removeLineEdit();
        return;
    }

    if (column == m_columnForId) {
        m_dockIdName->setPos(m_tableLineedit);
        m_dockIdName->setCodeFilter("");
    }
    if (column == m_columnForName) {
        m_dockIdName->setPos(m_tableLineedit);
        m_dockIdName->setNameFilter("");
    }

    if (item)
        m_tableLineedit->setText(item->data(Qt::DisplayRole).toString());
    ui->tableGrid->setCellWidget(row, column, m_tableLineedit);
    m_tableLineedit->setFocus();
    m_tableLineedit->setSelection(0, m_tableLineedit->text().length());

    connect(m_tableLineedit, SIGNAL(textChanged(QString)), SLOT(tableCellWidgetTextChanged(QString)));
}

void FoodGridWidget::removeLineEdit()
{
    disconnect(m_tableLineedit, SIGNAL(textChanged(QString)), this, SLOT(tableCellWidgetTextChanged(QString)));
    delete m_tableLineedit;
    m_tableLineedit = 0;
    ui->tableGrid->setFocus();
}

void FoodGridWidget::tableCellWidgetTextChanged(const QString &str)
{
    QLineEdit *l = qobject_cast<QLineEdit*>(sender());
    QModelIndex index = ui->tableGrid->indexAt(l->pos());
    QString column = ui->tableGrid->horizontalHeaderItem(index.column())->text();

    if (index.column() == m_columnForId) {
        m_dockIdName->setPos(m_tableLineedit);
        m_dockIdName->setCodeFilter(str);
    }
    if (index.column() == m_columnForName) {
        m_dockIdName->setPos(m_tableLineedit);
        m_dockIdName->setNameFilter(str);
    }

    emit cellTextChanged(column, index.row(), str);
}

void FoodGridWidget::on_tableGrid_currentCellChanged(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
    if (m_tableLineedit) {
        ui->tableGrid->removeCellWidget(previousRow, previousColumn);
        removeLineEdit();
    }
    m_dockIdName->hide();
    makeLineEdit(currentRow, currentColumn);
}

void FoodGridWidget::filteredRow(int row)
{
    int currentRow = Toolkit::currentRow(ui->tableGrid, currentRow);
    if (m_tableLineedit)
        m_tableLineedit->setSelection(0, m_tableLineedit->text().length());
    ui->tableGrid->setCurrentCell(currentRow, m_columnForQty, QItemSelectionModel::Select);
    emit selectedItem(row);
}

void FoodGridWidget::on_pushButton_2_clicked()
{
    int row = Toolkit::currentRow(ui->tableGrid, row);
    if (row < 0)
        return;

    removeRow(row);
    emit rowRemoved(row);
}

void FoodGridWidget::on_tableGrid_clicked(const QModelIndex &index)
{
    if (!m_tableLineedit)
            makeLineEdit(index.row(), index.column());
}
