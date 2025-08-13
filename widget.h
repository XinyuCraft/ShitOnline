#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QClipboard>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>
#include <QApplication>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QJsonDocument _doc, QJsonDocument _opDoc, QWidget *parent = nullptr);
    ~Widget();

    void addNewApp(QString uuid, int dstPort, int srcport, QString protocol);
    void saveOpConfig();
    void startOpenP2P();

private slots:
    void on_pushButton_copy_myuuid_clicked();

    void on_pushButton_start_clicked();

    void on_pushButton_copy_addr_clicked();

    void onSystemTrayClicked(QSystemTrayIcon::ActivationReason reason);

private:
    Ui::Widget *ui;

    void addToClipboard(QString copyMessage);
    void readOpOutut();
    void closeEvent(QCloseEvent *event);

    QJsonDocument doc;
    QJsonDocument opDoc;
    QProcess *opProcess;

    //系统托盘
    QSystemTrayIcon *systemTrayIcon;
    QMenu *menu;
};
#endif // WIDGET_H
