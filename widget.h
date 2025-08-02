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

private slots:
    void on_pushButton_copy_myuuid_clicked();

    void on_pushButton_start_clicked();

private:
    Ui::Widget *ui;

    void addToClipboard(QString copyMessage);

    QJsonDocument doc;
    QJsonDocument opDoc;
};
#endif // WIDGET_H
