#ifndef DATAPARSER_H
#define DATAPARSER_H

#include <list>
#include <QString>
#include <map>
#include <QJsonObject>
#include <memory>

namespace bitcoinAnalyzer
{
class dataParser
{

struct bitcoinData {
    char id;
    char startPointTime;
    char startingPointTime;
    char endPointTime;

};
public:
    dataParser();

private:
    std::shared_ptr<bitcoinData> marketdata_;
};

}
#endif // DATAPARSER_H
