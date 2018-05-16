# eos-inventory

A smart contract for simulating an inventory system running on a blockchain.


### Overview

The smart contract was created with the intention of using it as a backend in an online game with shopping mechanics. Such use cases are the following:

- In-app purchases in games and apps for digital goods
- In-game NPC shops in an role-playing game such as a Blacksmith selling weapons and armor, Cleric selling items, etc.
- Player inventory management - manages items that the player can carry, equip, and sell (if part a game's mechanics)
    
The goal of this smart contract is to be a cryptocurrency payment solution to an app. Take for an example that a business has an ecosystem of apps with digital goods to sell, and has created its own crytocurrency. The users will now be able to purchase items within the app using the crytocurreny they have. This smart contract will automatically handle the transactions plus the management of items on the blockchain. The following diagram illustrates this.

![Inventory Smart Contract Diagram](https://github.com/novazerone/eos-inventory/blob/master/isc_graph.png?raw=true)
*Created with MindMup*

### Usage

The contract is to be applied to an account created for the purpose of managing an inventory of apps. So, this is not really applied to an individual user with inventory capabilities, say, a player in a game. The smart contract manages the inventory of every account involved: buyers and sellers.

For example, multiple users do an in-app purchase of varying amounts of digital goods. 

- Their apps will then communicate with the smart contract.
- The smart contract will process the transactions and see if the items they want to purchase are available.
- The smart contract will keep track of each of the users' inventory. (i.e. 3 apples and 5 oranges for User1, 49 grapes for User2, etc.)

Furthermore, the smart contract is derived from the **currency** contract included in the eosio installation. As a result, you may use the actions defined in the parent contract (issue, transfer, etc.) from within.

#### Actions

###### cenlist

Registers an item in the catalog. This item will then be available for purchase.

Sample command:

    cleos push action _account_ cenlist '["fbow", "Flaming Bow", "1000.00 EOS", 1]' -p _account_

###### cview

Views an item from the catalog. Prints the details of the item such as its full name and price to the console.

Sample command:

    cleos push action _account_ cview '["fbow"]' -p _account_

###### iadd

Adds a certain amount of specified item to an account's inventory.

Sample command:

    cleos push action _account_ iadd '["issuer", "targetaccount", "fbow", 5]' -p _account_

###### idiscard

Discards a certain amount of specified item in an account's inventory.

Sample command:

    cleos push action _account_ idiscard '["issuer", "targetaccount", "fbow", 5]' -p _account_

###### ipurchase

Purchases an item from one account to another. This command will also call iadd and idiscard, as well as the inline_transfer action from the currency contract.

Sample command:

    cleos push action _account_ ipurchase '["buyingaccount", "sellingaccount", "fbow", 5]' -p _account_


#### Tables

###### Catalog

This table is used to identify an item in the inventory system. 

###### Inventory

This table is used to represent the inventory itself. From the catalog table, a digital item can be tracked by the account. Basically, this table contains data on the amount of items an account has.
