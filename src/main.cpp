//
// Created by Elen Soghomonyan on 09.03.25.
//
#include <iostream>
#include <sstream>
#include <string>

#include "trading_engine.hpp"

int main() {
    using namespace trading;

    TradingEngine engine;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) continue;

        std::istringstream is{line};

        std::string traderId;
        char side;
        int quantity;
        int price;

        if (!(is >> traderId >> side >> quantity >> price)) {
            std::cout << "Wrong input format: " << line << std::endl;
            std::cout << "Please enter in <trader id> <B/S> <quantity> <price> format" << std::endl;
            continue;
        }

        auto trades = engine.processOrder({traderId, side, quantity, price, 0});

        if (!trades.empty()) {
            std::cout << formatTrades(trades) << std::endl;
        }
    }
}
