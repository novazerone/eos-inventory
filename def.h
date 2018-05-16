#pragma once

#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <string>

// Defines for easier implementation
// This is to prevent missing a name change across the code.
// NOTES: 
//   Make sure the names reflect on the .abi file! 
//   Decided not to use the ABI generator for now as some of its not yet finalized.
#define ACTION_CATALOG_ENLIST cenlist
#define ACTION_CATALOG_VIEW cview

#define ACTION_INVENTORY_ADD iadd
#define ACTION_INVENTORY_DISCARD idiscard
#define ACTION_INVENTORY_PURCHASE ipurchase

// Helper Macros
#define EOSIO_STRING_TO_NAME(s) N(s)


// Namespace to contain all definitions in one.
// This structured this way so that other smart contracts 
// can  easily include this header instead of re-defining everything
// or including specific files.
namespace horus {

    typedef uint64_t unique_name;

    #pragma region Helper Functions

    /**
     * Simple helper function for checking an element in the multi-index container.
     */
    template<typename T, typename K>
    static bool contains(const T& mi, const K& k) {

        auto existing = mi.find(k);
        return existing != mi.end();
    }

    #pragma endregion

    #pragma region Catalog Actions

    /**
     * Registers an item in the catalog.
     * This item will then be available for purchase.
     */
    struct ACTION_CATALOG_ENLIST {
    
        unique_name id;
        std::string full_name;
        eosio::asset price;
        uint8_t can_sell;
    };

    /**
     * Views an item from the catalog.
     * Prints the details of the item to the console.
     */
    struct ACTION_CATALOG_VIEW {

        unique_name id;
    };

    #pragma endregion

    #pragma region Inventory Actions

    /**
     * Adds a certain amount of specified item to an account's inventory.
     */ 
    struct ACTION_INVENTORY_ADD {

        account_name issuer;
        account_name to;
        unique_name id;
        uint64_t quantity;
    };

    /**
     * Discards a certain amount of specified item in an account's inventory.
     */
    struct ACTION_INVENTORY_DISCARD {

        account_name issuer;
        account_name to;
        unique_name id;
        uint64_t quantity;
    };

    /**
     * Purchases an item from one account to another
     */
    struct ACTION_INVENTORY_PURCHASE {

        account_name buyer;
        account_name seller;
        unique_name id;
        uint64_t quantity;
    };

    #pragma endregion
 
    #pragma region Tables
    
    /**
     * Used by the Catalog table.
     * Types of items that are used throughout this contract.
     */ 
    struct catalog_item {

        unique_name id;       // Primary Key
        std::string full_name;
        eosio::asset price;
        bool can_sell = true;

        uint64_t primary_key() const { return id; }
    };

    /**
     * Used by the Inventory table.
     * Represents an item (from the catalog) in your inventory. 
     */
    struct inventory_item {

        unique_name id;       // Primary key
        uint64_t quantity;
        bool infinite = false;

        uint64_t primary_key() const { return id; }
    };

    #pragma endregion
}
