//
// Created by Elen Soghomonyan on 09.03.25.
//
#include <sstream>

#include "trading_engine.hpp"

namespace trading {
    std::vector<Trade> TradingEngine::processOrder(const Order &newOrder) {
        std::vector<Trade> trades;

        Order aggressor = newOrder;
        aggressor.time = _time++;

        // TODO get rid of the code duplication
        if (aggressor.side == 'B') {
            while (aggressor.quantity > 0 && !_sellOrders.empty()) {
                auto sellIt = _sellOrders.begin();

                if (sellIt->price > aggressor.price) {
                    break;
                }

                int tradeQuantity = std::min(aggressor.quantity, sellIt->quantity);
                Trade buyTrade = {aggressor.traderId, '+', tradeQuantity, sellIt->price};
                Trade sellTrade = {sellIt->traderId, '-', tradeQuantity, sellIt->price};

                trades.push_back(buyTrade);
                trades.push_back(sellTrade);

                aggressor.quantity -= sellIt->quantity;

                Order updatedSellOrder = *sellIt;
                updatedSellOrder.quantity -= tradeQuantity;
                _sellOrders.erase(sellIt);

                if (updatedSellOrder.quantity > 0) {
                    _sellOrders.insert(updatedSellOrder);
                }
            }
        } else if (aggressor.side == 'S') {
            while (aggressor.quantity > 0 && !_buyOrders.empty()) {
                auto buyIt = _buyOrders.begin();

                if (buyIt->price < aggressor.price) {
                    break;
                }

                int tradeQuantity = std::min(aggressor.quantity, buyIt->quantity);

                Trade sellTrade = {aggressor.traderId, '-', tradeQuantity, buyIt->price};
                Trade buyTrade = {buyIt->traderId, '+', tradeQuantity, buyIt->price};

                trades.push_back(sellTrade);
                trades.push_back(buyTrade);

                aggressor.quantity -= tradeQuantity;

                Order updatedBuyOrder = *buyIt;
                updatedBuyOrder.quantity -= tradeQuantity;
                _buyOrders.erase(buyIt);

                if (updatedBuyOrder.quantity > 0) {
                    _buyOrders.insert(updatedBuyOrder);
                }
            }

        }
        if (aggressor.quantity > 0) {
            if (aggressor.side == 'B') {
                _buyOrders.insert(aggressor);
            } else {
                _sellOrders.insert(aggressor);
            }
        }
        return mergeTrades(std::move(trades));
    }

    struct TupleHash {
        std::size_t operator()(const std::tuple<std::string, char, int> &k) const {
            return std::hash<std::string>()(std::get<0>(k)) ^ std::hash<char>()(std::get<1>(k)) ^
                   std::hash<int>()(std::get<2>(k));
        }
    };

    std::vector<Trade> TradingEngine::mergeTrades(std::vector<Trade> &&trades) {
        std::unordered_map<std::tuple<std::string, char, int>, int, TupleHash> tradeMap;

        for (const auto &trade: trades) {
            auto key = std::make_tuple(trade.tradeId, trade.sign, trade.price);
            tradeMap[key] += trade.quantity;
        }

        std::vector<Trade> consolidatedTrades;

        for (const auto &[key, quantity]: tradeMap) {
            const auto &[traderId, sign, price] = key;
            consolidatedTrades.emplace_back(traderId, sign, quantity, price);
        }

        std::sort(consolidatedTrades.begin(), consolidatedTrades.end(), [](Trade &lhs, Trade &rhs) {
            if (lhs.tradeId != rhs.tradeId) {
                return lhs.tradeId < rhs.tradeId;
            }
            if (lhs.sign != rhs.sign) {
                return lhs.sign < rhs.sign;
            }
            return lhs.price < rhs.price;
        });

        return consolidatedTrades;
    }

    const std::multiset<Order, SellOrderComparator> &TradingEngine::getSellOrders() const {
        return _sellOrders;
    }

    const std::multiset<Order, BuyOrderComparator> &TradingEngine::getBuyOrders() const {
        return _buyOrders;
    }


    std::string formatTrades(const std::vector<Trade> &trades) {
        std::stringstream result;
        for (size_t i = 0; i < trades.size(); ++i) {
            const Trade &trade = trades[i];
            result << trade.tradeId << trade.sign << trade.quantity << "@" << trade.price;
            if (i < trades.size() - 1) {
                result << " ";
            }
        }
        return result.str();
    }
}
