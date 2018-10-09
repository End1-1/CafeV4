#ifndef PRINTPREVIEW_H
#define PRINTPREVIEW_H

#include "core.h"

namespace Ui {
class PrintPreview;
}

class PrintPreview : public QMainWindow
{
    Q_OBJECT

private:
    int m_currentPage;
    void setPageLabel();
    SizeMetrics m_sm;
    XmlPrintMaker m_xpm;
    
public:
    explicit PrintPreview(QWidget *parent);
    ~PrintPreview();
    void setPaintData(const XmlPrintMaker xpm, int page);
    QString m_lastPrinter;
    
private slots:
    void on_actionPrint_triggered();
    void on_actionZoom_in_triggered();
    void on_actionZoom_out_triggered();
    void on_actionPrevious_page_triggered();
    void on_actionNext_page_triggered();

private:
    Ui::PrintPreview *ui;
};

#endif // PRINTPREVIEW_H
