#include "configmanager.h"

ConfigManager::ConfigManager(QString _configPath, QString _opConfigPath, QObject *parent)
    : QObject{parent}
{
    configPath = _configPath;
    opConfigPath = _opConfigPath;
}

ConfigManager *ConfigManager::getInstance() //获取单例
{
    static ConfigManager *instance = new ConfigManager();
    return instance;
}

void ConfigManager::initializeConfig() //初始化配置文件
{
    QFile *configFile = new QFile(configPath);
    QFile *opConfigFile = new QFile(opConfigPath);
    QDir().mkpath(QApplication::applicationDirPath() + "/.shitonline/bin");  //创建目录
    if(!configFile->exists()){
        qDebug() << "配置文件未存在, 正在创建默认配置...";

        //生成UUID
        QUuid uuid = QUuid::createUuid();
        QString uuidString = uuid.toString(QUuid::WithoutBraces);

        configObj.insert("UUID", uuidString);
        configDoc.setObject(configObj);

        //创建并打开配置文件
        if (!configFile->open(QIODevice::WriteOnly)) {
            qDebug() << "打开文件报错: " << configFile->errorString();
        }

        //写入JSON文件
        configFile->write(configDoc.toJson());
        configFile->close();
    }
    if(!opConfigFile->exists()){
        //创建文件
        if(!opConfigFile->open(QIODevice::WriteOnly)){
            qDebug() << "打开文件报错: " << opConfigFile->errorString();
        }

        QJsonObject networkConfig;
        networkConfig.insert("Token", 5623403884671677892);
        networkConfig.insert("Node", configObj["UUID"]);
        networkConfig.insert("User", "ShitOnline");
        networkConfig.insert("ShareBandwidth", 10);
        networkConfig.insert("ServerHost", "api.openp2p.cn");
        networkConfig.insert("ServerPort", 27183);
        networkConfig.insert("UDPPort1", 27182);
        networkConfig.insert("UDPPort2", 27183);
        networkConfig.insert("TCPPort", 55176);

        opConfigObj.insert("network", networkConfig);
        opConfigObj.insert("apps", QJsonValue::Null);
        opConfigObj.insert("LogLevel", 2);
        opConfigObj.insert("MaxLogSize", 1048576);

        opConfigDoc.setObject(opConfigObj);

        opConfigFile->write(opConfigDoc.toJson());
        opConfigFile->close();
    }
}

void ConfigManager::loadConfig()
{
    QFile *file = new QFile(configPath);
    if(!file->open(QIODevice::ReadOnly)){
        qDebug() << "打开文件报错: " << file->errorString();
    }

    configDoc = QJsonDocument::fromJson(file->readAll());
    configObj = configDoc.object();
    file->close();
}

void ConfigManager::loadOpConfig()
{
    QFile *file = new QFile(opConfigPath);
    if(!file->open(QIODevice::ReadOnly)){
        qDebug() << "打开文件报错: " << file->errorString();
    }

    opConfigDoc = QJsonDocument::fromJson(file->readAll());
    opConfigObj = opConfigDoc.object();
    file->close();
}

void ConfigManager::saveConfig()
{
    QFile *file = new QFile(configPath);
    if (!file->open(QIODevice::WriteOnly)) {
        qDebug() << "打开文件报错: " << file->errorString();
    }

    //写入配置文件
    file->write(configDoc.toJson());
    file->close();
}

void ConfigManager::saveOpConfig()
{
    QFile *file = new QFile(opConfigPath);
    if(!file->open(QIODevice::WriteOnly)){
        qDebug() << "打开文件报错: " << file->errorString();
    }

    //写入配置文件
    file->write(opConfigDoc.toJson());
    file->close();
}

void ConfigManager::addNewApp(const QString &appName, const QString &uuid, const int &dstPort, const int &srcport, const QString &protocol) //添加新隧道
{
    QJsonArray appsArray = opConfigObj["apps"].toArray();
    QJsonObject newApp;

    newApp.insert("AppName", appName);
    newApp.insert("Protocol", protocol);
    newApp.insert("Whitelist", "");
    newApp.insert("SrcPort", srcport);
    newApp.insert("PeerNode", uuid);
    newApp.insert("DstPort", dstPort);
    newApp.insert("DstHost", "localhost");
    newApp.insert("PeerUser", "");
    newApp.insert("RelayNode", "");
    newApp.insert("Enabled", 1);

    appsArray.append(newApp);
    opConfigObj["apps"] = appsArray;
    opConfigDoc.setObject(opConfigObj);
}

void ConfigManager::changeEnabled(const int &srcport) //改变启用状态
{
    QJsonArray appsArray = opConfigObj["apps"].toArray();
    for(int i = 0;i < appsArray.size();i++){
        QJsonObject app = appsArray[i].toObject();
        if(app["SrcPort"].toInt() == srcport){
            if(app["Enabled"].toInt() == 1){
                app["Enabled"] = 0;
            }
            else{
                app["Enabled"] = 1;
            }
            appsArray[i] = app;
            qDebug() << app["AppName"].toString() << "的启用状态被改变: " << app["Enabled"].toInt();
            break;
        }
    }

    opConfigObj["apps"] = appsArray;
    opConfigDoc.setObject(opConfigObj);
    saveOpConfig();
}

void ConfigManager::deleteApp(const int &srcport) //删除隧道配置
{
    QJsonArray appsArray = opConfigObj["apps"].toArray();
    for(int i = 0;i < appsArray.size();i++){
        QJsonObject app = appsArray[i].toObject();
        if(app["SrcPort"].toInt() == srcport){
            appsArray.removeAt(i);
            break;
        }
    }

    opConfigObj["apps"] = appsArray;
    opConfigDoc.setObject(opConfigObj);
}

bool ConfigManager::isSrcPortTaken(const int &srcport) // //判断本地端口是否被占用
{
    QJsonArray appsArray = opConfigObj["apps"].toArray();
    for(int i = 0;i < appsArray.size();i++){
        QJsonObject app = appsArray[i].toObject();
        if(app["SrcPort"].toInt() == srcport){
            return false;
        }
    }
    return true;
}

void ConfigManager::changeAppConfig(const int &pastSrcPort, const QString &appName, const QString &uuid, const int &dstport, const int &srcport, const QString &protocol) //修改隧道配置
{
    QJsonArray appsArray = opConfigObj["apps"].toArray();
    for(int i = 0;i < appsArray.size();i++){
        QJsonObject app = appsArray[i].toObject();
        if(app["SrcPort"].toInt() == pastSrcPort){
            app["AppName"] = appName;
            app["Protocol"] = protocol;
            app["SrcPort"] = srcport;
            app["PeerNode"] = uuid;
            app["DstPort"] = dstport;
            appsArray[i] = app;
            break;
        }
    }

    opConfigObj["apps"] = appsArray;
    opConfigDoc.setObject(opConfigObj);
}

