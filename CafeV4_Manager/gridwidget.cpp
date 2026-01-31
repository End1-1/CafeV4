#include "gridwidget.h"
#include "ui_gridwidget.h"
#include "grideditordialog.h"
#include "grideditorwidget.h"
#include <QScrollBar>
#include <QSettings>
#include <QTimer>
#include <QAction>

GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GridWidget)
{
    ui->setupUi(this);
    mEditorWidget = nullptr;
    hideTotalTable(true);
    mModel = new GridTableModel(mDb.mDbRows, mDb.mColumns);
    ui->tblData->setModel(mModel);
    ui->wQuickSearch->setVisible(false);
    mDb.setDatabase(_gr_host_, _gr_path_, _gr_user_, _gr_pass_, "lc_ctype=utf8");
    connect(&mDb, SIGNAL(finished()), this, SLOT(queryFinished()));
    connect(ui->tblData->horizontalHeader(), SIGNAL(sectionResized(int, int, int)), this, SLOT(mainTableSectionResized(int, int, int)));
    connect(ui->tblData->horizontalHeader(), SIGNAL(sectionClicked(int)), this, SLOT(mainTableSectionClicked(int)));
    connect(ui->tblTotal->horizontalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(mainTableValueChanged(int)));
}

GridWidget::~GridWidget()
{
    if(mEditorWidget) {
        mEditorWidget->deleteLater();
    }

    delete ui;
}

void GridWidget::go()
{
    QString select;
    QStringList leftJoin;
    QMap<QString, QString> translation;

    for(GridColumn &g : mColumns) {
        if(!g.mSelect) {
            continue;
        }

        if(select.isEmpty()) {
            select += "select ";
        } else {
            select += ", ";
        }

        select += g.mField;

        if(!g.mLeftJoinTable.isEmpty()) {
            if(!leftJoin.contains(g.mLeftJoinTable)) {
                leftJoin.append(g.mLeftJoinTable);
            }
        }

        if(!g.mLeftJoinRelation.isEmpty()) {
            if(!leftJoin.contains(g.mLeftJoinRelation)) {
                leftJoin.insert(0, g.mLeftJoinRelation);
            }
        }

        QString trField = g.mField;
        int pos = trField.indexOf(" as ", 0, Qt::CaseInsensitive);

        if(pos > -1) {
            trField = trField.mid(pos + 4, trField.length() - pos);
        } else {
            pos = trField.indexOf(".", 0, Qt::CaseInsensitive);

            if(pos > -1) {
                trField = trField.mid(pos + 1, trField.length() - pos);
            }
        }

        translation[trField] = g.mTranslation;
    }

    QString query = select + " from " + mMainTable + " ";

    for(const QString &s : leftJoin) {
        query += mLeftJoinTables[s] + " ";
    }

    mModel->setTranslation(translation);
    mDb.setSqlQuery(query);
    mDb.start();
}

void GridWidget::hideTotalTable(bool hidden)
{
    ui->tblTotal->setVisible(!hidden);
    ui->tblData->setHorizontalScrollBarPolicy(hidden ? Qt::ScrollBarAsNeeded : Qt::ScrollBarAlwaysOff);
}

void GridWidget::showMenu()
{
    mMenu.popup(cursor().pos());
}

void GridWidget::search()
{
    ui->wQuickSearch->setVisible(true);
    ui->leQuickSearch->setFocus();
    ui->leQuickSearch->setSelection(0, ui->leQuickSearch->text().length());
}

void GridWidget::escapeKey()
{
    if(ui->wQuickSearch->isVisible()) {
        ui->wQuickSearch->setVisible(false);
    }
}

void GridWidget::queryFinished()
{
    mModel->reset();
    QStringList totalHeader;
    totalHeader.append(QString::number(mModel->rowCount()));
    ui->tblTotal->setColumnCount(mModel->columnCount());
    ui->tblTotal->setVerticalHeaderLabels(totalHeader);
    restoreColumnsWidths();
}

void GridWidget::mainTableSectionResized(int logicalIndex, int oldSize, int newSize)
{
    Q_UNUSED(oldSize);
    ui->tblTotal->setColumnWidth(logicalIndex, newSize);
    QString columnName = mModel->nameForColumnIndex(logicalIndex);
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\columnswidth")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(mReportTitle));
    s.setValue(columnName, newSize);
}

void GridWidget::mainTableSectionClicked(int logicalIndex)
{
    mModel->sort(logicalIndex);
}

void GridWidget::mainTableValueChanged(int value)
{
    ui->tblData->horizontalScrollBar()->setValue(value);
}

void GridWidget::newItem()
{
    if(mEditorWidget) {
        GridEditorDialog *gw = new GridEditorDialog(mEditorWidget, this);

        while(gw->exec() == QDialog::Accepted) {
            if(!gw->mFlagNew) {
                break;
            }
        }

        mEditorWidget->setParent(nullptr);
        delete gw;
    }
}

void GridWidget::editItem()
{
    if(mEditorWidget) {
        GridEditorDialog *gw = new GridEditorDialog(mEditorWidget, this);
        mEditorWidget->load(getItemID());

        while(gw->exec() == QDialog::Accepted) {
            if(!gw->mFlagNew) {
                break;
            }
        }

        mEditorWidget->setParent(nullptr);
        delete gw;
    }
}

void GridWidget::setTitle(const QString &title)
{
    mReportTitle = title;
}

void GridWidget::prepare()
{
}

void GridWidget::restoreColumnsWidths()
{
    QSettings s(_ORGANIZATION_, QString("%1\\%2\\reports\\%3\\columnswidth")
                .arg(_APPLICATION_)
                .arg(_MODULE_)
                .arg(mReportTitle));

    for(int i = 0; i < mModel->columnCount(); i++) {
        if(s.contains(mModel->nameForColumnIndex(i))) {
            ui->tblData->setColumnWidth(i, s.value(mModel->nameForColumnIndex(i)).toInt());
        }
    }
}

int GridWidget::getItemID()
{
    QModelIndexList il = ui->tblData->selectionModel()->selectedIndexes();

    if(il.count() > 0) {
        return mModel->data(il.at(0).row(), 0, Qt::EditRole).toInt();
    }

    return 0;
}

void GridWidget::addColumn(const QString &field, const QString &translation, bool select, const QString &leftJoin, const QString &leftJoinRelation)
{
    GridColumn g;
    g.mField = field;
    g.mTranslation = translation;
    g.mSelect = select;
    g.mLeftJoinTable = leftJoin;
    g.mLeftJoinRelation = leftJoinRelation;
    mColumns.append(g);
}

GridTableModel::GridTableModel(QList<QList<QVariant> >& data, QMap<QString, int>& indexOfField) :
    mData(data),
    mIndexOfField(indexOfField)
{
}

QVariant GridTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role) {
    case Qt::DisplayRole:
        if(orientation == Qt::Vertical) {
            return section + 1;
        } else {
            return mTranslation.contains(mFieldIndex[section]) ? mTranslation[mFieldIndex[section]] : mFieldIndex[section];
        }

    case Qt::DecorationRole:
        if(orientation == Qt::Horizontal) {
//            if (fFilters.contains(section)) {
//                return QIcon(":/filter_set.png");
//            }
        }
    }

    return QVariant();
}

int GridTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mProxyRows.count();
}

int GridTableModel::rowCount() const
{
    return mProxyRows.count();
}

int GridTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return mIndexOfField.count();
}

int GridTableModel::columnCount() const
{
    return mIndexOfField.count();
}

QVariant GridTableModel::data(const QModelIndex &index, int role) const
{
    return data(index.row(), index.column(), role);
}

QVariant GridTableModel::data(int row, int column, int role) const
{
    row = mProxyRows[row];

    switch(role) {
    case Qt::DisplayRole: {
        QVariant &v = mData[row][column];

        switch(v.type()) {
        case QVariant::Int:
            return v.toString();

        case QVariant::Date:
            return v.toDate().toString(FORMAT_DATE_TO_STR);

        case QVariant::DateTime:
            return v.toDateTime().toString(FORMAT_DATETIME_TO_STR);

        case QVariant::Time:
            return v.toTime().toString(FORMAT_TIME_TO_STR);

        case QVariant::Double:
            return double_str(v.toDouble(), 4);

        default:
            return v.toString();
        }

        break;
    }

    case Qt::EditRole:
        return mData[row][column];
    }

    return QVariant();
}

void GridTableModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(order);

    if(mLastSortedColumn == column) {
        mSortAsc = !mSortAsc;
    } else {
        mSortAsc = true;
    }

    mLastSortedColumn = column;
    QMap<QVariant, int> data;

    foreach(int i, mProxyRows) {
        data.insertMulti(mData[i][column], i);
    }

    beginResetModel();
    mProxyRows = data.values();

    if(!mSortAsc) {
        std::reverse(mProxyRows.begin(), mProxyRows.end());
    }

    endResetModel();
}

void GridTableModel::sort(int column)
{
    sort(column, Qt::AscendingOrder);
}

QString GridTableModel::nameForColumnIndex(int index) const
{
    return mFieldIndex[index];
}

void GridTableModel::setTranslation(const QMap<QString, QString>& translation)
{
    mTranslation = translation;
}

void GridTableModel::setFilter(int column, const QString &pattern)
{
    mFilters[column] = pattern;
    mProxyRows.clear();

    for(int i = 0; i < mData.count(); i++) {
        mProxyRows << i;
    }

    filterData();
}

void GridTableModel::reset()
{
    QStringList fields = mIndexOfField.keys();

    for(const QString s : fields) {
        mFieldIndex[mIndexOfField[s]] = s;
    }

    beginResetModel();
    mProxyRows.clear();

    for(int i = 0; i < mData.count(); i++) {
        mProxyRows.append(i);
    }

    endResetModel();
}

void GridTableModel::filterData()
{
    beginResetModel();
    QList<int> ps;
    QList<int> columns = mFilters.keys();

    if(columns.contains(-1)) {
        columns.removeFirst();
    }

    QMap<int, QStringList> filter;

    for(QMap<int, QString>::const_iterator it = mFilters.begin(); it != mFilters.end(); it++) {
        filter[it.key()] = it.value().split("|", Qt::SkipEmptyParts);

        if(filter[it.key()].count() == 0) {
            filter.remove(it.key());
        }
    }

    for(int r = 0, count = mData.count(); r < count; r++) {
        int row = r;
        bool found = filter.count() == 0;

        if(found) {
            goto FOUND;
        }

        if(mFilters.contains(-1)) {
            for(int c = 0; c < mFieldIndex.count(); c++) {
                foreach(QString searchStr, filter[-1]) {
                    if(data(row, c, Qt::DisplayRole).toString().contains(searchStr, Qt::CaseInsensitive)) {
                        found = true;
                        goto FOUND;
                    }
                }
            }
        }

        found = columns.count() > 0;

        foreach(int col, columns) {
            bool found2 = false;

            foreach(QString searchStr, filter[col]) {
                found2 = found2 || data(row, col, Qt::DisplayRole).toString().contains(searchStr, Qt::CaseInsensitive);

                if(found2) {
                    break;
                }
            }

            found = found && found2;

            if(!found) {
                goto FOUND;
            }
        }

FOUND:

        if(found) {
            ps.append(row);
        }
    }

    mProxyRows = ps;
    endResetModel();
}

void GridWidget::on_leQuickSearch_textChanged(const QString &arg1)
{
    mModel->setFilter(-1, arg1);
}

void GridWidget::on_tblData_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    editItem();
}
