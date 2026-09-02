#include "OrderBook.h"
#include "CSVReader.h"
#include "CryptoMain.h"
#include <map>
#include <algorithm>

//OrderBook processing functions

OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string, bool> prodMap;
    for (OrderBookEntry& e: orders)
    {
        prodMap[e.product] = true;
    }
    // converting map to vector
    for (const std::pair<std::string, bool> &productStringBoolPair : prodMap)
    {
        products.push_back(productStringBoolPair.first);
    }

    return products;
}

std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type,
                                                 std::string product,
                                                 std::string timestamp)
{
    std::vector<OrderBookEntry> ordersSub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
                           e.product == product &&
                           e.timestamp == timestamp)
        {
            ordersSub.push_back(e);
        }
    }
    return ordersSub;
}

//Bounch of statistical functions

double OrderBook::getHighestPrice(const std::vector<OrderBookEntry> &orders)
{
    if (orders.empty())
    {
        return 0.0;
    }

    double maxPrice = orders[0].price;
    for (const OrderBookEntry &e : orders)
    {
        if (e.price > maxPrice)
        {
            maxPrice = e.price;
        }
    }
    return maxPrice;
}

double OrderBook::getLowestPrice(const std::vector<OrderBookEntry> &orders)
{
    if (orders.empty())
    {
        return 0.0;
    }

    double minPrice = orders[0].price;
    for (const OrderBookEntry &e : orders)
    {
        if (e.price < minPrice)
        {
            minPrice = e.price;
        }
    }
    return minPrice;
}

double OrderBook::avgPriceChangeSinceLastTimeframe(const std::vector<OrderBookEntry> &ordersTimestamp1,
                                                   const std::vector<OrderBookEntry> &ordersTimestamp2)
{
    return (computeAveragePrice(ordersTimestamp2) - computeAveragePrice(ordersTimestamp1));
}

double OrderBook::computePriceSpread(const std::vector<OrderBookEntry> &asks,
                                     const std::vector<OrderBookEntry> &bids)
{
    return getLowestPrice(asks) - getHighestPrice(bids);
}

double OrderBook::computeAveragePrice(const std::vector<OrderBookEntry> &orders)
{
    if (orders.empty())
    {
        return 0.0;
    }
    double sum = 0.0;
    for (const OrderBookEntry &e : orders)
    {
        sum += e.price;
    }
    return sum / orders.size();
}

//Time simulation functions

std::string OrderBook::getEarliestTime()
{
    std::string earliestTime = orders[0].timestamp;
    for (const OrderBookEntry& e : orders)
    {
        if( e.timestamp < earliestTime)
        {
            earliestTime = e.timestamp;
        }
    }
    return earliestTime;
}

std::string OrderBook::getNextTime(const std::string& timestamp)
{
    std::string nextTimestamp = "";

    for (OrderBookEntry &e : orders)
    {
        if (e.timestamp > timestamp)
        {
            nextTimestamp = e.timestamp;
            break;
        }
    }
    if (nextTimestamp == "")
    {
        nextTimestamp = getEarliestTime();
    }
    return nextTimestamp;
}

void OrderBook::insertOrder(OrderBookEntry &order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

//Matching engine

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
    std::vector<OrderBookEntry> asks = OrderBook::getOrders(OrderBookType::ask, product, timestamp);
    std::vector<OrderBookEntry> bids = OrderBook::getOrders(OrderBookType::bid, product, timestamp);

    std::vector<OrderBookEntry> sales;

    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);

    for (OrderBookEntry& a : asks)
    {
        for(OrderBookEntry& b : bids)
        {
            if (b.price >= a.price)
            {
                OrderBookEntry sale{a.price, 0, timestamp, product, OrderBookType::asksale};

                if (b.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }

                if (a.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::asksale;
                }

                if (b.amount == a.amount)
                {
                    sale.amount = a.amount;
                    sales.push_back(sale);
                    b.amount = 0;
                    break;
                }
                if (b.amount > a.amount)
                {
                    sale.amount = a.amount;
                    sales.push_back(sale);
                    b.amount = b.amount - a.amount;
                    break;
                }
            
                if (b.amount < a.amount && b.amount > 0)
                {
                    sale.amount = b.amount;
                    sales.push_back(sale);
                    a.amount = a.amount - b.amount;
                    b.amount = 0;
                    continue;
                }
            }
        }
    }
    return sales;
}