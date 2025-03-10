//
// Created by Elen Soghomonyan on 09.03.25.
//

#ifndef TRADING_ENGINE_TASK_TRADING_ENGINE_HPP
#define TRADING_ENGINE_TASK_TRADING_ENGINE_HPP

#include <set>
#include <string>
#include <vector>

namespace trading {
    struct Order {
        std::string traderId;
        char side;
        int quantity;
        int price;
        int time;
    };

    struct Trade {
        std::string tradeId;
        char sign;
        int quantity;
        int price;
    };

    struct BuyOrderComparator {
        bool operator()(const Order &first, const Order &second) {
            if (first.price != second.price) {
                return first.price > second.price;
            }
            return first.time < second.time;
        }
    };

    struct SellOrderComparator {
        bool operator()(const Order &first, const Order &second) {
            if (first.price != second.price) {
                return first.price < second.price;
            }
            return first.time < second.time;
        }
    };

    class TradingEngine {
    private:
        int _time = 0;
        std::multiset<Order, BuyOrderComparator> _buyOrders;
        std::multiset<Order, SellOrderComparator> _sellOrders;

        std::vector<Trade> mergeTrades(std::vector<Trade> &&trades);

    public:
        std::vector<Trade> processOrder(const Order &newOrder);

        // For testing purposes
        [[nodiscard]] const std::multiset<Order, BuyOrderComparator> &getBuyOrders() const;

        [[nodiscard]] const std::multiset<Order, SellOrderComparator> &getSellOrders() const;
    };

    std::string formatTrades(const std::vector<Trade> &trades);
}
#endif //TRADING_ENGINE_TASK_TRADING_ENGINE_HPP
