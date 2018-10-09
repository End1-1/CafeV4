#ifndef DLGVIEWIMAGE_H
#define DLGVIEWIMAGE_H

#include <QDialog>

namespace Ui {
class DlgViewImage;
}

class DlgViewImage : public QDialog
{
    Q_OBJECT

public:
    QString m_imgLink;
    explicit DlgViewImage(int itemId, const QString &img_link, int level, QWidget *parent = 0);
    ~DlgViewImage();

private:
    Ui::DlgViewImage *ui;
    QString m_itemId;
    QString m_imgLevel;
    QString m_imgPath;
    void uploadImage();
    void downloadImage();

private slots:
    void uploadResponse(const QString &data, bool isError);
    void downloadResponse(QByteArray &data, bool isError);
    void on_btnClose_clicked();
    void on_btnSave_clicked();
    void on_btnLoad_clicked();
};

#endif // DLGVIEWIMAGE_H
