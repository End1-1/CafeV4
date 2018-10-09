#include "dlgviewimage.h"
#include "ui_dlgviewimage.h"
#include "core.h"
#include "qnet.h"
#include <QFileDialog>
#include <QFileInfo>

DlgViewImage::DlgViewImage(int itemId, const QString &img_link, int level, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DlgViewImage)
{
    ui->setupUi(this);
    m_itemId = QString::number(itemId);
    m_imgLink = img_link;
    m_imgLevel = QString::number(level);
    ui->lbImageSize->setText("");
    downloadImage();
}

DlgViewImage::~DlgViewImage()
{
    delete ui;
}

void DlgViewImage::uploadImage()
{
    if (!m_imgPath.length())
        return;
    QFile f(m_imgPath);
    if (!f.open(QIODevice::ReadOnly))
        return;
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getResponse(QString,bool)), this, SLOT(uploadResponse(QString,bool)));
    n->URL = QSystem::WebUrl + "/menuimgup.php";

    n->addData("--" + boundary.toLatin1() + "\r\n");
    n->addData("phrase", QSystem::WebPass);
    n->addData("level", m_imgLevel);
    n->addData("item_id", m_itemId);

    n->addData(QString("Content-Disposition: form-data; name=\"file\"; filename=\"" + m_imgPath + "\";\r\n").toLatin1());
    n->addData("Content-Type: image/png\r\n");
    n->addData("Content-transfer-encoding: binary\r\n\r\n");
    n->addData(f.readAll() + "\r\n");
    n->addData("--" + boundary.toLatin1() + "--\r\n");

    n->go();
}

void DlgViewImage::downloadImage()
{
    if (!m_imgLink.length())
        return;
    QNet *n = new QNet(this);
    connect(n, SIGNAL(getRawResponse(QByteArray&,bool)), this, SLOT(downloadResponse(QByteArray&,bool)));
    n->downloadRawData(QSystem::WebUrl + "/" + m_imgLink);
}

void DlgViewImage::uploadResponse(const QString &data, bool isError)
{
    if (isError) {
        QMessageBox::critical(this, tr("Network error"), data);
        return;
    } else {
        m_imgLink = data;
        QMessageBox::information(this, tr("Information"), tr("Saved"));
    }
}

void DlgViewImage::downloadResponse(QByteArray &data, bool isError)
{
    QNet *n = static_cast<QNet*>(sender());
    if (isError) {
        QMessageBox::critical(this, tr("Network error"), data);
    } else {
        QPixmap p;
        p.loadFromData(data);
        QGraphicsScene *s = new QGraphicsScene();
        s->addPixmap(p);
        ui->grImage->setScene(s);
        ui->grImage->fitInView(s->itemsBoundingRect(), Qt::KeepAspectRatio);
        ui->lbImageSize->setText(QString("%1 Kb").arg(data.size() / 1000));
    }
    n->deleteLater();
}

void DlgViewImage::on_btnClose_clicked()
{
    accept();
}

void DlgViewImage::on_btnSave_clicked()
{
    uploadImage();
}

void DlgViewImage::on_btnLoad_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select image"), "", "*.png;*.jpeg;*.jpg");
    if (!fileName.length())
        return;
    m_imgPath = fileName;
    QPixmap pixmap(fileName);
    QGraphicsScene *s = new QGraphicsScene();
    s->addPixmap(pixmap);
    ui->grImage->setScene(s);
    ui->grImage->fitInView(s->itemsBoundingRect(), Qt::KeepAspectRatio);
    ui->lbImageSize->setText(QString("%1 Kb").arg(QFileInfo(QFile(fileName)).size() / 1000));
}
