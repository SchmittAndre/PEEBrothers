#include "stdafx.h"

#include "Inventory.h"
#include "Room.h"
#include "Player.h"
#include "ItemCombiner.h"
#include "DefaultAdventureAction.h"

bool DefaultAdventureAction::run(const Command::Result & params) const
{
    // The input does not match with any available command
    write("I didn't quiet get that...");
    return true;
}

bool HelpAction::run(const Command::Result & params) const
{
    // Give the player some helpful advice               
    write("I'm sorry, but I can't help you at the moment!");
    return true;
}

bool LookAroundAction::run(const Command::Result & params) const
{
    write("You can see " + currentRoom()->formatLocations(getPlayer()) + ".");
    return true;
}

bool ShowInventoryAction::run(const Command::Result & params) const
{
    // List the players inventory
    if (getPlayerInv()->isEmpty())
    {
        write("You are not carrying anything.");
    }
    else
    {
        write("You are carrying " + getPlayerInv()->formatContents() + ".");
    }
    return true;
}

bool InspectAction::run(const Command::Result & params) const
{
    // Inspect a location and show the description, also goes there
    if (getPlayer()->isAtLocation())
    {
        for (Location::PInventory* inv : currentLocation()->getInventories())
        {
            if (Item* item = inv->findItem(params["location"]))
            {
                write(item->getDescription());
                return true;
            }
        }
    }

    if (RoomConnection* connection = currentRoom()->findRoomConnectionTo(params["location"]))
    {
        getPlayer()->inform(connection);
        getPlayer()->gotoRoom(connection->getOtherRoom(currentRoom()));
        write("You went through the " + connection->getName(getPlayer()) + " and entered " + currentRoom()->getName(getPlayer()) + ".");
        write(currentRoom()->getDescription());
    }
    else if (currentRoom()->getAliases().has(params["location"]))
    {
        write(currentRoom()->getDescription());
    }
    else if (Location* location = currentRoom()->findLocation(params["location"]))
    {
        getPlayer()->gotoLocation(location);
        write(location->getDescription());
        if (location->filledInventoryCount() > 0)
        {
            std::string content;
            auto invs = location->getInventories();
            for (auto inv = invs.begin(); inv != invs.end(); inv++)
            {
                if ((*inv)->isEmpty())
                    continue;
                if (inv == invs.end() - 1 && content != "")
                    content += " and ";
                content += (*inv)->formatContents() + " " + (*inv)->getPrepositionName() + " " + location->getName(getPlayer());
                if (inv < invs.end() - 2)
                    content += ", ";
            }
            std::string be = location->filledInventoryCount() > 1 ||
                location->firstFilledInventory()->getItemCount() > 1 ||
                location->firstFilledInventory()->getItems()[0]->isNamePlural() ? "are " : "is ";
            write("There " + be + content + ".");
        }

        if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        {
            if (connection->isAccessible())
            {
                write(location->getName(getPlayer(), true) + " leads to " + connection->getOtherRoom(currentRoom())->getName(getPlayer()) + ".");
            }
        }
    }
    else if (Item* item = getPlayerInv()->findItem(params["location"]))
    {
        write(item->getDescription());
    }
    else
    {
        write("Where exactly is that, if I shall ask?");
    }
    return true;
}

bool PickupAction::run(const Command::Result & params) const
{
    // Pick up an item from a location, if it is accessible
    if (Location* location = currentRoom()->findLocation(params["item"]))
    {
        getPlayer()->inform(location);
        write("You can't pick up " + location->getName(getPlayer()) + ".");
    }
    else if (currentRoom()->getAliases().has(params["item"]))
    {
        write("Are you crazy? You can't pick up " + currentRoom()->getName(getPlayer()) + ".");
    }
    else if (Room* room = currentRoom()->findRoom(params["item"]))
    {
        getPlayer()->inform(room);
        write("Are you insane? You can't pick up " + room->getName(getPlayer()) + ".");
    }
    else if (getPlayer()->isAtLocation())
    {
        for (Location::PInventory* inv : currentLocation()->getInventories())
        {
            if (Item* item = inv->findItem(params["item"]))
            {
                inv->delItem(item);
                getPlayerInv()->addItem(item);
                write("You picked up " + item->getName(true) + " " + inv->getPrepositionName(true) + " " + location->getName() + ".");
                return true;
            }
        }
        write("Here is no " + Alias(params["item"]).nameOnly() + ".");
    }
    else
    {
        write("There is no " + Alias(params["item"]).nameOnly() + " here.");
    }
    return true;
}

bool PickupFromAction::run(const Command::Result & params) const
{
    if (Location* location = currentRoom()->findLocation(params["location"]))
    {
        getPlayer()->inform(location);
        getPlayer()->gotoLocation(location);
        Location::PInventory* lastMatch = NULL;
        for (Location::PInventory* inv : location->getInventories())
        {
            if (inv->hasPrepositionAlias(params["prep"], true))
            {
                lastMatch = inv;
                if (Item* item = inv->findItem(params["item"]))
                {
                    inv->delItem(item);
                    getPlayerInv()->addItem(item);
                    write("You picked up " + item->getName(true) + " " + inv->getPrepositionName(true) + " " + location->getName(getPlayer()) + ".");
                    return true;
                }
            }
        }             
        if (!lastMatch)
        {
            if (location->getInventories().size() > 0)
            {
                write("There is only something " + location->formatPrepositions() + " " + location->getName(getPlayer()) + ".");
            }
            else
            {
                write("There is nothing at " + location->getName(getPlayer()) + ".");
            }
        }
        else
            write("There is no " + Alias(params["item"]).nameOnly() + " " + lastMatch->getPrepositionName() + " " + location->getName(getPlayer()) + ".");
    }
    else
    {
        write("There is no " + Alias(params["location"]).nameOnly() + " here.");
    }
    return true;
}

bool UseRoomConnectionAction::run(const Command::Result & params) const
{
    // Use a room connection to get to another room if it is accessible
    if (Location* location = currentRoom()->findLocation(params["door"]))
    {
        if (RoomConnection* connection = dynamic_cast<RoomConnection*>(location))
        {
            if (connection->isAccessible())
            {
                getPlayer()->inform(connection);
                Room* room = connection->getOtherRoom(currentRoom());
                getPlayer()->gotoRoom(room);
                write("You went through " + connection->getName(getPlayer()) + " and entered " + currentRoom()->getName() + ".");
            }
            else
            {
                write(connection->getDescription());
            }
        }
        else
        {
            write("You can't go through " + location->getName(getPlayer()) + ".");
        }
    }
    else if (Room* room = currentRoom()->findRoom(params["door"]))
    {
        write("You can't go through " + room->getName(getPlayer()) + ".");
    }
    else
    {
        write("Where is that?");
    }
    return true;
}

bool GotoAction::run(const Command::Result & params) const
{
    // Go to a specific location in the current room
    if (Location* location = currentRoom()->findLocation(params["location"]))
    {
        if (currentLocation() == location)
        {
            write("You are at " + location->getName(getPlayer()) + " already.");
        }
        else
        {
            getPlayer()->gotoLocation(location);
            write("You went to " + location->getName(getPlayer()) + ".");
        }
    }
    else if (currentRoom()->getAliases().has(params["location"]))
    {
        write("You are in " + currentRoom()->getName(getPlayer()) + " already.");
    }
    else if (RoomConnection* connection = currentRoom()->findRoomConnectionTo(params["location"]))
    {
        getPlayer()->inform(connection);
        getPlayer()->gotoRoom(connection->getOtherRoom(currentRoom()));
        write("You went through " + connection->getName(getPlayer()) + " and entered " + currentRoom()->getName(getPlayer()) + ".");
    }
    else
    {
        write("Here is no " + Alias(params["location"]).nameOnly() + ".");
    }
    return true;
}

bool EnterRoomAction::run(const Command::Result & params) const
{
    if (currentRoom()->getAliases().has(params["room"]))
    {
        write("You are in " + currentRoom()->getName(getPlayer()) + " already.");
    }
    else if (RoomConnection* connection = currentRoom()->findRoomConnectionTo(params["room"]))
    {
        if (connection->isAccessible())
        {
            getPlayer()->inform(connection);
            getPlayer()->gotoRoom(connection->getOtherRoom(currentRoom()));
            write("You went through " + connection->getName(getPlayer()) + " and entered " + currentRoom()->getName() + ".");
        }
        else
        {
            write(connection->getDescription());
        }
    }   
    else
    {
        write("Here is no " + Alias(params["room"]).nameOnly() + ".");
    }
    return true;
}

bool CombineItemsAction::run(const Command::Result & params) const
{
    // Combine two items from the players inventory if possible
    Item* item1 = getPlayerInv()->findItem(params["item1"]);
    Item* item2 = getPlayerInv()->findItem(params["item2"]);
    if (item1 && item2)
    {
        if (item1 == item2)
        {
            write("You can't combine " + item1->getName(true) + " with itself!");
        }
        else if (Item* result = getItemCombiner()->getResult(item1, item2))
        {
            getPlayerInv()->delItem(item1);
            getPlayerInv()->delItem(item2);
            getPlayerInv()->addItem(result);
            write("You combined the two and received " + result->getName() + ".");
        }
        else
        {
            write("You can't combine " + item1->getName(true) + " with " + item2->getName(true) + ".");
        }
    }        
    else if (item2)
    {
        write("You have no " + Alias(params["item1"]).nameOnly() + ".");
    }
    else if (item1)
    {
        write("You have no " + Alias(params["item2"]).nameOnly() + ".");
    }
    else
    {
        write("You have neither " + Alias(params["item1"]).nameOnly() + " nor " + Alias(params["item2"]).nameOnly() + "!");
    }
    return true;
}
