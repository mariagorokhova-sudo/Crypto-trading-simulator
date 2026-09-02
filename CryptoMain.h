#pragma once
#include <vector>
#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"

class CryptoMain
{
    public:
        CryptoMain();
        void init();

    private:
        bool running = true;
        void printMenu();
        int getUserOption();
        void printHelp();
        void printStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        void exitMenu();
        void processUserOption(int userOption);
        void loadOrderBook();

        std::string currentTime;
        std::string previousTime;

        OrderBook orderBook{"20200317.csv"};
        Wallet wallet{};
};