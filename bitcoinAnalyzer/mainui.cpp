#include "mainui.h"
#include "ui_mainui.h"
#include "dataparser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDateTime>
#include <QTime>


mainUI::mainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainUI)
{
    ui->setupUi(this);

    networkManager_ = new QNetworkAccessManager(this);
    ui->endDateEdit->setTime(QTime(1, 0, 0));

    ui->startDateEdit->setDate(QDate(2021, 10, 1));
    ui->endDateEdit->setDate(QDate(2021, 11, 1));

    connect(networkManager_, &QNetworkAccessManager::finished, this, &mainUI::onResult);


}

mainUI::~mainUI()
{
    delete ui;
    delete networkManager_;

}

void mainUI::setTimes()
{


    // get UCT-times from user input
    timeVars.uctStartTime_ = ui->startDateEdit->dateTime();
    // storing the time value as UTC-time
    timeVars.uctStartTime_.setTimeSpec(Qt::UTC);

    timeVars.uctEndTime_ = ui->endDateEdit->dateTime();
    timeVars.uctEndTime_.setTimeSpec(Qt::UTC);

    // convert UCT-times to an unix timestamp
    timeVars.unixStartTime_ = timeVars.uctStartTime_.toTime_t();
    timeVars.unixEndTime_ = timeVars.uctEndTime_.toTime_t();

    // convert the unix-times to string for API request URL

    strUnixStartTime_ = std::to_string(timeVars.unixStartTime_);
    strUnixEndTime_ = std::to_string(timeVars.unixEndTime_);

    // preparing the correct URL for data fetching
    REQUEST_URL = "https://api.coingecko.com/api/v3/coins/" + CRYPTO_ID +
            "/market_chart/range?vs_currency=" + FIAT_CURRENCY + "&from=" + strUnixStartTime_ +
            "&to=" + strUnixEndTime_;



    coingeckoUrl_ = QUrl(QString::fromStdString(REQUEST_URL));

    qDebug()<< coingeckoUrl_;
}

std::map<double, double> mainUI::readData(QJsonArray array)
{

    std::map<double, double> dataStorage;
    for(int i = 0; i < array.size(); ++i){
        QJsonArray tempArray = array.at(i).toArray();



        dataStorage.insert({ tempArray.at(0).toDouble(), tempArray.at(1).toDouble() });

    }
    return dataStorage;
}

void mainUI::arrayElemsToArray(QJsonArray jsonArray)
{
    for(int i = 0; i < jsonArray.size(); ++i){
        jsonArray.at(i).toArray();

    }
}

void mainUI::loadData()
{
    // convert all the elements to actual arrays
    arrayElemsToArray(pricesArray_);
    arrayElemsToArray(marketCapsArray_);
    arrayElemsToArray(totalVolumesArray_);

    // loads the parsed and cleaned data into maps
    pricesMap_ = readData(pricesArray_);
    marketCapsMap_ = readData(marketCapsArray_);
    totalVolumesMap_ = readData(totalVolumesArray_);

}

void mainUI::calculateLongestBearTrend()
{

}

void mainUI::calculateHighestVolumeDay()
{

}

void mainUI::giveInvestmentRecommendation()
{

}

void mainUI::testPrintMaps()
{
    for (pricesIterator_=pricesMap_.begin(); pricesIterator_!=pricesMap_.end(); ++pricesIterator_) {
        qDebug()<< "unix time"<<pricesIterator_->first << "and price: "<<pricesIterator_->second;
        break;
    }

    for (marketCapsIterator_=marketCapsMap_.begin(); marketCapsIterator_!=marketCapsMap_.end(); ++marketCapsIterator_) {
        qDebug()<< "unix time"<<marketCapsIterator_->first << "and market cap: "<<marketCapsIterator_->second;
        break;
    }

    for (totalVolumesIterator_=totalVolumesMap_.begin(); totalVolumesIterator_!=totalVolumesMap_.end(); ++totalVolumesIterator_) {
        qDebug()<< "unix time"<<totalVolumesIterator_->first << "and total volume: "<<totalVolumesIterator_->second;
        break;
    }
}

void mainUI::on_executeButton_clicked()
{
    // storing the necessary times in right formats from the user input
    setTimes();

    qDebug() << "Unix start time is: " << timeVars.unixStartTime_;
    qDebug() << "Unix end time is: " << timeVars.unixEndTime_;

    networkManager_->get(QNetworkRequest(coingeckoUrl_));

}

void mainUI::on_pushButton_clicked()
{
    this->close();
}

void mainUI::onResult(QNetworkReply *reply)
{
    // If there are no errors
    if(!reply->error()){

        // So create an object Json Document, by reading into it all the data from the response
        jsonDocument_ = QJsonDocument::fromJson(reply->readAll());

        // Taking from the document root object
        jsonObject_ = jsonDocument_.object();

        pricesArray_ = jsonObject_.value("prices").toArray();
        marketCapsArray_ = jsonObject_.value("market_caps").toArray();
        totalVolumesArray_ = jsonObject_.value("total_volumes").toArray();


        loadData();

        qDebug()<<"pricesmap size: "<<pricesMap_.size();
        qDebug()<<"marketcapsMap size: "<<marketCapsMap_.size();
        qDebug()<<"totalVolumesMap size: "<<totalVolumesMap_.size();

        testPrintMaps();
    }

    reply->deleteLater();
}

