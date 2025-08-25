#include "appitem.h"
#include "ui_appitem.h"
#include "widget.h"

AppItem::AppItem(QListWidgetItem *_item, const int &_enabled, const QString &_appName, const QString &_uuid, const int &_dstport, const int &_srcport, const QString &_protocol, Widget *parent)
    : QWidget(parent)
    , ui(new Ui::AppItem)
{
    ui->setupUi(this);

    //初始化
    item = _item;
    enabled = _enabled;
    appName = _appName;
    uuid = _uuid;
    dstport = _dstport;
    srcport = _srcport;
    protocol = _protocol;

    ui->label_app_name->setText(QString("<html><head/><body><p><span style=\" font-size:12pt; font-weight:700;\">%1(离线)</span></p></body></html>").arg(appName));
    ui->label_uuid->setText(uuid);
    ui->label_dstport->setText("远程端口: " + QString::number(dstport));
    ui->label_srcport->setText("本地端口: " + QString::number(srcport));
    ui->label_protocol->setText("协议: " + protocol);
    ui->checkBox->setChecked(enabled);

    connect(this, &AppItem::appDeleted, parent, &Widget::deleteApp);
    connect(this, &AppItem::appEdited, parent, &Widget::editApp);
    connect(parent, &Widget::started, this, [this](){
        ui->pushButton_del->setEnabled(false);
        ui->pushButton_edit->setEnabled(false);
        ui->checkBox->setEnabled(false);
    });
    connect(parent, &Widget::stopped, this, [this](){
        ui->pushButton_del->setEnabled(true);
        ui->pushButton_edit->setEnabled(true);
        ui->checkBox->setEnabled(true);
    });
}

AppItem::~AppItem()
{
    delete ui;
}

void AppItem::changeConnectionState(QString connectionInfo)
{
    if(connectionInfo.contains("LISTEN ON PORT " + protocol + ":" + QString::number(srcport) + " START")){
        qDebug() << appName << "隧道连接成功";

        ui->label_app_name->setText(QString("<html><head/><body><p><span style=\" font-size:12pt; font-weight:700;\">%1(在线)</span></p></body></html>").arg(appName));
    }
    else if(connectionInfo == ""){
        qDebug() << appName << "隧道断开";

        ui->label_app_name->setText(QString("<html><head/><body><p><span style=\" font-size:12pt; font-weight:700;\">%1(离线)</span></p></body></html>").arg(appName));
    }
}

void AppItem::on_pushButton_copy_addr_clicked() //复制连接地址
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText("localhost:" + QString::number(srcport));
}


void AppItem::on_pushButton_del_clicked() //删除隧道
{
    emit appDeleted(item, srcport);
}


void AppItem::on_checkBox_stateChanged(int arg1) //改变启用状态
{
    emit enabledChanged(srcport);
}


void AppItem::on_pushButton_edit_clicked() //编辑隧道
{
    emit appEdited(appName, uuid, dstport, srcport, protocol, this);
}

void AppItem::changeApp(const QString &_appName, const QString &_uuid, const int &_dstport, const int &_srcport, const QString &_protocol) //改变隧道信息
{
    appName = _appName;
    uuid = _uuid;
    dstport = _dstport;
    srcport = _srcport;
    protocol = _protocol;

    ui->label_app_name->setText(QString("<html><head/><body><p><span style=\" font-size:12pt; font-weight:700;\">%1(离线)</span></p></body></html>").arg(appName));
    ui->label_uuid->setText(uuid);
    ui->label_dstport->setText("远程端口: " + QString::number(dstport));
    ui->label_srcport->setText("本地端口: " + QString::number(srcport));
    ui->label_protocol->setText("协议: " + protocol);
}

