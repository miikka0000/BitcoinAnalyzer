#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include <QDateTime>
#include <string>
#include <map>
#include <QNetworkAccessManager>
#include <QJsonArray>
#include <QObject>

namespace Ui {
class mainUI;
}

/**
 * @brief Are the classes of this program are in the bitcoinAnalyzer (main program) namespace.
 */
namespace bitcoinAnalyzer {

/**
 * @brief The mainUI class is the class that contains all the functionality of the main menu.
 */
class mainUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit mainUI(QWidget *parent = nullptr);
    ~mainUI();

    const std::string FIAT_CURRENCY = "eur";
    const std::string CRYPTO_ID = "bitcoin";
    std::string REQUEST_URL;

    // struct that holds all the necessary times in different data types
    struct times {
        QDateTime uctStartTime_;
        QDateTime uctEndTime_;
        uint unixStartTime_;
        uint unixEndTime_;
        std::string strUnixStartTime_;
        std::string strUnixEndTime_;
    };

    struct times timeVars;

    /**
     * @brief initializeGUI function initializes the mainMenu GUI.
     * For example it sets the start date to 1.3.2020 0.00 and the end date to 1.8.2021 0.00
     * so that we can test our programs first query in a convenient way.
     */
    void initializeGUI();

    /**
     * @brief setTimes sets the times given by the user in the mainMenu and stores them into private attributes.
     */
    void setTimes();

    /**
     * @brief unixTimeToHumanReadable converts unix timestamp to human readable UCT-time string.
     * @param amount of seconds from the UNIX Epoch time (January 1st, 1970 at 00:00:00 UTC)
     * @param showTime is by default false but when set to true, it also outputs the time in the result query in the GUI.
     * @return Time (string) as a human readable UCT-time.
     */
    std::string unixTimeToHumanReadable(long int seconds, bool showTime = false);

    /**
     * @brief readData reads the raw data from the API into QJsonArray.
     * @param array is a QJsonArray that contains the desired data which is later transformed to usable data.
     * @return STL map that has the UNIX-timestamp as a key and either BTC price, total volume or market cap as a value.
     */
    std::map<double, double> readData(QJsonArray array);

    /**
     * @brief loadData loads the parsed and cleaned data into pricesMap_, totalVolumesMap_ and marketCapsmap_.
     */
    void loadData();

    /**
     * @brief clearCachedData clears the cached data between queries from all of the maps and arrays (private attributes)
     * to avoid data corruption. We want to start from a clean slate in every new query made by the user.
     */
    void clearCachedData();

    /**
     * @brief arrayElemsToArray converts all the elements of the QJsonArray (given as a parameter) to Arrays.
     * @param jsonArray is a QJsonArray (as of now either pricesArray_, totalVolumesArray_ or marketCapsArray_).
     */
    void arrayElemsToArray(QJsonArray jsonArray);

    /**
     * @brief findHighestEntry finds and returns the key, value -pair that contains the highest value of the whole map given as a parameter.
     * @param targetMap is a STL map data structure from which we find the pair with highest entry.
     * @return STL pair that has the UNIX-timestamp as first element and some highest value
     * (e.g. BTC price, market cap or total trading volume) as the second element.
     */
    std::pair<double, double> findHighestEntry(std::map<double, double> targetMap);

    /**
     * @brief findLowestEntry finds and returns the key, value -pair that contains the lowest value of the whole map given as a parameter.
     * @param targetMap is a STL map data structure from which we find the pair with lowest entry.
     * @return STL pair that has the UNIX-timestamp as first element and some lowest value
     * (e.g. BTC price, market cap or total trading volume) as the second element.
     */
    std::pair<double, double> findLowestEntry(std::map<double, double> targetMap);

    /**
     * @brief calculateLongestBearTrend calculates/finds the longest BTC bear (downward) trend from the given date range and updates the
     * result to the GUI instantly.
     */
    void calculateLongestBearTrend();

    /**
     * @brief findHighestVolumeDay calculates/finds the highest trading volume day from the given date range and updates
     * the result to the GUI instantly.
     */
    void findHighestVolumeDay();

    /**
     * @brief giveInvestmentRecommendation calculates/finds the optimal days to sell and buy or HOLD BTC from
     * the given date range and updates the result to the GUI instantly.
     */
    void giveInvestmentRecommendation();

    /**
     * @brief executeQueries makes function call to the three previosly mentioned query-functions
     * calculateLongestBearTrend(), findHighestVolumeDay() and giveInvestmentRecommendation()
     * thus executing all the three queries and updates the results to the GUI for the user to see.
     */
    void executeQueries();

private slots:

    /**
     * @brief on_executeButton_clicked is a private slot that stores the necessary times in right formats
     * from the user input, calculates the delta of the start and end date and makes get request from the coinGecko API
     * based on the correct URL when the user presses the "Execute" -button in the GUI.
     */
    void on_executeButton_clicked();

    /**
     * @brief on_closeButton_clicked closes the main program when the user clicks the "Close" -button in the GUI.
     */
    void on_closeButton_clicked();

    /**
     * @brief onResult is a private slot to which we connect to in the mainUI class constructor. It basically
     * listens to QNetworkReply and if the reply from the GET request is successful, it reads the data from the API to initial placeholder
     * data storages, then calls the parsing and cleaning functions that transform the data into usable format, and
     * finally calls the executeQueries() function to execute all the three queries and show the results to the user.
     */
    void onResult(QNetworkReply *reply);

private:
    Ui::mainUI *ui;
    QNetworkAccessManager *networkManager_;

    QUrl coingeckoUrl_;

    QDate startDate_;
    QDate endDate_;

    unsigned int daysBetween_;

    int longestBearTrend_;

    // stores the highest closing price for bitcoin in the given date range
    double highestBitcoinPrice_;
    // stores the lowest closing value for bitcoin in the given date range
    double lowestBitcoinPrice_;

    QJsonArray pricesArray_;
    QJsonArray marketCapsArray_;
    QJsonArray totalVolumesArray_;

    // map data structures that hold the relevent data
    std::map<double, double> pricesMap_;
    std::map<double, double> marketCapsMap_;
    std::map<double, double> totalVolumesMap_;

    std::map<QDateTime, double> uctDatePrices_;

    // iterator for later use when iterating through the map
    std::map<double, double>::iterator pricesIterator_ = pricesMap_.begin();

};
}

#endif // MAINUI_H
