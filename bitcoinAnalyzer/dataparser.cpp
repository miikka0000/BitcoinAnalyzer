#include "dataparser.h"

#include <string>
#include <iostream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QString>
#include <QDebug>
#include <QUrl>
#include <QNetworkAccessManager>


namespace bitcoinAnalyzer {


dataParser::dataParser()
{

    networkAccessManager = new QNetworkAccessManager();
    url_ = QUrl(QString::fromStdString(REQUEST_URL));
    reply = networkAccessManager->get(QNetworkRequest(url_));


}


void dataParser::parseData()
{

}

void dataParser::loadData()
{

}

void dataParser::onResult(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError){

            QByteArray result = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(result);
            QJsonObject obj = jsonResponse.object();
            QJsonArray pricesArray = obj["prices"].toArray();
            QJsonArray marketCapsArray = obj["market_caps"].toArray();
            QJsonArray totalVolumesArray = obj["total_volumes"].toArray();

            for(const QJsonValue & value : pricesArray) {
               qDebug()<< "prices: " <<value.toString();
            }

            for(const QJsonValue & value : marketCapsArray) {
               qDebug()<< "market_caps: " <<value.toString();
            }

            for(const QJsonValue & value : totalVolumesArray) {
               qDebug()<< "total_volumes: " <<value.toString();
            }

        }
        else
            qDebug() << "ERROR";
        reply->deleteLater();
    }
}








