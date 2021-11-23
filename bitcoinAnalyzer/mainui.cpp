#include "mainui.h"
#include "ui_mainui.h"
#include "dataparser.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QDateTime>
#include <QTime>
#include <bits/stdc++.h>


mainUI::mainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainUI)
{
    ui->setupUi(this);
    initializeGUI();

    networkManager_ = new QNetworkAccessManager(this);
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
    timeVars.uctEndTime_ = ui->endDateEdit->dateTime();

    // storing the time value as UTC-time
    timeVars.uctStartTime_.setTimeSpec(Qt::UTC);
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

    // This array contains the closest to midnight [unixTimeStamp, value] subarrays
    QJsonArray transformedArray;

    int arraySizeCounter = 0;
    for(int i = 0; i < array.size(); ++i){

        QJsonArray tempArray;

        if (daysBetween_ < 1) {

            tempArray = array.last().toArray();

        }

        else if (daysBetween_ >= 1 && daysBetween_ < 90){

            if(arraySizeCounter % 24 == 0) {

                tempArray = array.at(i).toArray();

            }

        } else if (daysBetween_ >= 90) {
            tempArray = array.at(i).toArray();


        }


        ++arraySizeCounter;

        if(dataStorage.size() < daysBetween_) {
            dataStorage.insert({ tempArray.at(0).toDouble(), tempArray.at(1).toDouble() });
        }

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

    qDebug()<<"pricesMap_ size (amount of days): "<<pricesMap_.size();
    qDebug()<<"marketCapsMap_ size (amount of days): "<<marketCapsMap_.size();
    qDebug()<<"totalVolumesMap_ size (amount of days): "<<totalVolumesMap_.size();

    qDebug()<<"days between: "<<daysBetween_;

}

void mainUI::calculateLongestBearTrend()
{

    std::vector<double> prices;


    for (pricesIterator_=pricesMap_.begin(); pricesIterator_!=pricesMap_.end(); ++pricesIterator_) {


        prices.push_back(pricesIterator_->second);

    }


    int downSequence = 1;
    int longestDownSequence = 1;

    for (int i = 1; i < (int) prices.size(); i++) {

        try {



            if(prices.at(i) <= prices.at(i - 1)) {
                downSequence++;

            } else {
                if(downSequence > longestDownSequence) {
                    longestDownSequence = downSequence;
                }
                downSequence = 1;
            }

            if (downSequence > longestDownSequence) {
                longestDownSequence = downSequence;
            }

        }

        catch (...) {

            break;
        }
    }
    qDebug()<<"vector size: "<< prices.size();

    qDebug()<<QString::number(longestDownSequence);
    ui->bearTrendLengthEdit->setText(QString::number(longestDownSequence));

}



void mainUI::giveInvestmentRecommendation()
{

}



void mainUI::findHighestVolumeDay(std::map<double, double> targetMap)
{
    // Reference variable to help find
    // the entry with the highest value
    std::pair<double, double> entryWithMaxValue
            = std::make_pair(0, 0);

    // Iterate in the map to find the required entry
    std::map<double, double>::iterator currentEntry;
    for (currentEntry = targetMap.begin();
         currentEntry != targetMap.end();
         ++currentEntry) {

        // If this entry's value is more
        // than the max value
        // Set this entry as the max
        if (currentEntry->second
                > entryWithMaxValue.second) {

            entryWithMaxValue
                    = std::make_pair(
                        currentEntry->first,
                        currentEntry->second);
        }
    }


    // date as unix timestamp
    QString unixDateWithHighestVolume = QString().setNum(entryWithMaxValue.first, 'g', 15);

    QString totalEurVolume = QString().setNum(entryWithMaxValue.second, 'g', 15);

    long int seconds = (long int) (entryWithMaxValue.first / 1000);

    // uct date
    QString uctDateWithHighestVolume = QString::fromStdString(unixTimeToHumanReadable(seconds));
    qDebug() << QString::fromStdString(unixTimeToHumanReadable(seconds));

    qDebug()<<"unix day with highest volume: "<<unixDateWithHighestVolume.toDouble()<<" and total volume in euros: "<<totalEurVolume;

    ui->highestVolumeDayEdit->setText(QString("The date with the highest volume: " + uctDateWithHighestVolume + " and the total volume:  " + totalEurVolume + " euros."));

}


void mainUI::on_executeButton_clicked()
{
    // storing the necessary times in right formats from the user input
    setTimes();

    qDebug() << "Unix start time is: " << timeVars.unixStartTime_;
    qDebug() << "Unix end time is: " << timeVars.unixEndTime_;

    networkManager_->get(QNetworkRequest(coingeckoUrl_));

    qDebug()<<"new GET request created!";

    startDate_ = ui->startDateEdit->date();
    endDate_ = ui->endDateEdit->date();

    unsigned int delta = static_cast<unsigned int>(startDate_.daysTo(endDate_));

    if(delta > 1) {
        daysBetween_ = delta + 1;
    } else {
        daysBetween_ = delta;
    }


}

void mainUI::on_closeButton_clicked()
{
    this->close();
}

void mainUI::onResult(QNetworkReply *reply)
{
    // If there are no errors
    if(!reply->error()){
        clearCachedData();
        qDebug()<<"new get request arrived, fetching reply...";

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

        calculateLongestBearTrend();
        findHighestVolumeDay(totalVolumesMap_);
        giveInvestmentRecommendation();

        qDebug()<<"''''''''''''''''''''''''''''''''''''   This query ended '''''''''''''''''''''''''''''''''''''";




    }

    reply->deleteLater();
}



std::string mainUI::unixTimeToHumanReadable(long int seconds, bool showTime)
{


    std::string humanReadableTime = "";



    int daysInAMonth[] = { 31, 28, 31, 30, 31, 30,
                           31, 31, 30, 31, 30, 31 };

    long int currentYear, daysTillNow, extraTime,
            extraDays, index, date, month, hours,
            minutes, secs, flag = 0;


    daysTillNow = seconds / (24 * 60 * 60);
    extraTime = seconds % (24 * 60 * 60);
    currentYear = 1970;

    while (daysTillNow >= 365) {
        if (currentYear % 400 == 0
                || (currentYear % 4 == 0
                    && currentYear % 100 != 0)) {
            daysTillNow -= 366;
        }
        else {
            daysTillNow -= 365;
        }
        currentYear += 1;
    }


    extraDays = daysTillNow + 1;

    if (currentYear % 400 == 0
            || (currentYear % 4 == 0
                && currentYear % 100 != 0))
        flag = 1;

    // calculating the month and date from the unix timestamp
    month = 0, index = 0;
    if (flag == 1) {
        while (true) {

            if (index == 1) {
                if (extraDays - 29 < 0)
                    break;
                month += 1;
                extraDays -= 29;
            }
            else {
                if (extraDays
                        - daysInAMonth[index]
                        < 0) {
                    break;
                }
                month += 1;
                extraDays -= daysInAMonth[index];
            }
            index += 1;
        }
    }
    else {
        while (true) {

            if (extraDays
                    - daysInAMonth[index]
                    < 0) {
                break;
            }
            month += 1;
            extraDays -= daysInAMonth[index];
            index += 1;
        }
    }


    if (extraDays > 0) {
        month += 1;
        date = extraDays;
    }
    else {
        if (month == 2 && flag == 1)
            date = 29;
        else {
            date = daysInAMonth[month - 1];
        }
    }

    hours = extraTime / 3600;
    minutes = (extraTime % 3600) / 60;
    secs = (extraTime % 3600) % 60;

    humanReadableTime += std::to_string(date);
    humanReadableTime += "/";
    humanReadableTime += std::to_string(month);
    humanReadableTime += "/";
    humanReadableTime += std::to_string(currentYear);

    if (showTime == true) {
        humanReadableTime += " ";
        humanReadableTime += std::to_string(hours);
        humanReadableTime += ":";
        humanReadableTime += std::to_string(minutes);
        humanReadableTime += ":";
        humanReadableTime += std::to_string(secs);
    }

    return humanReadableTime;
}


void mainUI::initializeGUI()
{
    ui->endDateEdit->setTime(QTime(1, 0, 0));

    ui->startDateEdit->setDate(QDate(2020, 3, 1));
    ui->endDateEdit->setDate(QDate(2021, 8, 1));

    // disabling the possibility to edit the humanReadableTimewers of our queries
    ui->bearTrendLengthEdit->setReadOnly(true);
    ui->highestVolumeDayEdit->setReadOnly(true);
    ui->investmentAdviceEdit->setReadOnly(true);
}

void mainUI::clearCachedData()
{
    pricesMap_.clear();
    totalVolumesMap_.clear();
    marketCapsMap_.clear();

    while(pricesArray_.count()) {
        pricesArray_.pop_back();
    }

    while(marketCapsArray_.count()) {
        marketCapsArray_.pop_back();
    }

    while(totalVolumesArray_.count()) {
        totalVolumesArray_.pop_back();
    }

    //qDebug()<< "array size after cleanup: "<<totalVolumesArray_.size();


}




