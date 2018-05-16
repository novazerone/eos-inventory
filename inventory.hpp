/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */

#include <eosiolib/eosio.hpp>
#include <eosiolib/currency.hpp>
#include <eosiolib/contract.hpp>
#include "def.h"
#include <string>

namespace horus {

    class Inventory : public eosio::currency,
                      public eosio::contract {
    public: // Actions

        // Catalog
        void on(const horus::ACTION_CATALOG_ENLIST& e);
        void on(const horus::ACTION_CATALOG_VIEW& v);

        // Inventory
        void on(const horus::ACTION_INVENTORY_ADD& a);
        void on(const horus::ACTION_INVENTORY_DISCARD& d);
        void on(const horus::ACTION_INVENTORY_PURCHASE& p);

    public: // Class Methods

        Inventory(account_name self);

        void init();
        void apply(uint64_t code, uint64_t action);

        void inline_add(const horus::ACTION_INVENTORY_ADD& a);
        void inline_discard(const horus::ACTION_INVENTORY_DISCARD& d);

    private:

        typedef eosio::multi_index<N(catalog), catalog_item> catalog_table;
        typedef eosio::multi_index<N(inventory), inventory_item> inventory_table;
    };
}