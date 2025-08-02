#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QJsonDocument _doc, QJsonDocument _opDoc, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    doc = _doc;
    opDoc = _opDoc;
    ui->label_myuuid->setText(doc.object().value("UUID").toString());
}

Widget::~Widget()
{
    delete ui;
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


void Widget::on_pushButton_start_clicked()
{
    QJsonObject json = opDoc.object();
    QJsonObject app;
    QJsonArray appsArray = json.value("apps").toArray();
    app.insert("AppName", "ShitP2P");
    app.insert("Protocol", ui->comboBox_protocol->currentText());
    app.insert("Whitelist", "");
    app.insert("SrcPort", ui->lineEdit_srcport->text().toInt());
    app.insert("PeerNode", ui->lineEdit_uuid->text());
    app.insert("DstPort", ui->lineEdit_dstport->text().toInt());
    app.insert("DstHost", "localhost");
    app.insert("PeerUser", "");
    app.insert("RelayNode", "");
    app.insert("Enabled", 1);
    appsArray.append(app);
    json["apps"] = appsArray;
    opDoc.setObject(json);

    qDebug() <<opDoc.toJson();
}

