/*
    Copyright (c) 2007-2008 FastMQ Inc.

    This file is part of 0MQ.

    0MQ is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    0MQ is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __EXCHANGE_MATCHING_ENGINE_HPP_INCLUDED__
#define __EXCHANGE_MATCHING_ENGINE_HPP_INCLUDED__

#include <assert.h>
#include <stdio.h>
#include <vector>
#include <deque>

#include "common.hpp"

namespace exchange
{

    //  Matching engine for a particular product.
    //
    //  Given the assumption that the order prices are densly packed around
    //  the current market price, the complexity of matching is O(1) with
    //  respect to the number of orders in the orderbook.

    class matching_engine_t
    {
    private:

        //  Lower limit is the maximal out-of-range price
        //  Upper limit is the minimal out-of-range price
        //  If we later choose to adjust the price range dynamically, these should
        //  be standard member variables rather than constants.
        enum
        {
            lower_limit = 0,
            upper_limit = 1000
        };

    public:

        inline matching_engine_t () :
            min_ask (upper_limit),
            max_bid (lower_limit),
            orderbook (upper_limit - lower_limit - 1)
        {
        }

        template <typename T> bool ask (T *callback, order_id_t order_id,
            price_t price, volume_t volume)
        {
            assert (price > lower_limit && price < upper_limit);
            assert (volume > 0);

            //  If at least one trade was generated, this variable will be true
            bool trades_sent = false;

            while (true) {

                //  If there's no matching bid, store the order and return
                if (price > max_bid) {
                    entry_t entry = {volume, order_id};
                    orderbook [price - lower_limit - 1].push_back (entry);
                    min_ask = std::min (min_ask, price);
                    return trades_sent;
                }

                //  Get the bids with maximal prices
                entries_t &entries = orderbook [max_bid - lower_limit - 1];

                //  Amount traded at this price level
                volume_t traded = 0;

                while (!entries.empty ()) {

                    //  Get the oldest available bid
                    entry_t &entry = entries.front ();

                    //  Determine trade volume
                    volume_t trade_volume = std::min (volume, entry.volume);

                    //  Execute the trade on bid
                    callback->traded (entry.order_id, max_bid, trade_volume);
                    traded += trade_volume;
		    trades_sent = true;

                    //  Adjust the bid: If it's fully executed, delete it
                    entry.volume -= trade_volume;
                    if (entry.volume == 0)
                        entries.pop_front ();

                    //  Adjust the ask: If it's fully executed, exit
                    volume -= trade_volume;
                    if (volume == 0) 
                        break;
                }

                //  Execute the trade on ask
                if (traded) {
                    callback->traded (order_id, max_bid, traded);
                    trades_sent = true;
                }

                //  If order is fully executed, exit
                if (volume == 0)
                    return trades_sent;

                //  We have executed all the bids with best price at this point
                //  Move to next best bid price level
                max_bid --;
            }
        }

        template <typename T> bool bid (T *callback, order_id_t order_id,
            price_t price, volume_t volume)
        {
            assert (price > lower_limit && price < upper_limit);
            assert (volume > 0);

            //  If at least one trade was executed, this variable will be true
            bool trades_sent = false;

            while (true) {

                //  If there's no matching ask, store the order and return
                if (price < min_ask) {
                    entry_t entry = {volume, order_id};
                    orderbook [price - lower_limit - 1].push_back (entry);
                    max_bid = std::max (max_bid, price);
                    return trades_sent;
                }

                //  Get the asks with minimal prices
                entries_t &entries = orderbook [min_ask - lower_limit - 1];

                //  Amount traded at this price level
                volume_t traded = 0;

                while (!entries.empty ()) {

                    //  Get the oldest available ask
                    entry_t &entry = entries.front ();

                    //  Determine trade volume
                    volume_t trade_volume = std::min (volume, entry.volume);

                    //  Execute the trade on ask
                    callback->traded (entry.order_id, min_ask, trade_volume);
                    traded += trade_volume;
                    trades_sent = true;

                    //  Adjust the ask: If it's fully executed, delete it
                    entry.volume -= trade_volume;
                    if (entry.volume == 0)
                        entries.pop_front ();

                    //  Adjust the bid: If it's fully executed, exit
                    volume -= trade_volume;
                    if (volume == 0)
                        break;
                }

                //  Execute the trade on bid
                if (traded) {
                    callback->traded (order_id, min_ask, traded);
                    trades_sent = true;
                }

                //  If order is fully executed, exit
                if (volume == 0)
                    return trades_sent;

                //  We have executed all the asks with best price at this point
                //  Move to next best ask price level
                min_ask ++;
            }
        }
    private:

        //  Represents the state of particular order in the order book.
        //  Volume is the remaining volume to trade rather than
        //  full order volume.
        struct entry_t
        {
            volume_t volume;
            order_id_t order_id;
        };

        //  Set of all orders with the same price
        typedef std::deque <entry_t> entries_t;

        //  Set of all orders
        typedef std::vector <entries_t> orderbook_t;

        //  Minimal hypotetically available ask price & maximal hypotetically
        //  available bid price
        //  If min_ask == upper_limit, there are no asks in the orderbook
        //  If max_bid == lower_limit, there are no bids in the orderbook
        price_t min_ask;
        price_t max_bid;

        //  The first element in the orderbook corresponds to the minimal possible
        //  price, second one to the second minimal price etc.
        orderbook_t orderbook;
    };

}

#endif
