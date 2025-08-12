#include "widget.h"
#include "downloadframe.h"

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
QString configPath;
QString opConfigPath;
QFile config;
QFile opConfig;
QJsonDocument doc;
QJsonDocument opDoc;
QJsonObject jsonConfig;

bool zipReader(QString zipPath, QString zipDir){ //压缩包解压
    QZipReader reader(zipPath);
    return reader.extractAll(zipDir);
}

void verifyFileIntegrity(){ //检查文件完整性
    QString uuidString;
    QDir().mkpath(path + "/.shitonline/bin");  //创建目录
    if(!QFileInfo::exists(configPath)){
        //生成UUID
        QUuid uuid = QUuid::createUuid();
        uuidString = uuid.toString(QUuid::WithoutBraces);

        //创建并打开配置文件
        if (!config.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open config file:" << config.errorString();
        }

        //将UUID添加到json
        jsonConfig.insert("UUID", uuidString);
        doc.setObject(jsonConfig);

        //写入JSON文件
        config.write(doc.toJson());
        config.close();
    }
    else{
        //打开配置文件
        if (!config.open(QIODevice::ReadOnly)) {
            qDebug() << "Error opening file:" << config.errorString();
        }

        QString jsonString = config.readAll();
        doc = QJsonDocument::fromJson(jsonString.toUtf8());
        uuidString = doc.object().value("UUID").toString();
        config.close();
    }
    if(!QFileInfo::exists(opConfigPath)){
        //生成OpenP2P的配置
        QJsonObject networkConfig;
        networkConfig.insert("Token", 5623403884671677892);
        networkConfig.insert("Node", uuidString);
        networkConfig.insert("User", "ShitOnline");
        networkConfig.insert("ShareBandwidth", 10);
        networkConfig.insert("ServerHost", "api.openp2p.cn");
        networkConfig.insert("ServerPort", 27183);
        networkConfig.insert("UDPPort1", 27182);
        networkConfig.insert("UDPPort2", 27183);
        networkConfig.insert("TCPPort", 55176);

        QJsonObject opJsonConfig;
        opJsonConfig.insert("network", networkConfig);
        opJsonConfig.insert("apps", QJsonValue::Null);
        opJsonConfig.insert("LogLevel", 2);
        opJsonConfig.insert("MaxLogSize", 1048576);

        opDoc.setObject(opJsonConfig);

        if(!opConfig.open(QIODevice::WriteOnly)){
            qDebug() <<"Failed to open config file:" << opConfig.errorString();
        }

        //写入配置文件
        opConfig.write(opDoc.toJson());
        opConfig.close();
    }
    else{
        //打开OpenP2P的配置文件
        if(!opConfig.open(QIODevice::ReadOnly)){
            qDebug() <<"Failed to open config file:" << opConfig.errorString();
        }

        QString jsonString = opConfig.readAll();
        opDoc = QJsonDocument::fromJson(jsonString.toUtf8());
        opConfig.close();
    }
    if(!QFileInfo::exists(opPath)){
        //创建下载界面
        DownloadFrame d("下载依赖文件");
        d.show();

        //等待下载完成
        QEventLoop loop;
        QObject::connect(&d, &DownloadFrame::downloadFinished, &loop, &QEventLoop::quit);

        d.downloadFile(QUrl("https://gh-proxy.com/https://github.com/openp2p-cn/openp2p/releases/download/v3.21.12/openp2p3.21.12.windows-amd64.zip"),
                       path + "/.shitonline/openp2p.zip");
        loop.exec();
        d.close();

        //解压压缩包
        zipReader(path + "/.shitonline/openp2p.zip", path + "/.shitonline/bin");
    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    path = QApplication::applicationDirPath();
    // DownloadFrame d;
    // d.show();
    // d.downloadFile(QUrl("https://raw.githubusercontent.com/XinyuCraft/ShitOnline/refs/heads/master/main.cpp"), path + "/.shitonline/main.cpp");
    opPath = path + "/.shitonline/bin/openp2p.exe";
    configPath = path + "/.shitonline/config.json";
    opConfigPath = path + "/.shitonline/bin/config.json";

    config.setFileName(configPath);
    opConfig.setFileName(opConfigPath);

    //检查文件完整性
    verifyFileIntegrity();

    Widget w(doc, opDoc);
    w.show();
    return a.exec();
}
