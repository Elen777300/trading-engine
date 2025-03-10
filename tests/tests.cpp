//
// Created by Elen Soghomonyan on 10.03.25.
//
#include <cassert>
#include <iostream>

#include "trading_engine.hpp"

void runUnitTests() {
    using namespace trading;
    // Test 1: Basic buy order matching
    {
        TradingEngine engine;

        // Set up the order book with a sell order
        Order sellOrder = {"T1", 'S', 5, 100, 0};
        engine.processOrder(sellOrder);

        // Try to match with a buy order
        Order buyOrder = {"T2", 'B', 3, 100, 1};
        auto trades = engine.processOrder(buyOrder);

        // Expected trades: T2 buys 3 @ 100, T1 sells 3 @ 100
        // Note: After consolidation and sorting, the trades may be ordered differently
        assert(trades.size() == 2);

        // Just check the values, not comparing entire objects
        bool foundBuyTrade = false;
        bool foundSellTrade = false;

        for (const auto &trade: trades) {
            if (trade.tradeId == "T2" && trade.sign == '+' && trade.quantity == 3 && trade.price == 100) {
                foundBuyTrade = true;
            }
            if (trade.tradeId == "T1" && trade.sign == '-' && trade.quantity == 3 && trade.price == 100) {
                foundSellTrade = true;
            }
        }

        assert(foundBuyTrade && foundSellTrade);

        // Check remaining order book
        auto sellOrders = engine.getSellOrders();
        assert(sellOrders.size() == 1);
        assert(sellOrders.begin()->quantity == 2);
    }

    // Test 2: Buy order matching with multiple sell orders
    {
        TradingEngine engine;

        // Set up the order book with sell orders
        engine.processOrder({"T1", 'S', 5, 100, 0});
        engine.processOrder({"T2", 'S', 3, 90, 1});

        // Try to match with a buy order that matches both
        auto trades = engine.processOrder({"T3", 'B', 8, 100, 2});

        // Expected trades: T3 buys 3 @ 90 and 5 @ 100, T2 sells 3 @ 90, T1 sells 5 @ 100
        assert(trades.size() == 4);

        // Check for expected trades
        bool foundT3Buy90 = false;
        bool foundT3Buy100 = false;
        bool foundT2Sell = false;
        bool foundT1Sell = false;

        for (const auto &trade: trades) {
            if (trade.tradeId == "T3" && trade.sign == '+' && trade.quantity == 3 && trade.price == 90) {
                foundT3Buy90 = true;
            } else if (trade.tradeId == "T3" && trade.sign == '+' && trade.quantity == 5 && trade.price == 100) {
                foundT3Buy100 = true;
            } else if (trade.tradeId == "T2" && trade.sign == '-' && trade.quantity == 3 && trade.price == 90) {
                foundT2Sell = true;
            } else if (trade.tradeId == "T1" && trade.sign == '-' && trade.quantity == 5 && trade.price == 100) {
                foundT1Sell = true;
            }
        }

        assert(foundT3Buy90 && foundT3Buy100 && foundT2Sell && foundT1Sell);

        // Check order book is empty
        assert(engine.getSellOrders().empty());
    }

    // Test 3: Basic sell order matching
    {
        TradingEngine engine;

        // Set up the order book with a buy order
        engine.processOrder({"T1", 'B', 5, 100, 0});

        // Try to match with a sell order
        auto trades = engine.processOrder({"T2", 'S', 3, 100, 1});

        // Expected trades: T2 sells 3 @ 100, T1 buys 3 @ 100
        assert(trades.size() == 2);

        // Check for expected trades
        bool foundT1Buy = false;
        bool foundT2Sell = false;

        for (const auto &trade: trades) {
            if (trade.tradeId == "T1" && trade.sign == '+' && trade.quantity == 3 && trade.price == 100) {
                foundT1Buy = true;
            } else if (trade.tradeId == "T2" && trade.sign == '-' && trade.quantity == 3 && trade.price == 100) {
                foundT2Sell = true;
            }
        }

        assert(foundT1Buy && foundT2Sell);

        // Check remaining order book
        auto buyOrders = engine.getBuyOrders();
        assert(buyOrders.size() == 1);
        assert(buyOrders.begin()->quantity == 2);
    }

    // Test 4: Order consolidation
    {
        TradingEngine engine;

        // Set up the order book with multiple sell orders from same trader
        engine.processOrder({"T1", 'S', 3, 100, 0});
        engine.processOrder({"T1", 'S', 2, 100, 1});

        // Match with a buy order
        auto trades = engine.processOrder({"T2", 'B', 5, 100, 2});

        // Expected: Trades get consolidated for T1
        assert(trades.size() == 2);

        // Check for expected trades
        bool foundT2Buy = false;
        bool foundT1Sell = false;

        for (const auto &trade: trades) {
            if (trade.tradeId == "T2" && trade.sign == '+' && trade.quantity == 5 && trade.price == 100) {
                foundT2Buy = true;
            } else if (trade.tradeId == "T1" && trade.sign == '-' && trade.quantity == 5 && trade.price == 100) {
                foundT1Sell = true;
            }
        }

        assert(foundT2Buy && foundT1Sell);
    }

    // Test 5: No matching orders
    {
        TradingEngine engine;

        // Add a sell order at high price
        engine.processOrder({"T1", 'S', 5, 100, 0});

        // Add a buy order at low price
        auto trades = engine.processOrder({"T2", 'B', 5, 90, 1});

        // No trades should be executed
        assert(trades.empty());

        // Both orders should be in the book
        assert(engine.getSellOrders().size() == 1);
        assert(engine.getBuyOrders().size() == 1);
    }

    // Test 6: Time priority
    {
        TradingEngine engine;

        // Add sell orders at same price, different times
        engine.processOrder({"T1", 'S', 3, 100, 0}); // Earlier
        engine.processOrder({"T2", 'S', 3, 100, 1}); // Later

        // Match with a buy order
        auto trades = engine.processOrder({"T3", 'B', 4, 100, 2});

        // T1's order should be fully matched first, then partial match with T2
        assert(trades.size() == 3);

        // Check for expected trades
        bool foundT3Buy = false;
        bool foundT1Sell = false;
        bool foundT2Sell = false;

        for (const auto &trade: trades) {
            if (trade.tradeId == "T3" && trade.sign == '+' && trade.quantity == 4 && trade.price == 100) {
                foundT3Buy = true;
            } else if (trade.tradeId == "T1" && trade.sign == '-' && trade.quantity == 3 && trade.price == 100) {
                foundT1Sell = true;
            } else if (trade.tradeId == "T2" && trade.sign == '-' && trade.quantity == 1 && trade.price == 100) {
                foundT2Sell = true;
            }
        }

        assert(foundT3Buy && foundT1Sell && foundT2Sell);

        // T2 should have a remaining order of 2
        auto sellOrders = engine.getSellOrders();
        assert(sellOrders.size() == 1);
        assert(sellOrders.begin()->traderId == "T2");
        assert(sellOrders.begin()->quantity == 2);
    }

    // Test 7: Example from the problem statement
    {
        TradingEngine engine;

        // Process the orders from the example
        engine.processOrder({"T1", 'B', 5, 30, 0});
        engine.processOrder({"T2", 'S', 5, 70, 0});
        engine.processOrder({"T3", 'B', 1, 40, 0});
        engine.processOrder({"T4", 'S', 2, 60, 0});
        engine.processOrder({"T5", 'S', 3, 70, 0});
        engine.processOrder({"T6", 'S', 20, 80, 0});
        engine.processOrder({"T7", 'S', 1, 50, 0});
        engine.processOrder({"T2", 'S', 5, 70, 0});

        // Order 9: T1 B 1 50
        auto trades1 = engine.processOrder({"T1", 'B', 1, 50, 0});
        assert(trades1.size() == 2);
        assert(formatTrades(trades1) == "T1+1@50 T7-1@50");

        // Order 10: T1 B 3 60
        auto trades2 = engine.processOrder({"T1", 'B', 3, 60, 0});
        assert(trades2.size() == 2);
        assert(formatTrades(trades2) == "T1+2@60 T4-2@60");

        // Order 11: T7 S 2 50
        auto trades3 = engine.processOrder({"T7", 'S', 2, 50, 0});
        assert(trades3.size() == 2);
        assert(formatTrades(trades3) == "T1+1@60 T7-1@60");

        // Order 12: T8 B 10 90
        auto trades4 = engine.processOrder({"T8", 'B', 10, 90, 0});
        assert(trades4.size() == 5);

        // For this specific test, we use direct string comparison as the output must match exactly
        std::string expectedOutput = "T2-6@70 T5-3@70 T7-1@50 T8+1@50 T8+9@70";
        std::string actualOutput = formatTrades(trades4);
        assert(actualOutput == expectedOutput);
    }

    std::cout << "All unit tests passed!" << std::endl;

}