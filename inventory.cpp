/**
 *  @file
 *  @copyright defined in eos/LICENSE.txt
 */
#include <inventory.hpp>
#include <vector>
#include <tuple>

using namespace horus;

#pragma region Class Methods

Inventory::Inventory(account_name self) 
: eosio::currency(self)
, eosio::contract(self) {

}

void Inventory::init() {
    
}

void Inventory::apply(uint64_t code, uint64_t action) {

    if (_self != code)
        return;

    if (eosio::currency::apply(code, action))
         return;

    switch(action) {

        case EOSIO_STRING_TO_NAME(ACTION_CATALOG_ENLIST):
            on(eosio::unpack_action_data<ACTION_CATALOG_ENLIST>());
            return;

        case EOSIO_STRING_TO_NAME(ACTION_CATALOG_VIEW):
            on(eosio::unpack_action_data<ACTION_CATALOG_VIEW>());
            return;


        case EOSIO_STRING_TO_NAME(ACTION_INVENTORY_ADD):
            on(eosio::unpack_action_data<ACTION_INVENTORY_ADD>());
            return;

        case EOSIO_STRING_TO_NAME(ACTION_INVENTORY_DISCARD):
            on(eosio::unpack_action_data<ACTION_INVENTORY_DISCARD>());
            return;

        case EOSIO_STRING_TO_NAME(ACTION_INVENTORY_PURCHASE):
            on(eosio::unpack_action_data<ACTION_INVENTORY_PURCHASE>());
            return;

        default:
            return;
    }
}

void Inventory::inline_add(const horus::ACTION_INVENTORY_ADD& a) {

    eosio::action act(eosio::permission_level(a.to, N(active)), 
                      _self, EOSIO_STRING_TO_NAME(ACTION::ACTION_INVENTORY_ADD), 
                      a);
    act.send();
}

void Inventory::inline_discard(const horus::ACTION_INVENTORY_DISCARD& d) {

    eosio::action act(eosio::permission_level(d.to, N(active)), 
                      _self, EOSIO_STRING_TO_NAME(ACTION::ACTION_INVENTORY_DISCARD), 
                      d);
    act.send();
}

#pragma endregion

#pragma region Actions

void Inventory::on(const horus::ACTION_CATALOG_ENLIST& e) {

    require_auth(_self);
    
    eosio_assert(e.price.symbol.precision() == 2, 
                "Invalid precision. 2 decimals should be used for simpler management.");

    catalog_table catalog(_self, _self);
    eosio_assert(!contains(catalog, e.id),
                "Item already enlisted! Consider modifying it instead.");

    eosio_assert(!e.full_name.empty(), "Full name is empty.");

    eosio_assert(e.price.is_valid(), "Invalid price.");
    eosio_assert(e.price.amount > 0, "Price must be positive.");
    
    catalog.emplace(_self, [&](auto& s) {

        s.id =  e.id;
        s.full_name = e.full_name;
        s.price = e.price;
        s.can_sell = e.can_sell;
    });
    
    eosio::print("Enlisted -> ", e.full_name.c_str(), "\n");
}

void Inventory::on(const horus::ACTION_CATALOG_VIEW& v) {

    catalog_table catalog(_self, _self);

    eosio_assert(contains(catalog, v.id), "Item does not exist.");

    const auto& item = catalog.get(v.id);

    eosio::print("Full Name: ", item.full_name.c_str(), "\n");
    eosio::print("Price: ",  item.price, "\n");
    eosio::print("Can sell: ",  item.can_sell, "\n");
}

void Inventory::on(const horus::ACTION_INVENTORY_ADD& a) {

    require_recipient(a.to);

    eosio_assert(a.quantity > 0, "Cannot add zero items");

    catalog_table catalog(_self, _self);
    eosio_assert(contains(catalog, a.id), "Item does not exist.");

    inventory_table inventory(_self, a.to);

    if (contains(inventory, a.id)) {

        const auto& item = inventory.get(a.id);
        inventory.modify(item, a.issuer, [&](auto& i) {

            i.quantity += a.quantity;
        });

    } else {

        inventory.emplace(a.issuer, [&](auto& i) {

            i.id = a.id;
            i.quantity += a.quantity;
        });
    }
}

void Inventory::on(const horus::ACTION_INVENTORY_DISCARD& d) {

    require_recipient(d.to);

    eosio_assert(d.quantity > 0, "Cannot discard zero items");
    
    inventory_table inventory(_self, d.to);
    eosio_assert(contains(inventory, d.id), "Item not in inventory.");

    const auto& item = inventory.get(d.id);
    eosio_assert(!item.infinite, "Cannot discard item that has infinite amount.");
    eosio_assert(item.quantity > 0, "Cannot discard because quantity held is already zero.");
    eosio_assert(item.quantity >= d.quantity, "Cannot discard more than the quantity held.");

    inventory.modify(item, d.issuer, [&](auto& i) {

        i.quantity -= d.quantity;
    });
}

void Inventory::on(const horus::ACTION_INVENTORY_PURCHASE& p) {

    require_auth(p.buyer);

    catalog_table catalog(_self, _self);
    eosio_assert(contains(catalog, p.id), "Item does not exist.");

    const auto& item = catalog.get(p.id);

    // Not inlined because we need its assertions on the inventory.
    on(ACTION_INVENTORY_DISCARD{ _self, p.seller, item.id, p.quantity });
    on(ACTION_INVENTORY_ADD{ _self, p.buyer, item.id, p.quantity });

    // Only do currency transfer when there are no problems transferring the items.
    inline_transfer(p.buyer, p.seller, item.price * p.quantity, "Bought item");

    eosio::print("Purchased -> ", item.full_name.c_str(), " for ", item.price * p.quantity, "\n");
}

#pragma endregion

extern "C" {

   [[noreturn]] void apply(uint64_t receiver, uint64_t code, uint64_t action) {

        horus::Inventory i(receiver);
        i.apply(code, action);
        eosio_exit(0);
   }
}
