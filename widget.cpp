#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QJsonDocument _doc, QJsonDocument _opDoc, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    doc = _doc;
    opDoc = _opDoc;
    opProcess = new QProcess(this);

    ui->label_myuuid->setText(doc.object().value("UUID").toString());

    //连接信号与槽
    connect(opProcess, &QProcess::readyReadStandardOutput, this, &Widget::readOpOutut);
    connect(opProcess, &QProcess::errorOccurred, this, [=](QProcess::ProcessError error){
        qDebug() <<"Process error:"<<error;
    });
    connect(opProcess, &QProcess::started, this, [=](){
        qDebug() <<"OpenP2P started";
    });
}

Widget::~Widget()
{
    delete ui;
}

void Widget::addNewApp(QString uuid, int dstPort, int srcport, QString protocol) //添加APP
{
    QJsonObject rootObj = opDoc.object();
    QJsonArray appsArray; //因为现在只能使用一个隧道
    QJsonObject newApp;

    newApp.insert("AppName", "ShitOnline");
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
    rootObj["apps"] = appsArray;
    opDoc.setObject(rootObj);
}

void Widget::saveOpConfig() //保存OpenP2P的配置
{
    QString path = QApplication::applicationDirPath();
    QFile opConfig(path + "/.shitonline/bin/config.json");

    if(!opConfig.open(QIODevice::WriteOnly)){
        qDebug() <<"Failed to open config file:" << opConfig.errorString();
    }

    //写入配置文件
    opConfig.write(opDoc.toJson());
    opConfig.close();
}

void Widget::startOpenP2P() //启动OpenP2P
{
    QString workingDir = QCoreApplication::applicationDirPath() + "/.shitonline/bin";
    QString exePath = workingDir + "/openp2p.exe";
    if(!QFile::exists(exePath)) { //检查是否存在
        qDebug() <<"openp2p.exe not found at "<<exePath;
        return;
    }

    //启动OpenP2P
    opProcess->setWorkingDirectory(workingDir);
    opProcess->start(exePath);
    // 检查是否启动成功
    // if(!opProcess->waitForStarted(3000)) {
    //     qDebug() << "启动失败:" << opProcess->errorString();
    //     qDebug() << "错误详情:" << opProcess->readAllStandardError();
    // }
}

void Widget::on_pushButton_copy_myuuid_clicked() //将UUID添加到剪贴板
{
    addToClipboard(ui->label_myuuid->text());
}

void Widget::addToClipboard(QString copyMessage) //添加到剪贴板
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(copyMessage);
}

void Widget::readOpOutut() //读取OpenP2P的输出
{
    qDebug() <<opProcess->readAllStandardOutput();
}


void Widget::on_pushButton_start_clicked()
{
    if(ui->pushButton_start->text() == "启动"){
        //添加新的隧道
        addNewApp(ui->lineEdit_uuid->text(), ui->lineEdit_dstport->text().toInt(), ui->lineEdit_srcport->text().toInt(), ui->comboBox_protocol->currentText());

        //保存OpenP2P的配置
        saveOpConfig();

        //启动OpenP2P
        startOpenP2P();

        ui->pushButton_start->setText("关闭");
    }
    else if(ui->pushButton_start->text() == "关闭"){
        opProcess->kill(); //关闭OpenP2P

        ui->pushButton_start->setText("启动");
    }
}

