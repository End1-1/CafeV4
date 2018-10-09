#include "cgridwidget.h"
#include "ui_cgridwidget.h"
#include "ctablemodel.h"
#include "cdatabase.h"
#include "cfilterdialog.h"
#include "ceditdialog.h"

#ifdef QT_DEBUG
#include <QDebug>
#endif

QString __DBPATH__;
QString __DBUSER__;
QString __DBPASS__;

CGridWidget::CGridWidget(const QString &guid, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CGridWidget)
{
    Q_ASSERT(!guid.isEmpty());
    ui->setupUi(this);
    fTableModel = new CTableModel();
    ui->tblData->setModel(fTableModel);
    fDatabase = new CDatabase(__DBPATH__, __DBUSER__, __DBPASS__, "lc_ctype=UTF8");
    connect(fDatabase, SIGNAL(dataLoaded(CDatabase*)), fTableModel, SLOT(dataLoaded(CDatabase*)));
    ui->tblTotal->setVisible(false);
    connect(ui->tblData->horizontalHeader(), &QHeaderView::sectionClicked, [this](int logicalIndex) {
       fTableModel->sort(logicalIndex);
    });
    ui->tblData->horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tblData->horizontalHeader(), &QHeaderView::customContextMenuRequested, [this](const QPoint &point) {
        filterRequest(ui->tblData->horizontalHeader()->logicalIndexAt(point));
    });
    fGuid = guid;

    fePrimaryKeyName = "ID";
}

CGridWidget::~CGridWidget()
{
    delete ui;
    delete fDatabase;
    delete fTableModel;
    for (QMap<int, CFilterDialog*>::iterator it = fFilters.begin(); it != fFilters.end(); it++) {
        delete it.value();
    }
}

void CGridWidget::setQuery(const QString &query)
{
    fQuery = query;
    fDatabase->exec(fQuery);
    for (QMap<QString, int>::const_iterator it = fColumnWidths.begin(); it != fColumnWidths.end(); it++) {
        int colIndex = fTableModel->columnIndexByName(it.key());
        ui->tblData->setColumnWidth(colIndex, it.value());
        ui->tblTotal->setColumnWidth(colIndex, it.value());
    }
    for (QMap<int, CFilterDialog*>::const_iterator it = fFilters.begin(); it != fFilters.end(); it++) {
        it.value()->clearValues();
    }
}

CGridWidget &CGridWidget::setColumnName(const QString &field, const QString &name, int width)
{
    fTableModel->setColumnNameMap(field, name);
    fColumnWidths[field.toUpper()] = width;
    return *this;
}

void CGridWidget::filterRequest(int i)
{
    CFilterDialog *d = 0;
    if (!fFilters.contains(i)) {
        d = new CFilterDialog(fTableModel->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString(), this);
        fFilters[i] = d;
    } else {
        d = fFilters[i];
    }
    if (d->isEmpty()) {
        QList<QStringList> filterData;
        QStringList oneFilter;
        fTableModel->getUniqueValuesForColumn(i, oneFilter);
        filterData.append(oneFilter);
        d->setFilterValues(filterData);
    }
    if (d->getFilter()) {
        fTableModel->search(i, d->fResult);
    }
}

void CGridWidget::endOfLoadData()
{
    QStringList l;
    l << QString::number(fTableModel->rowCount());
    ui->tblTotal->setVerticalHeaderLabels(l);
}

void CGridWidget::on_leQuickSearch_textChanged(const QString &arg1)
{
    fTableModel->search(arg1);
}

void CGridWidget::on_tblData_doubleClicked(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }
    if (!fSlaveTableKey.contains(index.column())) {
        return;
    }
    QVariant value = index.data();
    if (CEditDialog::setValue(fePrimaryKeyName, fTableModel->data(index.row(), 0).toString(),
                              feMasterTableName, fMasterTableKey[index.column()], fTableModel->headerData(index.column(), Qt::Horizontal, Qt::DisplayRole).toString(),
                              value, CEditDialog::FIELD_ID_TEXT,
                              fSlaveTableName[index.column()], fSlaveTableKey[index.column()], fSlaveTableValue[index.column()])) {
        fTableModel->setData(index, value);
    }
}
