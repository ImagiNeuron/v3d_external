#include "httputilsgetlocation.h"

#include <QJsonDocument>
#include <QJsonObject>

HttpGetLocation::HttpGetLocation(QWidget *parent)
{
    manager = new QNetworkAccessManager();
}

HttpGetLocation::~HttpGetLocation()
{

}

/**
 * @brief Construct get potential location post request Json body
 * @param userInfo
 */
void HttpGetLocation::getPotentialLoaction(QJsonObject &userInfo)
{
    QJsonObject body;
    body.insert("user", userInfo);
    asyncPostRequest(URL_GET_POTENTIAL_LOCATION, body);
}

/**
 * @brief HttpGetLocation::asyncPostRequest
 * @param url
 * @param body
 */
void HttpGetLocation::asyncPostRequest(QString url, QJsonObject &body)
{
    QJsonDocument document;
    document.setObject(body);
    QByteArray dataArray;
    dataArray = document.toJson(QJsonDocument::Compact);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=utf-8");
    request.setUrl(QUrl(url));

    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(locationReplyFinished(QNetworkReply*)));
    manager->post(request, dataArray);
}

/**
 * @brief slots
 */
void HttpGetLocation::locationReplyFinished(QNetworkReply *reply)
{
    int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if(status == 200) {
        QByteArray data = reply->readAll().trimmed();

        // pharse Json, need "loc" to download
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(data, &error);
        if(error.error == QJsonParseError::NoError) {
            QVariantMap map = doc.toVariant().toMap();
            if(map.contains("loc")) {
//                qDebug() << "map[loc]:" << map["loc"];
                QVariantMap mapLoc = map["loc"].toMap();
                int x = mapLoc["x"].toInt();
                int y = mapLoc["y"].toInt();
                int z = mapLoc["z"].toInt();
                // let x,y,z out to CSMainWindow to construct CoordinateConvert
                emit sendXYZ(x, y, z);
            }
        }
    }
    reply->deleteLater();
}
