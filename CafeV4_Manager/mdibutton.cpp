#include "mdibutton.h"
#include "ui_mdibutton.h"
#include "core.h"

MdiButton::MdiButton(QWidget *parent, const QString &text) :
    QWidget(parent),
    ui(new Ui::MdiButton)
{
    ui->setupUi(this);
    m_widget = parent;
    connect(m_widget, SIGNAL(destroyed()), SLOT(widgetDestroyed()));
    setText(text);
    ___toolBar->addWidget(this);
}

MdiButton::~MdiButton()
{
    delete ui;
}

void MdiButton::setText(const QString &text)
{
    ui->btnClick->setText(text);
    ui->btnClick->setMinimumWidth(QFontMetrics(qApp->font()).width(text));
}

void MdiButton::setIcon(const QIcon &icon)
{
    ui->btnClick->setIcon(icon);
}

void MdiButton::widgetDestroyed(QObject *object)
{
    Q_UNUSED(object)
    delete this;
}

void MdiButton::on_btnClose_clicked()
{
    ___mdiArea->removeSubWindow(qobject_cast<QWidget*>(m_widget->parent()));
    delete m_widget->parent();
    //delete m_widget;
}

void MdiButton::on_btnClick_clicked()
{
    m_widget->setWindowState((m_widget->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    m_widget->activateWindow();
    m_widget->setFocus();
}
