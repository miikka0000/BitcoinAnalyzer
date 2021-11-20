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



namespace bitcoinAnalyzer
{
class dataParser
{
const std::string FIAT_CURRENCY = "eur";
const std::string CRYPTO_ID = "bitcoin";

const std::string REQUEST_URL = "https://api.coingecko.com/api/v3/coins/" + CRYPTO_ID +
        "/market_chart/range?vs_currency=" + FIAT_CURRENCY + "&from=" + unixStartTime_ +
        "&to=" + unixEndTime_;

public:
    dataParser();

    void parseData();
    void loadData();
    void onResult(QNetworkReply *reply);

    QNetworkAccessManager *networkAccessManager;
    QNetworkReply *reply;

private:

    QUrl url_;
    //UTC time
    std::string uctStartTime_;
    std::string uctEndTime_;

    // UNIX time
    std::string unixStartTime_;
    std::string unixEndTime_;

    // map data structures that hold the relevent data
    std::map<int, int> prices_;
    std::map<int, int> marketCaps_;
    std::map<int, int> totalVolumes_;

    // iterators for later use when iterating through maps
    std::map<int, int>::iterator pricesIterator_ = prices_.begin();
    std::map<int, int>::iterator marketCapsIterator_ = marketCaps_.begin();
    std::map<int, int>::iterator totalVolumesIterator_ = totalVolumes_.begin();


};

}
#endif // DATAPARSER_H
