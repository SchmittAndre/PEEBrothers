#include "stdafx.h"
                   
#include "Command.h"
#include "CustomAdventureAction.h"
#include "Player.h"
#include "Item.h"

#include "Location.h"

Location::LocatedCommandAction::LocatedCommandAction(Command * command, CustomAdventureAction * action, bool anywhere)
:   commandAction(command, action)
{
    this->anywhere = anywhere;
}  

Location::~Location()
{
    for (auto entry : inventories)
        delete entry.second;
}

AliasList& Location::getAliases()
{
    return aliases;
}                          

std::string Location::getName(bool definiteArticle, bool startOfSentence) const
{
    return aliases.getName(definiteArticle, startOfSentence);
}
                           
std::string Location::getName(Player * player, bool startOfSentence) const
{
    return getName(player->knows((Location*)this), startOfSentence);
}

std::string Location::getDescription() const
{
    return description;
}

void Location::setDescription(std::string description)
{
    this->description = description;
}

void Location::addCommand(Command * command, CustomAdventureAction * action, bool anywhere)
{
    commands.push_back(LocatedCommandAction(command, action, anywhere));
}

Location::PInventory* Location::addInventory(std::string preposition)
{
    if (hasInventory(preposition))
        return NULL;
    return inventories[preposition] = new PInventory();
}

bool Location::delInventory(std::string preposition)
{
    auto entry = inventories.find(preposition);
    if (entry == inventories.end())
        return false;

    delete entry->second;
    inventories.erase(entry);
    return true;
}

bool Location::hasInventory(std::string preposition)
{
    return inventories.find(preposition) != inventories.end();
}

size_t Location::filledInventoryCount()
{
    size_t result = 0;
    for (auto inv : inventories)
        result += (int)!inv.second->isEmpty();
    return result;
}

Location::PInventory * Location::firstFilledInventory()
{
    for (auto inv : inventories)
        if (!inv.second->isEmpty())
            return inv.second;
    return NULL;
}

std::vector<Location::PInventory*> Location::getInventories()
{
    std::vector<PInventory*> invs;
    for (auto entry : inventories)
        invs.push_back(entry.second);
    return invs;
}

Location::PInventory * Location::getInventory(std::string preposition)
{
    auto entry = inventories.find(preposition);
    return entry != inventories.end() ? entry->second : NULL;
}

std::string Location::formatPrepositions(bool filledOnly)
{
    if (inventories.empty())
        return "none";
    std::string result = "";
    size_t i = 0;
    if (filledOnly)
    {
        for (auto inventory = inventories.begin(); i < filledInventoryCount(); inventory++, i++)
        {
            if (inventory->second->isEmpty())
                continue;
            if (i + 1 == filledInventoryCount() && result != "")
                result += " and ";
            result += (*inventory).second->getPrepositionName();
            if (i + 2 < filledInventoryCount())
                result += ", ";
        }
    }
    else
    {
        for (auto inventory = inventories.begin(); i < inventories.size(); inventory++, i++)
        {
            if (i + 1 == inventories.size() && result != "")
                result += " and ";
            result += (*inventory).second->getPrepositionName();
            if (i + 2 < inventories.size())
                result += ", ";
        }
    }
    return result;
}

Location::PInventory::PInventory()
{
    filter = NULL;
    mode = ifBlacklist;
}

Location::PInventory::~PInventory()
{
    delete filter;
}

bool Location::PInventory::addPrepositionAlias(std::string alias, bool take)
{
    if (hasPrepositionAlias(alias, true))
        return false;

    if (take)
        prepAliasesTake.push_back(alias);
    else
        prepAliasesList.push_back(alias);

    return true;
}

bool Location::PInventory::delPrepositionAlias(std::string alias)
{
    strings::iterator entry;
    entry = find(prepAliasesList.begin(), prepAliasesList.end(), alias);
    if (entry != prepAliasesList.end())
    {
        prepAliasesList.erase(entry);
        return true;
    }
    entry = find(prepAliasesTake.begin(), prepAliasesTake.end(), alias);
    if (entry != prepAliasesTake.end())
    {
        prepAliasesTake.erase(entry);
        return true;
    }                   
    return false;
}

std::string Location::PInventory::getPrepositionName(bool take, bool startOfSentence) const
{
    std::string name;
    if (take && prepAliasesTake.size() > 0)
        name = prepAliasesTake[0];
    else if (!take && prepAliasesList.size() > 0)
        name = prepAliasesList[0];
    else
        name = "[missing preposition]";
    if (startOfSentence)
        name[0] = toupper(name[0]);
    return name;
}

bool Location::PInventory::hasPrepositionAlias(std::string alias, bool take) const
{
    return find(prepAliasesList.begin(), prepAliasesList.end(), alias) != prepAliasesList.end() ||
           take && find(prepAliasesTake.begin(), prepAliasesTake.end(), alias) != prepAliasesTake.end();
}

bool Location::PInventory::addItem(Item * item)
{                                             
    // add if filter disabled or blacklist which doesn't have item or whitelist having item
    return !filter || (mode == PInventory::ifBlacklist) ^ filter->hasItem(item) ? Inventory::addItem(item) : false;
}

bool Location::PInventory::isFiltered() const
{
    return filter != NULL;
}

Location::PInventory::Filter Location::PInventory::getFilterMode() const
{
    return mode;
}

void Location::PInventory::enableFilter(Filter mode)
{
    this->mode = mode;
    filter = new Inventory();
}

void Location::PInventory::addToFilter(Item * item)
{
    filter->addItem(item);
}

bool Location::PInventory::delFromFilter(Item * item)
{
    return filter->delItem(item);
}
