#include "grselector.h"
#include "ui_grselector.h"
#include "sqlthread.h"
#include "grdb.h"
#include "grselectormodel.h"

GrSelector::GrSelector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrSelector)
{
    ui->setupUi(this);
}

GrSelector::~GrSelector()
{
    delete ui;
}

bool GrSelector::get(GrSelectorModel *sm)
{
    GrSelector *s = new GrSelector();
    s->mSelectorModel = sm;
    SqlThread *st = new SqlThread(_gr_host_, _gr_path_, _gr_user_, _gr_pass_, "lc_ctype=utf8");
    connect(st, SIGNAL(finished()), s, SLOT(sqlFinished()));
    if (s->exec() == QDialog::Accepted) {
        sm->mResult = true;
    }
    delete s;
    return sm->mResult;
}

void GrSelector::sqlFinished()
{
    SqlThread *st = static_cast<SqlThread*>(sender());
    ui->tblData->setRowCount(st->rowCount());
    st->deleteLater();
}
