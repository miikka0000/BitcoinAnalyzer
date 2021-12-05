#ifndef MAINUI_H
#define MAINUI_H



#include <QMainWindow>
#include <QDateTime>
#include <string>
#include <map>
#include <QNetworkAccessManager>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>


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
    std::string REQUEST_URL;

    struct times {
        QDateTime uctStartTime_;
        QDateTime uctEndTime_;
        uint unixStartTime_;
        uint unixEndTime_;
    };

    struct times timeVars;

    void initializeGUI();

    void setTimes();
    std::string unixTimeToHumanReadable(long int seconds, bool showTime = false);

    std::map<double, double> readData(QJsonArray array);
    void loadData();
    void clearCachedData();

    void arrayElemsToArray(QJsonArray jsonArray);

    void calculateLongestBearTrend();
    void findHighestVolumeDay(std::map<double, double> targetMap);
    void giveInvestmentRecommendation();

    void executeQueries();

private slots:
    void on_executeButton_clicked();

    void on_closeButton_clicked();

    void onResult(QNetworkReply *reply);

private:
    Ui::mainUI *ui;
    QNetworkAccessManager *networkManager_;

    std::string strUnixStartTime_;
    std::string strUnixEndTime_;
    QUrl coingeckoUrl_;

    QDate startDate_;
    QDate endDate_;

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

    // iterator for later use when iterating through the map
    std::map<double, double>::iterator pricesIterator_ = pricesMap_.begin();

};

#endif // MAINUI_H
