#include "downloadframe.h"
#include "ui_downloadframe.h"

DownloadFrame::DownloadFrame(QString title, QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::DownloadFrame)
    , manager(new QNetworkAccessManager(this))
{
    ui->setupUi(this);

    this->setWindowTitle(title);
}

DownloadFrame::~DownloadFrame()
{
    delete ui;
}



void DownloadFrame::downloadFile(QUrl downloadedUrl, QString filePath) //下载文件
{
    QFile *file = new QFile(filePath);
    if(!file->open(QIODevice::WriteOnly)){
        qDebug() <<"打开文件失败: " << file->errorString();
        emit downloadFinished(false); //发送失败信号
        return;
    }

    //获取请求
    request.setUrl(downloadedUrl);
    reply = manager->get(request);

    connect(reply, &QNetworkReply::downloadProgress, this, [this, file](qint64 bytesReceived, qint64 bytesTotal){
        ui->progressBar->setMaximum(bytesTotal);
        ui->progressBar->setValue(bytesReceived);
        ui->label_download_msg->setText(QString("下载进度: %1/%2").arg(formatFileSize(bytesReceived)).arg(formatFileSize(bytesTotal)));
    });

    //写入文件
    connect(reply, &QNetworkReply::readyRead, this, [this, file](){
        file->write(reply->readAll());
    });

    //结束下载
    connect(reply, &QNetworkReply::finished, this, [this, file](){
        if(reply->error() == QNetworkReply::NoError){
            file->write(reply->readAll());
            file->close();
            QMessageBox::information(this, "下载信息", "下载成功");
            emit downloadFinished(true); //成功
        }
        else{
            qDebug() <<"下载失败: "<<reply->errorString();
            QMessageBox::warning(this, "下载信息", QString("下载失败: %1").arg(reply->errorString()));
            emit downloadFinished(false); //失败
        }

        file->deleteLater();
        reply->deleteLater();
    });
}

QString DownloadFrame::formatFileSize(qint64 bytes) //将qint64转为文件大小格式
{
    qint64 KB = 1024;
    qint64 MB = KB * 1024;
    qint64 GB = MB * 1024;
    qint64 TB = GB * 1024;

    if(bytes >= TB)
        return QString("%1 TB").arg(QString::number(bytes / (double)TB, 'f', 2));
    else if(bytes >= GB)
        return QString("%1 GB").arg(QString::number(bytes / (double)GB, 'f', 2));
    else if(bytes >= MB)
        return QString("%1 MB").arg(QString::number(bytes / (double)MB, 'f', 2));
    else if(bytes >= KB)
        return QString("%1 KB").arg(QString::number(bytes / (double)KB, 'f', 2));
    else
        return QString("%1 B").arg(bytes);
}
