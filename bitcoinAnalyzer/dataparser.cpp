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

}

std::map<double, double> dataParser::readData(QJsonArray array)
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

void dataParser::arrayElemsToArray(QJsonArray jsonArray)
{
    for(int i = 0; i < jsonArray.size(); ++i){
        jsonArray.at(i).toArray();

    }
}

void dataParser::loadData()
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

std::string dataParser::unixTimeToHumanReadable(long seconds, bool showTime)
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

}












