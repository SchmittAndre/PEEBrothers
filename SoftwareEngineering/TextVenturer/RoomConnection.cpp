#include "stdafx.h"

#include "Room.h"
#include "CommandSystem.h"
#include "CustomAdventureAction.h"

#include "RoomConnection.h"

RoomConnection::RoomConnection(Room* room1, Room* room2, bool accessible)
{
    this->room1 = room1;
    this->room2 = room2;
    this->accessible = accessible;
    this->onUse = NULL;
}

RoomConnection::~RoomConnection()
{
    delete onUse;
}

Room * RoomConnection::getOtherRoom(const Room* room) const
{
    if (room1 == room)
        return room2;
    if (room2 == room)
        return room1;
    return NULL;
}

bool RoomConnection::isAccessible() const
{
    return accessible;
}

void RoomConnection::lock()
{
    accessible = false;
}

void RoomConnection::unlock()
{
    accessible = true;
}

void RoomConnection::runOnUse()
{
    if (onUse)
        onUse->run();
}

void RoomConnection::setOnUse(CustomAdventureAction * onUse)
{
    this->onUse = onUse;
}
