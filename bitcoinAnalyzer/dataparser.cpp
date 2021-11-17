#include "dataparser.h"

#include <string>
#include <iostream>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>
#include <QJsonArray>
#include <QString>


namespace bitcoinAnalyzer {
dataParser::dataParser()
{
    marketdata_ = std::make_shared<bitcoinData>();
}

}
