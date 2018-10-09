#include "dlgprogress.h"
#include "ui_dlgprogress.h"
#include "core.h"

DlgProgress::DlgProgress(QWidget *parent) :
    QDialog(parent, Qt::FramelessWindowHint),
    ui(new Ui::DlgProgress)
{
    ui->setupUi(this);
    m_timer = new QTimer();
    m_timer->start(1000);
    connect(m_timer, SIGNAL(timeout()), SLOT(timer()));
}

DlgProgress::~DlgProgress()
{
    m_timer->stop();
    delete m_timer;
    delete ui;
}

void DlgProgress::show(const QString &text)
{
    m_time = QTime::fromString("00:00:00", TIME_FORMAT);
    ui->lbTime->setText(m_time.toString(TIME_FORMAT));
    ui->label->setText(text);
    QDialog::show();
}

void DlgProgress::timer()
{
    m_time = m_time.addSecs(1);
    ui->lbTime->setText(m_time.toString(TIME_FORMAT));
    qApp->processEvents();
}
