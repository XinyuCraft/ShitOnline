#ifndef APPITEM_H
#define APPITEM_H

#include <QWidget>
#include <QClipboard>
#include <QListWidget>
#include <QListWidgetItem>

class Widget;

namespace Ui {
class AppItem;
}

class AppItem : public QWidget
{
    Q_OBJECT

public:
    explicit AppItem(QListWidgetItem *_item, const int &_enabled, const QString &_appName, const QString &_uuid, const int &_dstport, const int &_srcport, const QString &_protocol, Widget *parent = nullptr);
    ~AppItem();

    void changeConnectionState(QString connectionInfo = "");

signals:
    void appDeleted(QListWidgetItem *item, const int &srcport); //隧道被删除
    void enabledChanged(const int &srcport); //隧道启用状态被改变
    void appEdited(const QString &appName, const QString &uuid, const int &dstport, const int &srcport, const QString &protocol, AppItem *item); //隧道被编辑

private slots:
    void on_pushButton_copy_addr_clicked();

    void on_pushButton_del_clicked();

    void on_checkBox_stateChanged(int arg1);

    void on_pushButton_edit_clicked();


public:
    void changeApp(const QString &_appName, const QString &_uuid, const int &_dstport, const int &_srcport, const QString &_protocol); //改变隧道信息

private:
    Ui::AppItem *ui;

    QListWidgetItem *item;
    int enabled;
    QString appName;
    QString uuid;
    int dstport;
    int srcport;
    QString protocol;
};

#endif // APPITEM_H
