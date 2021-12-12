#include "mainui.h"
#include "ui_mainui.h"

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <vector>
#include <iostream>

bitcoinAnalyzer::mainUI::mainUI(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::mainUI)
{
    ui->setupUi(this);
    initializeGUI();

    networkManager_ = new QNetworkAccessManager(this);
    connect(networkManager_, &QNetworkAccessManager::finished, this, &mainUI::onResult);
}

bitcoinAnalyzer::mainUI::~mainUI()
{
    delete ui;
    delete networkManager_;
}

void bitcoinAnalyzer::mainUI::setTimes()
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

    // convert the unix-times to string for API FETCH request URL
    timeVars.strUnixStartTime_ = std::to_string(timeVars.unixStartTime_);
    timeVars.strUnixEndTime_ = std::to_string(timeVars.unixEndTime_);

    // preparing the correct URL for data fetching
    REQUEST_URL = "https://api.coingecko.com/api/v3/coins/" + CRYPTO_ID +
            "/market_chart/range?vs_currency=" + FIAT_CURRENCY + "&from=" + timeVars.strUnixStartTime_ +
            "&to=" + timeVars.strUnixEndTime_;

    coingeckoUrl_ = QUrl(QString::fromStdString(REQUEST_URL));

}

std::map<double, double> bitcoinAnalyzer::mainUI::readData(QJsonArray array)
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

        if(dataStorage.size() < daysBetween_ && tempArray.at(0).toDouble() != 0 && tempArray.at(1).toDouble() != 0) {
            dataStorage.insert({ tempArray.at(0).toDouble(), tempArray.at(1).toDouble() });
        }
    }
    return dataStorage;
}

void bitcoinAnalyzer::mainUI::arrayElemsToArray(QJsonArray jsonArray)
{
    for(int i = 0; i < jsonArray.size(); ++i){
        jsonArray.at(i).toArray();
    }
}

std::pair<double, double> bitcoinAnalyzer::mainUI::findLowestEntry(std::map<double, double> targetMap)
{
    auto it = std::min_element(std::begin(targetMap), std::end(targetMap),
                               [](const auto& l, const auto& r) { return l.second < r.second; });

    std::pair<double, double> entryWithMinValue = std::make_pair(it->first, it->second);

    return entryWithMinValue;
}

std::pair<double, double> bitcoinAnalyzer::mainUI::findHighestEntry(std::map<double, double> targetMap)
{
    // Reference variable to help find the entry with the highest value
    std::pair<double, double> entryWithMaxValue
            = std::make_pair(0, 0);

    // Iterate in the map to find the required entry
    std::map<double, double>::iterator currentEntry;
    for (currentEntry = targetMap.begin();
         currentEntry != targetMap.end();
         ++currentEntry) {

        /* If this entry's value is more
        than the max value set this entry as the max*/
        if (currentEntry->second > entryWithMaxValue.second) {

            entryWithMaxValue = std::make_pair(
                        currentEntry->first,
                        currentEntry->second);
        }
    }
    return std::make_pair(entryWithMaxValue.first, entryWithMaxValue.second);
}


void bitcoinAnalyzer::mainUI::loadData()
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

void bitcoinAnalyzer::mainUI::calculateLongestBearTrend()
{
    std::vector<double> prices;

    // populating the prices vector that contains all the close prices for the days in the given date period
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
    // update the value to the GUI

    QString numberOfDays = QString::number(longestDownSequence);
    ui->bearTrendLengthEdit->setText(QString(numberOfDays + " days."));
    longestBearTrend_ = longestDownSequence;
}

void bitcoinAnalyzer::mainUI::giveInvestmentRecommendation()
{

    // entryWithMaxValue and  entryWithMinValue is of type (unixTimestamp, price)
    std::pair<double, double> entryWithMaxValue = findHighestEntry(pricesMap_);
    std::pair<double, double> entryWithMinValue= findLowestEntry(pricesMap_);

    long int secondsForMax = (long int) (entryWithMaxValue.first / 1000);
    long int secondsForMin = (long int) (entryWithMinValue.first / 1000);

    // one should sell when the BTC price is at is highest and buy when the price is at its lowest
    QString bestDayToBuy = QString::fromStdString(unixTimeToHumanReadable(secondsForMin));
    QString bestDayToSell = QString::fromStdString(unixTimeToHumanReadable(secondsForMax));

    qDebug()<<"min bitcoin price: "<<entryWithMinValue.first << "and the date when it occured: " <<bestDayToBuy;
    qDebug()<<"max bitcoin price: "<<entryWithMaxValue.first << "and the date when it occured: " << bestDayToSell;


    if ((unsigned int) longestBearTrend_ == daysBetween_) {
        ui->investmentAdviceEdit->setText(QString("You should HOLD bitcoin for in the given date range"));
        return;
    } else {
        ui->investmentAdviceEdit->setText(QString("The best day to buy bitcoin: " + bestDayToBuy +
                                                  " and the the best day to sell bitcoin: " + bestDayToSell + "."));
    }

}

void bitcoinAnalyzer::mainUI::findHighestVolumeDay()
{
    std::pair<double, double> entryWithMaxValue = findHighestEntry(totalVolumesMap_);

    // date as unix timestamp
    QString unixDateWithHighestVolume = QString().setNum(entryWithMaxValue.first, 'g', 15);

    QString totalEurVolume = QString().setNum(entryWithMaxValue.second, 'g', 15);

    long int seconds = (long int) (entryWithMaxValue.first / 1000);

    // uct date
    QString uctDateWithHighestVolume = QString::fromStdString(unixTimeToHumanReadable(seconds));
    qDebug() << QString::fromStdString(unixTimeToHumanReadable(seconds));

    qDebug()<<"unix day with highest volume: "<<unixDateWithHighestVolume.toDouble()<<" and total volume in euros: "<<totalEurVolume;

    // update the value to the GUI
    ui->highestVolumeDayEdit->setText(QString("The date with the highest volume: " + uctDateWithHighestVolume + " and the total volume:  " + totalEurVolume + " euros."));
}


void bitcoinAnalyzer::mainUI::on_executeButton_clicked()
{

    startDate_ = ui->startDateEdit->date();
    endDate_ = ui->endDateEdit->date();

    unsigned int delta = static_cast<unsigned int>(startDate_.daysTo(endDate_));

    // how many days there are in the given date range
    daysBetween_ = delta;

    qDebug() << "Days between: "<<daysBetween_;

    if(daysBetween_ == 0) {
        // clearing the text from the query output labels
        ui->bearTrendLengthEdit->clear();
        ui->highestVolumeDayEdit->clear();
        ui->investmentAdviceEdit->clear();

        ui->statusMessageLabel->setText(QString("<font color='red'>Start and end dates cannot be the same!</font>"));
        return;
    }
    // storing the necessary times in right formats from the user input
    setTimes();

    networkManager_->get(QNetworkRequest(coingeckoUrl_));

}
void bitcoinAnalyzer::mainUI::on_closeButton_clicked()
{
    this->close();
}
void bitcoinAnalyzer::mainUI::onResult(QNetworkReply *reply)
{
    // If there are no errors
    if(!reply->error()){

        // Clear the cached data from previous queries
        clearCachedData();

        // So create an object Json Document, by reading into it all the data from the response
        QJsonDocument jsonDocument = QJsonDocument::fromJson(reply->readAll());

        // Taking from the document root object
        QJsonObject jsonObject = jsonDocument.object();

        pricesArray_ = jsonObject.value("prices").toArray();
        marketCapsArray_ = jsonObject.value("market_caps").toArray();
        totalVolumesArray_ = jsonObject.value("total_volumes").toArray();

        // load the data into STL map containers
        loadData();

        executeQueries();

        ui->statusMessageLabel->setText(QString("<font color='green'>Queries executed successfully!</font>"));

        qDebug()<<"**********************************************   This query ended    **********************************************";

    } else if(reply->error()){
    ui->statusMessageLabel->setText(QString("<font color='red'>Something went wrong with the API GET request, please check"
                                            " your internet connection and try again!</font>"));
    }

    reply->deleteLater();
}

std::string bitcoinAnalyzer::mainUI::unixTimeToHumanReadable(long int seconds, bool showTime)
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

void bitcoinAnalyzer::mainUI::initializeGUI()
{

    ui->startDateEdit->setDate(QDate(2020, 3, 1));
    ui->endDateEdit->setDate(QDate(2021, 8, 1));

    // disabling the possibility to edit the textLineEditBoxes of our queries
    ui->bearTrendLengthEdit->setReadOnly(true);
    ui->highestVolumeDayEdit->setReadOnly(true);
    ui->investmentAdviceEdit->setReadOnly(true);
}

void bitcoinAnalyzer::mainUI::clearCachedData()
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

}

void bitcoinAnalyzer::mainUI::executeQueries()
{
    calculateLongestBearTrend();
    findHighestVolumeDay();
    giveInvestmentRecommendation();
}




