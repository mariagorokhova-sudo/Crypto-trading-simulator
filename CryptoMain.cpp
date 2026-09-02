#include "CryptoMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <iostream>
#include <map>
#include <vector>


CryptoMain::CryptoMain()
{
    
}

//Initialize simulation

void CryptoMain::init()
{
    currentTime = orderBook.getEarliestTime();
    previousTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10.0);

    int userOption;
    while (running)
    {
        printMenu();
        userOption = getUserOption();
        processUserOption(userOption);
    }
}


//Menu functions

void CryptoMain::printMenu()
{
    // 1 print help
    std::cout << "1: Help" << std::endl;
    // 2 print exchange stats
    std::cout << "2: Exchange Statistics" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an Ask" << std::endl;
    // 4 make a bid
    std::cout << "4: Make a Bid" << std::endl;
    // 5 print wallet
    std::cout << "5: Wallet Status" << std::endl;
    // 6 continue
    std::cout << "6: Continue" << std::endl;
    // 7 exit
    std::cout << "7: Exit" << std::endl;
    std::cout << "++++++++++++++" << std::endl;

    std::cout << "Current time is " << currentTime << std::endl;
}

int CryptoMain::getUserOption()
{
    std::cout << "Please choose an option from 1 to 7." << std::endl;
    std::string userOption;
    std::getline(std::cin, userOption);
    try{
        std::size_t pos;
        int option = std::stoi(userOption, &pos);
        if (pos != userOption.size())
        {
            return -1;
        }
        std::cout << "You've chosen " << option << std::endl;
        return option;
    }catch(const std::exception& e)
    {
        return -1;
    }
}

void CryptoMain::processUserOption(int userOption)
{
    std::map<int, void (CryptoMain::*)()> menu;
    menu[1] = &CryptoMain::printHelp;
    menu[2] = &CryptoMain::printStats;
    menu[3] = &CryptoMain::enterAsk;
    menu[4] = &CryptoMain::enterBid;
    menu[5] = &CryptoMain::printWallet;
    menu[6] = &CryptoMain::gotoNextTimeframe;
    menu[7] = &CryptoMain::exitMenu;

    if (menu.count(userOption))
    {
        (this->*menu[userOption])();
    }
    else
    {
        std::cout << "Wrong input. Please choose 1-7." << std::endl;
    }
}

void CryptoMain::printHelp()
{
    std::cout << "Your aim is to gain money. Analyse market, make bids and offers, succeed!" << std::endl;
}

void CryptoMain::printStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> askEntries = orderBook.getOrders(OrderBookType::ask,
                                                                  p,
                                                                  currentTime);
        std::cout << "Asks seen: " << askEntries.size() << std::endl;
        std::cout << "Maximum ask is " << OrderBook::getHighestPrice(askEntries) << std::endl;
        std::cout << "Minimum ask is " << OrderBook::getLowestPrice(askEntries) << std::endl;
        std::cout << "Average ask is " << OrderBook::computeAveragePrice(askEntries) << std::endl;
        std::cout << "Price change since last timeframe is " << OrderBook::avgPriceChangeSinceLastTimeframe(orderBook.getOrders(OrderBookType::ask, 
                                                                                                                               p, 
                                                                                                                               previousTime), 
                                                                                                           askEntries) << std::endl;

        std::vector<OrderBookEntry> bidEntries = orderBook.getOrders(OrderBookType::bid,
                                                                     p,
                                                                     currentTime);
        std::cout << "Bids seen: " << bidEntries.size() << std::endl;
        std::cout << "Maximum bid is " << OrderBook::getHighestPrice(bidEntries) << std::endl;
        std::cout << "Minimum bid is " << OrderBook::getLowestPrice(bidEntries) << std::endl;
        std::cout << "Average bid is " << OrderBook::computeAveragePrice(bidEntries) << std::endl;
        std::cout << "Price change since last timeframe is " << OrderBook::avgPriceChangeSinceLastTimeframe(orderBook.getOrders(OrderBookType::bid, 
                                                                                                                               p, 
                                                                                                                               previousTime), 
                                                                                                           bidEntries) << std::endl;

        std::cout << "Price spread is " << OrderBook::computePriceSpread(askEntries, bidEntries) << std::endl;
    }
}


void CryptoMain::enterAsk()
{
    std::cout << "Make an ask - please enter: product, price, amount (eg. ETH/BTC, 200, 0.5)." << std::endl;

    std::string input;
    std::getline(std::cin, input);
    std::cout << "You've entered " << input << std::endl;

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "Bad input, please try again." << std::endl;
    }
    else{
        try{
            OrderBookEntry obe = CSVReader::stringsToOBE(tokens[1], 
                                                     tokens[2], 
                                                     currentTime, 
                                                     tokens[0], 
                                                     OrderBookType::ask);
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet can fulfill order." << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Not enough funds." << std::endl;
            }
        }catch (const std::exception &e)
        {
            std::cout << "CryptoMain::enterAsk: Bad input." << std::endl;
        }
    }
}

void CryptoMain::enterBid()
{
    std::cout << "Make a bid - please enter: product, price, amount (eg. ETH/BTC, 100, 1)." << std::endl;
    std::string input;
    std::getline(std::cin, input);
    std::cout << "You've entered " << input << std::endl;

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "Bad input, please try again." << std::endl;
    }
    else
    {
        try
        {
            OrderBookEntry obe = CSVReader::stringsToOBE(tokens[1],
                                                         tokens[2],
                                                         currentTime,
                                                         tokens[0],
                                                         OrderBookType::bid);
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet can fulfill order." << std::endl;
                orderBook.insertOrder(obe);
            }
            else
            {
                std::cout << "Not enough funds." << std::endl;
            }
        }
        catch (const std::exception &e)
        {
            std::cout << "CryptoMain::enterBid: Bad input." << std::endl;
        }
    }
}

void CryptoMain::printWallet()
{
    std::cout << wallet << std::endl;
}

void CryptoMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame." << std::endl;

    for (std::string &p : orderBook.getKnownProducts())
    {
        std::cout << "Matching " << p << std::endl;
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Number of sales: " << sales.size() << std::endl;
        for (OrderBookEntry &sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " Sale amount: " << sale.amount << std::endl;
            if (sale.username == "simuser")
            {
                wallet.processSale(sale);
            }
        }
    }

    previousTime = currentTime;
    currentTime = orderBook.getNextTime(currentTime);
}

void CryptoMain::exitMenu()
{
    std::cout << "See you next time!" << std::endl;
    running = false;
}

