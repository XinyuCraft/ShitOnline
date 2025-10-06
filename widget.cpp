#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("ShitOnline");

    opProcess = new QProcess(this);
    manager = ConfigManager::getInstance();
    client = HttpApiClient::getInstance();

    //系统托盘
    systemTrayIcon = new QSystemTrayIcon(this);
    menu = new QMenu(this);
    systemTrayIcon->setToolTip("ShitOnline");
    systemTrayIcon->setIcon(QIcon(":/image/icon.png"));

    //绑定动作
    connect(ui->actionShow, &QAction::triggered, this, &Widget::show);
    connect(ui->actionExit, &QAction::triggered, this, &QApplication::quit);
    connect(systemTrayIcon, &QSystemTrayIcon::activated, this, &Widget::onSystemTrayClicked);

    //添加到系统托盘
    menu->addAction(ui->actionShow);
    menu->addAction(ui->actionExit);
    systemTrayIcon->setContextMenu(menu);
    systemTrayIcon->show();

    ui->label_myuuid->setText(manager->configObj.value("UUID").toString());

    //连接信号与槽
    connect(opProcess, &QProcess::readyReadStandardOutput, this, &Widget::readOpOutut);
    connect(opProcess, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error){
        qWarning() <<"线程错误: "<<error;
    });
    connect(opProcess, &QProcess::started, this, [=](){
        qInfo() <<"OpenP2P已启动!";
    });

    loadApp(); //加载隧道
}

Widget::~Widget()
{
    delete ui;
}

void Widget::addNewApp(const QString &appName, const QString &uuid, const int &dstport, const int &srcport, const QString &protocol)
{
    if(manager->isSrcPortTaken(srcport) == true){ //本地端口是否被占用
        //在列表中添加隧道
        QListWidgetItem *item = new QListWidgetItem();
        AppItem *appItem = new AppItem(item, 1, appName, uuid, dstport, srcport, protocol, this);

        item->setSizeHint(QSize(350, 60));
        ui->listWidget->addItem(item);

        ui->listWidget->setItemWidget(item, appItem);

        //将隧道添加到配置文件里
        manager->addNewApp(appName, uuid, dstport, srcport, protocol);
        manager->saveOpConfig();
    }
    else{
        QMessageBox::warning(this, "添加错误", "本地端口被占用, 请更换本地端口");
    }
}

void Widget::loadApp()
{
    //添加配置中的隧道
    QJsonArray appsArray = manager->opConfigObj.value("apps").toArray();
    for(int i = 0;i < appsArray.size();i++){
        QJsonObject app = appsArray[i].toObject();

        //在列表中添加隧道
        QListWidgetItem *item = new QListWidgetItem();
        AppItem *appItem = new AppItem(item, app["Enabled"].toInt(), app["AppName"].toString(), app["PeerNode"].toString(), app["DstPort"].toInt(), app["SrcPort"].toInt(), app["Protocol"].toString(), this);

        item->setSizeHint(QSize(350, 60));
        ui->listWidget->addItem(item);

        ui->listWidget->setItemWidget(item, appItem);

        connect(appItem, &AppItem::enabledChanged, manager, &ConfigManager::changeEnabled);
        connect(this, &Widget::connected, appItem, &AppItem::changeConnectionState);
        connect(this, &Widget::disconnected, appItem, &AppItem::changeConnectionState);
    }
}

void Widget::deleteApp(QListWidgetItem *item, const int &srcport) //删除隧道
{
    if (item)
    {
        delete item;
    }
    manager->deleteApp(srcport);
    manager->saveOpConfig();
}

void Widget::editApp(const QString &appName, const QString &uuid, const int &dstport, const int &pastSrcPort, const QString &protocol, AppItem *item)
{
    AddNewAppDialog *addNewAppDialog = new AddNewAppDialog(appName, uuid, QString::number(dstport), QString::number(pastSrcPort), protocol);
    addNewAppDialog->show();

    connect(addNewAppDialog, &AddNewAppDialog::dataReturned, this, [this, item, pastSrcPort](const QString &appName, const QString &uuid, const int &dstport, const int &srcport, const QString &protocol){
        if(manager->isSrcPortTaken(srcport) == true){
            this->manager->changeAppConfig(pastSrcPort, appName, uuid, dstport, srcport, protocol);
            this->manager->saveOpConfig();
            item->changeApp(appName, uuid, dstport, srcport, protocol);
        }
        else{
            QMessageBox::warning(this, "添加错误", "本地端口被占用, 请更换本地端口");
        }
    });

    addNewAppDialog->exec();
}

void Widget::startOpenP2P() //启动OpenP2P
{
    QString workingDir = QCoreApplication::applicationDirPath() + "/.shitonline/bin";
    QString exePath = workingDir + "/openp2p.exe";
    if(!QFile::exists(exePath)) { //检查是否存在
        qWarning() <<"openp2p.exe 没有找到!"<<exePath;
        return;
    }

    //启动OpenP2P
    qInfo() << "启动OpenP2P中...";
    opProcess->setWorkingDirectory(workingDir);
    opProcess->start(exePath);
    // 检查是否启动成功
    // if(!opProcess->waitForStarted(3000)) {
    //     qDebug() << "启动失败:" << opProcess->errorString();
    //     qDebug() << "错误详情:" << opProcess->readAllStandardError();
    // }
}


void Widget::readOpOutut() //读取OpenP2P的输出
{
    QString outPut = opProcess->readAllStandardOutput(); //获取OpenP2P的输出
    qDebug() <<outPut;

    if(outPut.contains("P2PNetwork init start")){ //OpenP2P是否启动成功
        qInfo() <<"OpenP2P启动成功";

        ui->label_mystate->setText("本机状态: 在线");
    }

    if(outPut.contains("LISTEN ON PORT ")){ //隧道是否添加成功
        emit connected(outPut);
    }
}

void Widget::closeEvent(QCloseEvent *event) //重写退出事件
{
    this->hide();
    event->ignore();
}


void Widget::on_pushButton_start_clicked()
{
    if(ui->pushButton_start->text() == "启动"){
        //发出启动信号
        emit started();

        //保存配置
        manager->saveConfig();
        manager->saveOpConfig();

        //启动OpenP2P
        startOpenP2P();

        ui->pushButton_start->setText("关闭");

        //禁用添加隧道按钮
        ui->pushButton_add_new_app->setEnabled(false);
    }
    else if(ui->pushButton_start->text() == "关闭"){
        //发出关闭信号
        emit stopped();

        opProcess->kill(); //关闭OpenP2P
        emit disconnected("");
        ui->pushButton_start->setText("启动");
        ui->label_mystate->setText("本机状态: 离线");

        //启用添加隧道功能
        ui->pushButton_add_new_app->setEnabled(true);
    }
}


void Widget::onSystemTrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::DoubleClick){
        this->show();
    }
}


void Widget::on_pushButton_add_new_app_clicked() //添加新的隧道
{
    AddNewAppDialog *addNewAppDialog = new AddNewAppDialog(this);
    addNewAppDialog->show();

    connect(addNewAppDialog, &AddNewAppDialog::dataReturned, this, &Widget::addNewApp);
    manager->saveOpConfig();

    addNewAppDialog->exec();
}


void Widget::on_pushButton_copy_myuuid_clicked() //复制UUID
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(ui->label_myuuid->text());
}

