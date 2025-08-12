#ifndef DOWNLOADFRAME_H
#define DOWNLOADFRAME_H

#include <QFrame>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>

namespace Ui {
class DownloadFrame;
}

class DownloadFrame : public QFrame
{
    Q_OBJECT

public:
    explicit DownloadFrame(QString title, QWidget *parent = nullptr);
    ~DownloadFrame();

    void downloadFile(QUrl downloadedUrl, QString filePath);
    QString formatFileSize(qint64 bytes);

signals:
    void downloadFinished(bool isSuccess); //下载结束信号

private:
    Ui::DownloadFrame *ui;

    QNetworkAccessManager *manager;
    QNetworkRequest request;
    QNetworkReply *reply;
    QFile *downloadedFile;
};

#endif // DOWNLOADFRAME_H
