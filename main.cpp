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

QString path;
QString configPath;
QString opConfigPath;
QFile config;
QFile opConfig;
QJsonDocument doc;
QJsonDocument opDoc;
QJsonObject jsonConfig;


// void verifyFileIntegrity(){ //检查文件完整性
//     if(QFileInfo::exists(configPath) == false){
//         QDir().mkpath(path + "/.shitonline/bin");  //创建目录
//         //生成UUID
//         QUuid uuid = QUuid::createUuid();
//         QString uuidString = uuid.toString(QUuid::WithoutBraces);
//     }
// }

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QString path = QApplication::applicationDirPath();
    // DownloadFrame d;
    // d.show();
    // d.DownloadFile(QUrl("https://raw.githubusercontent.com/XinyuCraft/ShitOnline/refs/heads/master/main.cpp"), path + "/.shitonline/main.cpp");
    configPath = path + "/.shitonline/config.json";
    opConfigPath = path + "/.shitonline/bin/config.json";

    config.setFileName(configPath);
    opConfig.setFileName(opConfigPath);

    if(QFileInfo::exists(configPath) == false){
        QDir().mkpath(path + "/.shitonline/bin");  //创建目录
        //生成UUID
        QUuid uuid = QUuid::createUuid();
        QString uuidString = uuid.toString(QUuid::WithoutBraces);

        //创建并打开配置文件
        if (!config.open(QIODevice::WriteOnly)) {
            qDebug() << "Failed to open config file:" << config.errorString();
            return -1;
        }

        //将UUID添加到json
        jsonConfig.insert("UUID", uuidString);
        doc.setObject(jsonConfig);

        //写入JSON文件
        config.write(doc.toJson());
        config.close();

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
        //打开配置文件
        if (!config.open(QIODevice::ReadOnly)) {
            qDebug() << "Error opening file:" << config.errorString();
            return -1;
        }

        QString jsonString = config.readAll();
        doc = QJsonDocument::fromJson(jsonString.toUtf8());
        config.close();

        //打开OpenP2P的配置文件
        if(!opConfig.open(QIODevice::ReadOnly)){
            qDebug() <<"Failed to open config file:" << opConfig.errorString();
        }

        jsonString = opConfig.readAll();
        opDoc = QJsonDocument::fromJson(jsonString.toUtf8());
        opConfig.close();
    }

    Widget w(doc, opDoc);
    w.show();
    return a.exec();
}
