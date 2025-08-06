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

private:
    Ui::Widget *ui;

    void addToClipboard(QString copyMessage);
    void readOpOutut();

    QJsonDocument doc;
    QJsonDocument opDoc;
    QProcess *opProcess;
};
#endif // WIDGET_H
