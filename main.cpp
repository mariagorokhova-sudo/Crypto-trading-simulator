#include <iostream>
#include <string>
#include <vector>
#include "OrderBookEntry.h"
#include "CryptoMain.h"
#include "CSVReader.h"
#include "Wallet.h"

// Entry point for the crypto trading simulation
int main()
{
    CryptoMain app{};
    app.init();

    return 0;
}