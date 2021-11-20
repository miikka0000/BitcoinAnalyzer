#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include <QJsonObject>
#include <memory>
#include <string>
#include <map>
#include <QUrl>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QJsonValue>

namespace Ui {
class mainUI;
}

class QNetworkAccessManager;

class mainUI : public QMainWindow
{
    Q_OBJECT


public:
    explicit mainUI(QWidget *parent = nullptr);
    ~mainUI();

    const std::string FIAT_CURRENCY = "eur";
    const std::string CRYPTO_ID = "bitcoin";

    struct times {
        QDateTime uctStartTime_;
        QDateTime uctEndTime_;
        uint unixStartTime_;
        uint unixEndTime_;
    };
    struct times timeVars;
    std::string REQUEST_URL;

    void setTimes();
    std::map<double, double> readData(QJsonArray array);

    void arrayElemsToArray(QJsonArray jsonArray);

    void loadData();



    void testPrintMaps();




private slots:
    void on_executeButton_clicked();

    void on_pushButton_clicked();

    void onResult(QNetworkReply *reply);

private:
    Ui::mainUI *ui;
    QNetworkAccessManager *networkManager_;


    std::string strUnixStartTime_;
    std::string strUnixEndTime_;
    QUrl coingeckoUrl_;

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



    // iterators for later use when iterating through maps
    std::map<double, double>::iterator pricesIterator_ = pricesMap_.begin();
    std::map<double, double>::iterator marketCapsIterator_ = marketCapsMap_.begin();
    std::map<double, double>::iterator totalVolumesIterator_ = totalVolumesMap_.begin();



};

#endif // MAINUI_H
