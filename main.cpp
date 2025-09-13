#include "widget.h"
#include "downloadframe.h"
#include "httpapiclient.h"
#include "configmanager.h"
#include "logger.h"

#include <QApplication>
#include <QUuid>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QProcess>
#include <QSslSocket>
#include <QUrl>
#include <QEventLoop>
#include <private/qzipreader_p.h>

QString path;
QString opPath;
HttpApiClient *client;
ConfigManager *manager;

bool zipReader(QString zipPath, QString zipDir){ //压缩包解压
    QZipReader reader(zipPath);
    return reader.extractAll(zipDir);
}

void verifyFileIntegrity(){ //检查文件完整性

    manager->initializeConfig(); //初始化配置
    //加载配置
    manager->loadConfig();
    manager->loadOpConfig();

    if(!QFileInfo::exists(opPath)){
        QEventLoop loop;

        //创建下载界面
        DownloadFrame d("下载依赖文件");
        d.show();

        //等待下载完成
        QObject::connect(&d, &DownloadFrame::downloadFinished, &loop, &QEventLoop::quit);

        d.downloadFile(QUrl("https://gh-proxy.com/https://github.com/openp2p-cn/openp2p/releases/download/v3.21.12/openp2p3.21.12.windows-amd64.zip"),
                       path + "/.shitonline/openp2p.zip");
        loop.exec();
        d.close();

        qInfo() << "下载成功, 正在解压中...";

        //解压压缩包
        zipReader(path + "/.shitonline/openp2p.zip", path + "/.shitonline/bin");

        qInfo() << "解压成功!";
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setWindowIcon(QIcon(":/image/icon.png"));

    client = HttpApiClient::getInstance();
    manager = ConfigManager::getInstance();

    path = QApplication::applicationDirPath();
    opPath = path + "/.shitonline/bin/openp2p.exe";

    //启动日志功能
    Logger::getInstance()->start();

    //检查文件完整性
    verifyFileIntegrity();

    qInfo() << "ShitOnline已启动!";


    // DownloadFrame d;
    // d.show();
    // d.downloadFile(QUrl("https://raw.githubusercontent.com/XinyuCraft/ShitOnline/refs/heads/master/main.cpp"), path + "/.shitonline/main.cpp");


    Widget w;
    w.show();
    return a.exec();
}
