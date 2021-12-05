#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <list>
#include <QString>
#include <map>
#include <QJsonObject>
#include <memory>
#include <string>
#include <map>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>



namespace bitcoinAnalyzer
{
class dataParser
{
const std::string FIAT_CURRENCY = "eur";
const std::string CRYPTO_ID = "bitcoin";

public:
    dataParser();

    std::map<double, double> readData(QJsonArray array);

    void arrayElemsToArray(QJsonArray jsonArray);

    void loadData();

    std::string unixTimeToHumanReadable(long int seconds, bool showTime = false);




    QNetworkAccessManager *networkAccessManager;
    QNetworkReply *reply;

private:

    unsigned int daysBetween_;

    int longestBearTrend_;

    QJsonDocument jsonDocument_;
    QJsonObject jsonObject_;

    QJsonArray pricesArray_;
    QJsonArray marketCapsArray_;
    QJsonArray totalVolumesArray_;

    QJsonValue jsonValue_;

    // map data structures that hold the relevent data
    std::map<double, double> pricesMap_;
    std::map<double, double> marketCapsMap_;
    std::map<double, double> totalVolumesMap_;

    std::map<QDateTime, double> uctDatePrices_;

    // iterators for later use when iterating through maps
    std::map<double, double>::iterator pricesIterator_ = pricesMap_.begin();
    std::map<double, double>::iterator marketCapsIterator_ = marketCapsMap_.begin();
    std::map<double, double>::iterator totalVolumesIterator_ = totalVolumesMap_.begin();


};

}
#endif // DATAPARSER_H
