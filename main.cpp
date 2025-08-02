#include "widget.h"

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

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QFile config(".shitp2p/config.json");
    QFile opConfig(".shitp2p/bin/config.json");
    QJsonDocument doc;
    QJsonDocument opDoc;
    QJsonObject jsonConfig;

    if(QFileInfo::exists(".shitp2p/config.json") == false){
        QDir().mkpath(".shitp2p/bin");  //创建目录
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
        // 生成 OpenP2P 的配置
        QJsonObject networkConfig;
        networkConfig.insert("Token", 5623403884671677892);
        networkConfig.insert("Node", uuidString);
        networkConfig.insert("User", "ShitP2P");
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
        qDebug() <<jsonString.toUtf8();
        doc = QJsonDocument::fromJson(jsonString.toUtf8());
        config.close();
    }

    Widget w(doc, opDoc);
    w.show();
    return a.exec();
}
