#ifndef HTTPAPICLIENT_H
#define HTTPAPICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

class HttpApiClient : public QObject
{
    Q_OBJECT
public:
    explicit HttpApiClient(QObject *parent = nullptr);

    void getOpLatestVersion();

private:
    QNetworkAccessManager *manager = new QNetworkAccessManager;

signals:
    void getOpLatestVersionFinished(QString _version, QString _downloadPath);
};


#endif // HTTPAPICLIENT_H
