#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>


class OrderBook
{
    public:
    /** construct, reading a csv datafile*/
        OrderBook(std::string filename);
    /** return vector of all products in the dataset*/
        std::vector<std::string> getKnownProducts();
    /** return vector of OrderBook entries according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type,
                                              std::string product,
                                              std::string timestamp);


        std::string getEarliestTime();
        std::string getNextTime(const std::string& timestamp);

        void insertOrder(OrderBookEntry &order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighestPrice(const std::vector<OrderBookEntry>& orders);
        static double getLowestPrice(const std::vector<OrderBookEntry> &orders);
        /** returns the difference between the minimum ask and the maximum bid in the current timeframe, 
         * requires a vector of asks and a vector of bids, returns a double*/
        static double computePriceSpread(const std::vector<OrderBookEntry> &asks,
                                         const std::vector<OrderBookEntry> &bids);
        /** computes average price in the orders set, requires a vector of orders, returns a double*/
        static double computeAveragePrice(const std::vector<OrderBookEntry> &orders);
        /** computes difference between the average price in the current timeframe and in the previus one
         * requires a vector of orders for the current timestamp and for the previus one, reurns a double*/
        static double avgPriceChangeSinceLastTimeframe(const std::vector<OrderBookEntry> &ordersTimestamp1,
                                                       const std::vector<OrderBookEntry> &ordersTimestamp2);

    private:
        std::vector<OrderBookEntry> orders;
};