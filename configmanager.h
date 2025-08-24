#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <QObject>
#include <QApplication>
#include <QString>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QUuid>

class ConfigManager : public QObject
{
    Q_OBJECT
public:
    explicit ConfigManager(QString _configPath = QApplication::applicationDirPath() + "/.shitonline/config.json", QString _opConfigPath = QApplication::applicationDirPath() + "/.shitonline/bin/config.json", QObject *parent = nullptr);

    void initializeConfig();
    void loadConfig();
    void loadOpConfig();
    void saveConfig();
    void saveOpConfig();
    void addNewApp(const QString &appName, const QString &uuid, const int &dstPort, const int &srcport, const QString &protocol);
    void changeEnabled(const int &srcport);
    void deleteApp(const int &srcport);
    bool isSrcPortTaken(const int &srcport);
    void changeAppConfig(const int &pastSrcPort, const QString &appName, const QString &uuid, const int &dstport, const int &srcport, const QString &protocol);

    QJsonObject configObj; //配置
    QJsonObject opConfigObj; //OpenP2P配置

signals:

private:
    QJsonDocument configDoc; //程序配置文档
    QJsonDocument opConfigDoc; //OpenP2P配置文档
    QString configPath; //配置文件路径
    QString opConfigPath; //OpenP2P配置文件路径
};

#endif // CONFIGMANAGER_H
