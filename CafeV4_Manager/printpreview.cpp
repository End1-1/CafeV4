#include "printpreview.h"
#include "ui_printpreview.h"
#include <QPrinter>
#include <QPrintDialog>
#include "storeorder.h"
#include "../core/printing.h"

PrintPreview::PrintPreview(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PrintPreview)
{
    ui->setupUi(this);
    m_currentPage = 0;
   // MdiButton *mdiButton = new MdiButton(this, tr("Print preview"));
   // Q_UNUSED(mdiButton);
    //___mdiArea->addSubWindow(this);
}

PrintPreview::~PrintPreview()
{
    delete ui;
}

void PrintPreview::setPaintData(const XmlPrintMaker xpm, int page)
{
    m_sm.reset();
    m_xpm.setSizeMetrics(&m_sm);
    m_xpm = xpm;
    XmlGraphicItem *i = new XmlGraphicItem(0);
    i->setPaintData(m_xpm.page(page), &m_sm);
    QGraphicsScene *scene = new QGraphicsScene(0, 0, ui->view->width(), ui->view->height());
    ui->view->setScene(scene);
    ui->view->scene()->addItem(i);
    setPageLabel();
}

void PrintPreview::on_actionPrint_triggered()
{
    QPrinter prn;
    QPrinter::Margins m;
    m.left = 5;
    m.right = 5;
    m.top = 5;
    m.bottom = 5;
    if (m_lastPrinter.length())
        prn.setPrinterName(m_lastPrinter);
    prn.setPageSize(QPrinter::A4);
    prn.setOrientation(QPrinter::Portrait);
    prn.setMargins(m);
    QPrintDialog pd(&prn);
    if (m_lastPrinter.length())
        pd.printer()->setPrinterName(m_lastPrinter);
    if (pd.exec() != QDialog::Accepted)
        return;

    m_lastPrinter = prn.printerName();
    if (m_xpm.pageWidth() > XmlPrintMaker::m_pageWidthProtrate)
        prn.setOrientation(QPrinter::Landscape);

    qreal left, top, right, bottom;
    prn.getPageMargins(&left, &top, &right, &bottom, QPrinter::Millimeter);
    SizeMetrics sm(prn.resolution());
    QPainter painter(&prn);
    XmlPrinter xPrn(&painter, &sm);
    xPrn.setMargins(left, top);

    for (int i = 0, count = m_xpm.pageCount(); i < count; i++) {
        if (i)
            prn.newPage();
        xPrn.render(m_xpm.page(i));
    }
}

void PrintPreview::on_actionZoom_in_triggered()
{
    ui->view->scale(1.5, 1.5);
}

void PrintPreview::on_actionZoom_out_triggered()
{
    ui->view->scale(1 / 1.5, 1 / 1.5);
}

void PrintPreview::on_actionPrevious_page_triggered()
{
    if (m_currentPage == 0)
        return;
    m_currentPage--;
    setPaintData(m_xpm, m_currentPage);
    setPageLabel();
}

void PrintPreview::on_actionNext_page_triggered()
{
    if (m_currentPage == m_xpm.pageCount() - 1)
        return;
    m_currentPage++;
    setPaintData(m_xpm, m_currentPage);
    setPageLabel();
}

void PrintPreview::setPageLabel()
{
    ui->lbPage->setText(QString(tr("Page %1 of %2")).arg(m_currentPage + 1).arg(m_xpm.pageCount()));
}
