#pragma once
#include <string>
#include <map>
#include "OrderBookEntry.h"

class Wallet
{
    public:
        Wallet();
        /** insert currency to the wallet*/
        void insertCurrency(std::string type, double amount);

        /** remove currency from the wallet*/
        bool removeCurrency(std::string type, double amount);

        /** checkif the wallet contains enough currency*/
        bool containsCurrency(std::string type, double amount);

        /** checks of the wallet can fulfill current ask or bid*/
        bool canFulfillOrder(OrderBookEntry order);

        /** update the contents of the wallet assuming the sale was made by the owner of the wallet*/
        void processSale(OrderBookEntry& sale);

        /** current wallet state in the string representation*/
        std::string toString();

        friend std::ostream &operator<<(std::ostream &os, Wallet &wallet);

    private:
        std::map<std::string, double> currencies;

};