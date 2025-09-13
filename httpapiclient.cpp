#include "httpapiclient.h"

HttpApiClient::HttpApiClient(QObject *parent)
    : QObject{parent}
{}

HttpApiClient *HttpApiClient::getInstance()
{
    static HttpApiClient *instance = new HttpApiClient();
    return instance;
}

void HttpApiClient::getOpLatestVersion() //获取OpenP2P最新版本
{
    qInfo() << "正在获取OpenP2P最新版本";

    QNetworkRequest request(QUrl("https://api.github.com/repos/openp2p-cn/openp2p/releases/latest"));

    QNetworkReply *reply = manager->get(request);

    connect(reply, &QNetworkReply::finished, this, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QString url;
            QByteArray ba = reply->readAll();
            QJsonDocument document = QJsonDocument::fromJson(ba);
            QJsonObject obj = document.object();
            QJsonArray array = obj.value("assets").toArray();
            for(int i = 0;i < array.size();i++){
                QJsonObject tobj = array[i].toObject();
                if(tobj.value("browser_download_url").toString().contains("windows-amd64")){
                    url = tobj.value("browser_download_url").toString();
                    break;
                }
            }
            emit getOpLatestVersionFinished(obj.value("name").toString(), url);
        }
        else{
            qWarning() <<"获取错误: "<<reply->errorString();
        }
        reply->deleteLater();
    });
}
