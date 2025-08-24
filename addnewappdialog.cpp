#include "addnewappdialog.h"
#include "ui_addnewappdialog.h"

AddNewAppDialog::AddNewAppDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddNewAppDialog)
{
    ui->setupUi(this);
}

AddNewAppDialog::AddNewAppDialog(QString _appName, QString _uuid, QString _dstport, QString _srcport, QString _protocol, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddNewAppDialog)
{
    ui->setupUi(this);

    ui->lineEdit_app_name->setText(_appName);
    ui->lineEdit_uuid->setText(_uuid);
    ui->lineEdit_dstport->setText(_dstport);
    ui->lineEdit_srcport->setText(_srcport);
    ui->comboBox_protocol->setCurrentText(_protocol);
}

AddNewAppDialog::~AddNewAppDialog()
{
    delete ui;
}

void AddNewAppDialog::on_buttonBox_accepted() //添加新隧道
{
    if(ui->lineEdit_app_name->text() != "" && ui->lineEdit_uuid->text() != "" && ui->lineEdit_dstport->text() != "" && ui->lineEdit_srcport->text() != ""){
        if(ui->lineEdit_dstport->text().toInt() >= 0 && ui->lineEdit_dstport->text().toInt() <= 65535 && ui->lineEdit_srcport->text().toInt() >= 0 && ui->lineEdit_srcport->text().toInt() <= 65535)
            emit dataReturned(ui->lineEdit_app_name->text(), ui->lineEdit_uuid->text(), ui->lineEdit_dstport->text().toInt(), ui->lineEdit_srcport->text().toInt(), ui->comboBox_protocol->currentText());
        else
            QMessageBox::warning(this, "报错信息", "端口范围错误, 应为0~65535");
    }
    else{
        QMessageBox::warning(this, "报错信息", "不能为空!!!");
    }
}

