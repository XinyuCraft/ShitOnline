#ifndef WIDGET_H
#define WIDGET_H

#include "httpapiclient.h"
#include "addnewappdialog.h"
#include "appitem.h"
#include "configmanager.h"

#include <QWidget>
#include <QClipboard>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(ConfigManager *_manager, HttpApiClient *_client, QWidget *parent = nullptr);
    ~Widget();

    void startOpenP2P();

private slots:
    void on_pushButton_start_clicked();

    void onSystemTrayClicked(QSystemTrayIcon::ActivationReason reason);

    void on_pushButton_add_new_app_clicked();

    void addNewApp(const QString &appName, const QString &uuid, const int &dstport, const int &srcport, const QString &protocol);

    void loadApp();

    void deleteApp(QListWidgetItem *item, const int &srcport);

    void editApp(const QString &appName, const QString &uuid, const int &dstport, const int &pastSrcPort, const QString &protocol, AppItem *item);

signals:
    void connected(QString connectionInfo); //连接成功
    void disconnected(QString connectionInfo); //取消连接

private:
    Ui::Widget *ui;

    void readOpOutut();
    void closeEvent(QCloseEvent *event);

    QProcess *opProcess;
    HttpApiClient *client;
    ConfigManager *manager;

    //系统托盘
    QSystemTrayIcon *systemTrayIcon;
    QMenu *menu;
};
#endif // WIDGET_H
