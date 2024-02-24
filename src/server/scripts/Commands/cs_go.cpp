/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Affero General Public License as published by the
 * Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/* ScriptData
Name: go_commandscript
%Complete: 100
Comment: All go related commands
Category: commandscripts
EndScriptData */

#include "Chat.h"
#include "CommandScript.h"
#include "GameGraveyard.h"
#include "Language.h"
#include "MapMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "TicketMgr.h"

#include "boost/algorithm/string.hpp"
#include <regex>

using namespace Acore::ChatCommands;

class go_commandscript : public CommandScript
{
public:
    go_commandscript() : CommandScript("go_commandscript") { }

    ChatCommandTable GetCommands() const override
    {
        static ChatCommandTable goCommandTable =
        {
            { "creature",      HandleGoCreatureSpawnIdCommand,   SEC_GAMEMASTER1,  Console::No },
            { "creature id",   HandleGoCreatureCIdCommand,       SEC_GAMEMASTER1,  Console::No },
            { "creature name", HandleGoCreatureNameCommand,      SEC_GAMEMASTER1,  Console::No },
            { "gameobject",    HandleGoGameObjectSpawnIdCommand, SEC_GAMEMASTER1,  Console::No },
            { "gameobject id", HandleGoGameObjectGOIdCommand,    SEC_GAMEMASTER1,  Console::No },
            { "graveyard",     HandleGoGraveyardCommand,         SEC_GAMEMASTER1,  Console::No },
            { "grid",          HandleGoGridCommand,              SEC_GAMEMASTER1,  Console::No },
            { "taxinode",      HandleGoTaxinodeCommand,          SEC_GAMEMASTER1,  Console::No },
            { "trigger",       HandleGoTriggerCommand,           SEC_GAMEMASTER1,  Console::No },
            { "zonexy",        HandleGoZoneXYCommand,            SEC_GAMEMASTER1,  Console::No },
            { "xyz",           HandleGoXYZCommand,               SEC_GAMEMASTER1,  Console::No },
            { "ticket",        HandleGoTicketCommand,            SEC_GAMEMASTER1,  Console::No },
            { "quest",         HandleGoQuestCommand,             SEC_GAMEMASTER1,  Console::No },
        };
        static ChatCommandTable barberdruidCommandTable =
        {
            { "bear",			BarberInfo_bear,            	 SEC_PLAYER,  Console::No },
            { "cat",			BarberInfo_cat,            		 SEC_PLAYER,  Console::No },
            { "buho",			BarberInfo_buho,            	 SEC_PLAYER,  Console::No },
            { "travel",			BarberInfo_travel,            	 SEC_PLAYER,  Console::No },
            { "fly",			BarberInfo_fly,            	 	 SEC_PLAYER,  Console::No },
            { "sea",			BarberInfo_sea,            	 	 SEC_PLAYER,  Console::No },
            { "tree",			BarberInfo_tree,            	 SEC_PLAYER,  Console::No },
            { "reload",         BarberInfo_Reload,             	 SEC_PLAYER,  Console::No },
        };
        static ChatCommandTable commandTable =
        {
            { "go", goCommandTable },
            { "barberdruid", barberdruidCommandTable }
        };
        return commandTable;
    }

    static bool DoTeleport(ChatHandler* handler, Position pos, uint32 mapId = MAPID_INVALID)
    {
        Player* player = handler->GetSession()->GetPlayer();

        if (mapId == MAPID_INVALID)
            mapId = player->GetMapId();
        if (!MapMgr::IsValidMapCoord(mapId, pos) || sObjectMgr->IsTransportMap(mapId))
        {
            handler->SendErrorMessage(LANG_INVALID_TARGET_COORD, pos.GetPositionX(), pos.GetPositionY(), mapId);
            return false;
        }

        // stop flight if need
        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo({ mapId, pos });
        return true;
    }

    static bool HandleGoCreatureCIdCommand(ChatHandler* handler, Variant<Hyperlink<creature_entry>, uint32> cId)
    {
        CreatureData const* spawnpoint = GetCreatureData(handler, *cId);

        if (!spawnpoint)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOCREATNOTFOUND);
            return false;
        }

        return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
    }

    static bool HandleGoCreatureSpawnIdCommand(ChatHandler* handler, Variant<Hyperlink<creature>, ObjectGuid::LowType> spawnId)
    {
        CreatureData const* spawnpoint = sObjectMgr->GetCreatureData(spawnId);
        if (!spawnpoint)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOCREATNOTFOUND);
            return false;
        }

        return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
    }

    static bool HandleGoCreatureNameCommand(ChatHandler* handler, Tail name)
    {
        if (!name.data())
            return false;

        QueryResult result = WorldDatabase.Query("SELECT entry FROM creature_template WHERE name = \"{}\" LIMIT 1" , name.data());
        if (!result)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOCREATNOTFOUND);
            return false;
        }

        uint32 entry = result->Fetch()[0].Get<uint32>();
        CreatureData const* spawnpoint = GetCreatureData(handler, entry);
        if (!spawnpoint)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOCREATNOTFOUND);
            return false;
        }

        return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
    }

    static bool HandleGoGameObjectSpawnIdCommand(ChatHandler* handler, uint32 spawnId)
    {
        GameObjectData const* spawnpoint = sObjectMgr->GetGameObjectData(spawnId);
        if (!spawnpoint)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOOBJNOTFOUND);
            return false;
        }

        return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
    }

    static bool HandleGoGameObjectGOIdCommand(ChatHandler* handler, uint32 goId)
    {
        GameObjectData const* spawnpoint = GetGameObjectData(handler, goId);

        if (!spawnpoint)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOOBJNOTFOUND);
            return false;
        }

        return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
    }

    static bool HandleGoGraveyardCommand(ChatHandler* handler, uint32 gyId)
    {
        GraveyardStruct const* gy = sGraveyard->GetGraveyard(gyId);
        if (!gy)
        {
            handler->SendErrorMessage(LANG_COMMAND_GRAVEYARDNOEXIST, gyId);
            return false;
        }

        if (!MapMgr::IsValidMapCoord(gy->Map, gy->x, gy->y, gy->z))
        {
            handler->SendErrorMessage(LANG_INVALID_TARGET_COORD, gy->x, gy->y, gy->Map);
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();
        // stop flight if need
        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        player->TeleportTo(gy->Map, gy->x, gy->y, gy->z, player->GetOrientation());
        return true;
    }

    //teleport to grid
    static bool HandleGoGridCommand(ChatHandler* handler, float gridX, float gridY, Optional<uint32> oMapId)
    {
        Player* player = handler->GetSession()->GetPlayer();
        uint32 mapId = oMapId.value_or(player->GetMapId());

        // center of grid
        float x = (gridX - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;
        float y = (gridY - CENTER_GRID_ID + 0.5f) * SIZE_OF_GRIDS;

        if (!MapMgr::IsValidMapCoord(mapId, x, y))
        {
            handler->SendErrorMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            return false;
        }

        // stop flight if need
        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        Map const* map = sMapMgr->CreateBaseMap(mapId);
        float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));

        player->TeleportTo(mapId, x, y, z, player->GetOrientation());
        return true;
    }

    static bool HandleGoTaxinodeCommand(ChatHandler* handler, Variant<Hyperlink<taxinode>, uint32> nodeId)
    {
        TaxiNodesEntry const* node = sTaxiNodesStore.LookupEntry(nodeId);
        if (!node)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOTAXINODENOTFOUND, uint32(nodeId));
            return false;
        }
        return DoTeleport(handler, { node->x, node->y, node->z }, node->map_id);
    }

    static bool HandleGoTriggerCommand(ChatHandler* handler, Variant<Hyperlink<areatrigger>, uint32> areaTriggerId)
    {
        AreaTrigger const* at = sObjectMgr->GetAreaTrigger(areaTriggerId);
        if (!at)
        {
            handler->SendErrorMessage(LANG_COMMAND_GOAREATRNOTFOUND, uint32(areaTriggerId));
            return false;
        }
        return DoTeleport(handler, { at->x, at->y, at->z }, at->map);
    }

    //teleport at coordinates
    static bool HandleGoZoneXYCommand(ChatHandler* handler, float x, float y, Optional<Variant<Hyperlink<area>, uint32>> areaIdArg)
    {
        Player* player = handler->GetSession()->GetPlayer();

        uint32 areaId = areaIdArg ? *areaIdArg : player->GetZoneId();

        AreaTableEntry const* areaEntry = sAreaTableStore.LookupEntry(areaId);

        if (x < 0 || x > 100 || y < 0 || y > 100 || !areaEntry)
        {
            handler->SendErrorMessage(LANG_INVALID_ZONE_COORD, x, y, areaId);
            return false;
        }

        // update to parent zone if exist (client map show only zones without parents)
        AreaTableEntry const* zoneEntry = areaEntry->zone ? sAreaTableStore.LookupEntry(areaEntry->zone) : areaEntry;
                ASSERT(zoneEntry);

        Map const* map = sMapMgr->CreateBaseMap(zoneEntry->mapid);

        if (map->Instanceable())
        {
            handler->SendErrorMessage(LANG_INVALID_ZONE_MAP, areaEntry->ID, areaEntry->area_name[handler->GetSessionDbcLocale()], map->GetId(), map->GetMapName());
            return false;
        }

        Zone2MapCoordinates(x, y, zoneEntry->ID);

        if (!MapMgr::IsValidMapCoord(zoneEntry->mapid, x, y))
        {
            handler->SendErrorMessage(LANG_INVALID_TARGET_COORD, x, y, zoneEntry->mapid);
            return false;
        }

        // stop flight if need
        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        float z = std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));

        player->TeleportTo(zoneEntry->mapid, x, y, z, player->GetOrientation());
        return true;
    }

    /**
     * @brief Teleports the GM to the specified world coordinates, optionally specifying map ID and orientation.
     *
     * @param handler The ChatHandler that is handling the command.
     * @param args The coordinates to teleport to in format "x y z [mapId [orientation]]".
     * @return true The command was successful.
     * @return false The command was unsuccessful (show error or syntax)
     */
    static bool HandleGoXYZCommand(ChatHandler* handler, Tail args)
    {
        std::wstring wInputCoords;
        if (!Utf8toWStr(args, wInputCoords))
        {
            return false;
        }

        // extract float and integer values from the input
        std::vector<float> locationValues;
        std::wregex floatRegex(L"(-?\\d+(?:\\.\\d+)?)");
        std::wsregex_iterator floatRegexIterator(wInputCoords.begin(), wInputCoords.end(), floatRegex);
        std::wsregex_iterator end;
        while (floatRegexIterator != end)
        {
            std::wsmatch match = *floatRegexIterator;
            std::wstring matchStr = match.str();

            // try to convert the match to a float
            try
            {
                locationValues.push_back(std::stof(matchStr));
            }
            // if the match is not a float, do not add it to the vector
            catch (std::invalid_argument const&){}

            ++floatRegexIterator;
        }

        // X and Y are required
        if (locationValues.size() < 2)
        {
            return false;
        }

        Player* player = handler->GetSession()->GetPlayer();

        uint32 mapId = locationValues.size() >= 4 ? uint32(locationValues[3]) : player->GetMapId();

        float x = locationValues[0];
        float y = locationValues[1];

        if (!sMapStore.LookupEntry(mapId))
        {
            handler->SendErrorMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            return false;
        }

        Map const* map = sMapMgr->CreateBaseMap(mapId);

        float z = locationValues.size() >= 3 ? locationValues[2] : std::max(map->GetHeight(x, y, MAX_HEIGHT), map->GetWaterLevel(x, y));
        // map ID (locationValues[3]) already handled above
        float o = locationValues.size() >= 5 ? locationValues[4] : player->GetOrientation();

        if (!MapMgr::IsValidMapCoord(mapId, x, y, z, o))
        {
            handler->SendErrorMessage(LANG_INVALID_TARGET_COORD, x, y, mapId);
            return false;
        }

        return DoTeleport(handler, { x, y, z, o }, mapId);
    }

    static bool HandleGoTicketCommand(ChatHandler* handler, uint32 ticketId)
    {
        GmTicket* ticket = sTicketMgr->GetTicket(ticketId);
        if (!ticket)
        {
            handler->SendSysMessage(LANG_COMMAND_TICKETNOTEXIST);
            return true;
        }

        Player* player = handler->GetSession()->GetPlayer();

        // stop flight if need
        if (player->IsInFlight())
        {
            player->GetMotionMaster()->MovementExpired();
            player->CleanupAfterTaxiFlight();
        }
        // save only in non-flight case
        else
            player->SaveRecallPosition();

        ticket->TeleportTo(player);
        return true;
    }

    static bool BarberInfo_tree(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        uint32 nIdBuy = atoi((char*)args);

        uint32 costo = 1;
        uint32 itsok = 1;
        uint32 nLearnSpellID = 0;

        uint32 nReqItemID = 0;
        uint32 nReqItemCant = 0;

        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE type = 'tree'");

        Player* player = handler->GetSession()->GetPlayer();

        if (!nIdBuy)
            return false;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 display = fields[2].Get<uint32>();
                uint32 SpellId = fields[5].Get<uint32>();
                uint32 ReqSpellID = fields[6].Get<uint32>();
                uint32 ReqItemID = fields[8].Get<uint32>();
                uint32 ReqItemCant = fields[9].Get<uint32>();
                player->removeSpell(SpellId, SPEC_MASK_ALL, false);
                if (ReqSpellID == nIdBuy)
                {
                    nReqItemID = ReqItemID;
                    nReqItemCant = ReqItemCant;

                    if (player->HasSpell(ReqSpellID))
                    {
                        nLearnSpellID = SpellId;
                    }
                }
            } while (result->NextRow());
        }

        if (nLearnSpellID == 0)
        {
            itsok = 0;
        }
        if (itsok == 0)
        {
            if (nIdBuy == 1)
            {
                handler->PSendSysMessage("Reverted changes");
            }
            else
            {
                if (nReqItemID > 0)
                {
                    if (player->HasItemCount(nReqItemID, nReqItemCant))
                    {
                        player->DestroyItemCount(nReqItemID, nReqItemCant, true, false);
                        handler->PSendSysMessage("Unlocked appearance");
                        player->learnSpell(nIdBuy, false);
                    }
                    else
                    {
                        handler->PSendSysMessage("You don't have enough money to buy this");
                    }
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Change made");
            player->learnSpell(nLearnSpellID, false);
        }

        return true;
    }

    static bool BarberInfo_sea(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        uint32 nIdBuy = atoi((char*)args);

        uint32 costo = 1;
        uint32 itsok = 1;
        uint32 nLearnSpellID = 0;

        uint32 nReqItemID = 0;
        uint32 nReqItemCant = 0;

        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE type = 'sea'");

        Player* player = handler->GetSession()->GetPlayer();

        if (!nIdBuy)
            return false;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 display = fields[2].Get<uint32>();
                uint32 SpellId = fields[5].Get<uint32>();
                uint32 ReqSpellID = fields[6].Get<uint32>();
                uint32 ReqItemID = fields[8].Get<uint32>();
                uint32 ReqItemCant = fields[9].Get<uint32>();
                player->removeSpell(SpellId, SPEC_MASK_ALL, false);
                if (ReqSpellID == nIdBuy)
                {
                    nReqItemID = ReqItemID;
                    nReqItemCant = ReqItemCant;

                    if (player->HasSpell(ReqSpellID))
                    {
                        nLearnSpellID = SpellId;
                    }
                }
            } while (result->NextRow());
        }

        if (nLearnSpellID == 0)
        {
            itsok = 0;
        }
        if (itsok == 0)
        {
            if (nIdBuy == 1)
            {
                handler->PSendSysMessage("Reverted changes");
            }
            else
            {
                if (nReqItemID > 0)
                {
                    if (player->HasItemCount(nReqItemID, nReqItemCant))
                    {
                        player->DestroyItemCount(nReqItemID, nReqItemCant, true, false);
                        handler->PSendSysMessage("Unlocked appearance");
                        player->learnSpell(nIdBuy, false);
                    }
                    else
                    {
                        handler->PSendSysMessage("You don't have enough money to buy this");
                    }
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Change made");
            player->learnSpell(nLearnSpellID, false);
        }

        return true;
    }

    static bool BarberInfo_fly(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        uint32 nIdBuy = atoi((char*)args);

        uint32 costo = 1;
        uint32 itsok = 1;
        uint32 nLearnSpellID = 0;

        uint32 nReqItemID = 0;
        uint32 nReqItemCant = 0;

        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE type = 'fly'");

        Player* player = handler->GetSession()->GetPlayer();

        if (!nIdBuy)
            return false;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 display = fields[2].Get<uint32>();
                uint32 SpellId = fields[5].Get<uint32>();
                uint32 ReqSpellID = fields[6].Get<uint32>();
                uint32 ReqItemID = fields[8].Get<uint32>();
                uint32 ReqItemCant = fields[9].Get<uint32>();
                player->removeSpell(SpellId, SPEC_MASK_ALL, false);
                if (ReqSpellID == nIdBuy)
                {
                    nReqItemID = ReqItemID;
                    nReqItemCant = ReqItemCant;

                    if (player->HasSpell(ReqSpellID))
                    {
                        nLearnSpellID = SpellId;
                    }
                }
            } while (result->NextRow());
        }

        if (nLearnSpellID == 0)
        {
            itsok = 0;
        }
        if (itsok == 0)
        {
            if (nIdBuy == 1)
            {
                handler->PSendSysMessage("Reverted changes");
            }
            else
            {
                if (nReqItemID > 0)
                {
                    if (player->HasItemCount(nReqItemID, nReqItemCant))
                    {
                        player->DestroyItemCount(nReqItemID, nReqItemCant, true, false);
                        handler->PSendSysMessage("Unlocked appearance");
                        player->learnSpell(nIdBuy, false);
                    }
                    else
                    {
                        handler->PSendSysMessage("You don't have enough money to buy this");
                    }
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Change made");
            player->learnSpell(nLearnSpellID, false);
        }

        return true;
    }

    static bool BarberInfo_travel(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        uint32 nIdBuy = atoi((char*)args);

        uint32 costo = 1;
        uint32 itsok = 1;
        uint32 nLearnSpellID = 0;

        uint32 nReqItemID = 0;
        uint32 nReqItemCant = 0;

        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE type = 'travel'");

        Player* player = handler->GetSession()->GetPlayer();

        if (!nIdBuy)
            return false;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 display = fields[2].Get<uint32>();
                uint32 SpellId = fields[5].Get<uint32>();
                uint32 ReqSpellID = fields[6].Get<uint32>();
                uint32 ReqItemID = fields[8].Get<uint32>();
                uint32 ReqItemCant = fields[9].Get<uint32>();
                player->removeSpell(SpellId, SPEC_MASK_ALL, false);
                if (ReqSpellID == nIdBuy)
                {
                    nReqItemID = ReqItemID;
                    nReqItemCant = ReqItemCant;

                    if (player->HasSpell(ReqSpellID))
                    {
                        nLearnSpellID = SpellId;
                    }
                }
            } while (result->NextRow());
        }

        if (nLearnSpellID == 0)
        {
            itsok = 0;
        }
        if (itsok == 0)
        {
            if (nIdBuy == 1)
            {
                handler->PSendSysMessage("Reverted changes");
            }
            else
            {
                if (nReqItemID > 0)
                {
                    if (player->HasItemCount(nReqItemID, nReqItemCant))
                    {
                        player->DestroyItemCount(nReqItemID, nReqItemCant, true, false);
                        handler->PSendSysMessage("Unlocked appearance");
                        player->learnSpell(nIdBuy, false);
                    }
                    else
                    {
                        handler->PSendSysMessage("You don't have enough money to buy this");
                    }
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Change made");
            player->learnSpell(nLearnSpellID, false);
        }

        return true;
    }

    static bool BarberInfo_buho(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        uint32 nIdBuy = atoi((char*)args);

        uint32 costo = 1;
        uint32 itsok = 1;
        uint32 nLearnSpellID = 0;

        uint32 nReqItemID = 0;
        uint32 nReqItemCant = 0;

        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE type = 'buho'");

        Player* player = handler->GetSession()->GetPlayer();

        if (!nIdBuy)
            return false;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 display = fields[2].Get<uint32>();
                uint32 SpellId = fields[5].Get<uint32>();
                uint32 ReqSpellID = fields[6].Get<uint32>();
                uint32 ReqItemID = fields[8].Get<uint32>();
                uint32 ReqItemCant = fields[9].Get<uint32>();
                player->removeSpell(SpellId, SPEC_MASK_ALL, false);
                if (ReqSpellID == nIdBuy)
                {
                    nReqItemID = ReqItemID;
                    nReqItemCant = ReqItemCant;

                    if (player->HasSpell(ReqSpellID))
                    {
                        nLearnSpellID = SpellId;
                    }
                }
            } while (result->NextRow());
        }

        if (nLearnSpellID == 0)
        {
            itsok = 0;
        }
        if (itsok == 0)
        {
            if (nIdBuy == 1)
            {
                handler->PSendSysMessage("Reverted changes");
            }
            else
            {
                if (nReqItemID > 0)
                {
                    if (player->HasItemCount(nReqItemID, nReqItemCant))
                    {
                        player->DestroyItemCount(nReqItemID, nReqItemCant, true, false);
                        handler->PSendSysMessage("Unlocked appearance");
                        player->learnSpell(nIdBuy, false);
                    }
                    else
                    {
                        handler->PSendSysMessage("You don't have enough money to buy this");
                    }
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Change made");
            player->learnSpell(nLearnSpellID, false);
        }

        return true;
    }

    static bool BarberInfo_bear(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        uint32 nIdBuy = atoi((char*)args);

        uint32 costo = 1;
        uint32 itsok = 1;
        uint32 nLearnSpellID = 0;

        uint32 nReqItemID = 0;
        uint32 nReqItemCant = 0;

        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE type = 'bear'");

        Player* player = handler->GetSession()->GetPlayer();

        if (!nIdBuy)
            return false;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 display = fields[2].Get<uint32>();
                uint32 SpellId = fields[5].Get<uint32>();
                uint32 ReqSpellID = fields[6].Get<uint32>();
                uint32 ReqItemID = fields[8].Get<uint32>();
                uint32 ReqItemCant = fields[9].Get<uint32>();
                player->removeSpell(SpellId, SPEC_MASK_ALL, false);
                if (ReqSpellID == nIdBuy)
                {
                    nReqItemID = ReqItemID;
                    nReqItemCant = ReqItemCant;

                    if (player->HasSpell(ReqSpellID))
                    {
                        nLearnSpellID = SpellId;
                    }
                }
            } while (result->NextRow());
        }

        if (nLearnSpellID == 0)
        {
            itsok = 0;
        }
        if (itsok == 0)
        {
            if (nIdBuy == 1)
            {
                handler->PSendSysMessage("Reverted changes");
            }
            else
            {
                if (nReqItemID > 0)
                {
                    if (player->HasItemCount(nReqItemID, nReqItemCant))
                    {
                        player->DestroyItemCount(nReqItemID, nReqItemCant, true, false);
                        handler->PSendSysMessage("Unlocked appearance");
                        player->learnSpell(nIdBuy, false);
                    }
                    else
                    {
                        handler->PSendSysMessage("You don't have enough money to buy this");
                    }
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Change made");
            player->learnSpell(nLearnSpellID, false);
        }

        return true;
    }

    static bool BarberInfo_cat(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;
        uint32 nIdBuy = atoi((char*)args);

        uint32 costo = 1;
        uint32 itsok = 1;
        uint32 nLearnSpellID = 0;

        uint32 nReqItemID = 0;
        uint32 nReqItemCant = 0;

        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE type = 'cat'");

        Player* player = handler->GetSession()->GetPlayer();

        if (!nIdBuy)
            return false;

        if (result)
        {
            do
            {
                Field* fields = result->Fetch();
                uint32 display = fields[2].Get<uint32>();
                uint32 SpellId = fields[5].Get<uint32>();
                uint32 ReqSpellID = fields[6].Get<uint32>();
                uint32 ReqItemID = fields[8].Get<uint32>();
                uint32 ReqItemCant = fields[9].Get<uint32>();
                player->removeSpell(SpellId, SPEC_MASK_ALL, false);
                if (ReqSpellID == nIdBuy)
                {
                    nReqItemID = ReqItemID;
                    nReqItemCant = ReqItemCant;

                    if (player->HasSpell(ReqSpellID))
                    {
                        nLearnSpellID = SpellId;
                    }
                }
            } while (result->NextRow());
        }

        if (nLearnSpellID == 0)
        {
            itsok = 0;
        }
        if (itsok == 0)
        {
            if (nIdBuy == 1)
            {
                handler->PSendSysMessage("Reverted changes");
            }
            else
            {
                if (nReqItemID > 0)
                {
                    if (player->HasItemCount(nReqItemID, nReqItemCant))
                    {
                        player->DestroyItemCount(nReqItemID, nReqItemCant, true, false);
                        handler->PSendSysMessage("Unlocked appearance");
                        player->learnSpell(nIdBuy, false);
                    }
                    else
                    {
                        handler->PSendSysMessage("You don't have enough money to buy this");
                    }
                }
            }
        }
        else
        {
            handler->PSendSysMessage("Change made");
            player->learnSpell(nLearnSpellID, false);
        }

        return true;
    }

    static bool BarberInfo_Reload(ChatHandler* handler, const char* args)
    {
        if (!*args)
            return false;

        uint32 TravelnReqSpellID = 0;
 
        QueryResult result = WorldDatabase.Query("SELECT type,name,display,npc,racemask,SpellId,ReqSpellID,path,ReqItemID,ReqItemCant FROM custom_druid_barbershop WHERE guid > 0");
        
        Player *player = handler->GetSession()->GetPlayer();
        handler->PSendSysMessage("INITBarberDruid");
        if (result)
        {
            do
            {
                Field *fields = result->Fetch();

                std::string type = fields[0].Get<std::string>();
                std::string name = fields[1].Get<std::string>();
                std::string display = fields[2].Get<std::string>();
                std::string npc = fields[3].Get<std::string>();
                std::string racemask = fields[4].Get<std::string>();
                std::string SpellId = fields[5].Get<std::string>();
                std::string ReqSpellID = fields[6].Get<std::string>();
                std::string path = fields[7].Get<std::string>();
                std::string ReqItemID = fields[8].Get<std::string>();
                std::string ReqItemCant = fields[9].Get<std::string>();
                std::string ifknow = "false";

                uint32 ReqSpellIDA = fields[6].Get<uint32>();
                uint32 ReqItemCantA = fields[9].Get<uint32>();

                if (ReqSpellIDA == 0)
                {
                    ifknow = "true";
                }
                else
                {
                    if (player->HasSpell(ReqSpellIDA))
                    {
                        ifknow = "true";
                    }
                    else
                    {
                        if (ReqItemCantA == 0)
                        {
                            ifknow = "BLOCK";
                        }
                    }
                }
                handler->PSendSysMessage("CHARBarberDruid%s:%s:%s:nil:%s:%s:%s:%s", type.c_str(), name.c_str(), display.c_str(), ReqItemID.c_str(), ReqItemCant.c_str(), ReqSpellID.c_str(), ifknow.c_str());
            } while (result->NextRow());
        }
        handler->PSendSysMessage("FINIBarberDruid");
        return true;
    }

    static bool HandleGoQuestCommand(ChatHandler* handler, std::string_view type, Quest const* quest)
    {
        uint32 entry = quest->GetQuestId();

        if (type == "starter")
        {
            QuestRelations* qr = sObjectMgr->GetCreatureQuestRelationMap();

            for (auto itr = qr->begin(); itr != qr->end(); ++itr)
            {
                if (itr->second == entry)
                {
                    CreatureData const* spawnpoint = GetCreatureData(handler, itr->first);
                    if (!spawnpoint)
                    {
                        handler->SendErrorMessage(LANG_COMMAND_GOCREATNOTFOUND);
                        return false;
                    }

                    // We've found a creature, teleport to it.
                    return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
                }
            }

            qr = sObjectMgr->GetGOQuestRelationMap();

            for (auto itr = qr->begin(); itr != qr->end(); ++itr)
            {
                if (itr->second == entry)
                {
                    GameObjectData const* spawnpoint = GetGameObjectData(handler, itr->first);
                    if (!spawnpoint)
                    {
                        handler->SendErrorMessage(LANG_COMMAND_GOOBJNOTFOUND);
                        return false;
                    }

                    return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
                }
            }
        }
        else if (type == "ender")
        {
            QuestRelations* qr = sObjectMgr->GetCreatureQuestInvolvedRelationMap();

            for (auto itr = qr->begin(); itr != qr->end(); ++itr)
            {
                if (itr->second == entry)
                {
                    CreatureData const* spawnpoint = GetCreatureData(handler, itr->first);
                    if (!spawnpoint)
                    {
                        handler->SendErrorMessage(LANG_COMMAND_GOCREATNOTFOUND);
                        return false;
                    }

                    // We've found a creature, teleport to it.
                    return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
                }
            }

            qr = sObjectMgr->GetGOQuestInvolvedRelationMap();

            for (auto itr = qr->begin(); itr != qr->end(); ++itr)
            {
                if (itr->second == entry)
                {
                    GameObjectData const* spawnpoint = GetGameObjectData(handler, itr->first);
                    if (!spawnpoint)
                    {
                        handler->SendErrorMessage(LANG_COMMAND_GOOBJNOTFOUND);
                        return false;
                    }

                    return DoTeleport(handler, { spawnpoint->posX, spawnpoint->posY, spawnpoint->posZ }, spawnpoint->mapid);
                }
            }
        }
        else
        {
            handler->SendErrorMessage(LANG_CMD_GOQUEST_INVALID_SYNTAX);
            return false;
        }

        return false;
    }

    static CreatureData const* GetCreatureData(ChatHandler* handler, uint32 entry)
    {
        CreatureData const* spawnpoint = nullptr;
        for (auto const& pair : sObjectMgr->GetAllCreatureData())
        {
            if (pair.second.id1 != entry)
            {
                continue;
            }

            if (!spawnpoint)
            {
                spawnpoint = &pair.second;
            }
            else
            {
                handler->SendSysMessage(LANG_COMMAND_GOCREATMULTIPLE);
                break;
            }
        }

        return spawnpoint;
    }

    static GameObjectData const* GetGameObjectData(ChatHandler* handler, uint32 entry)
    {
        GameObjectData const* spawnpoint = nullptr;
        for (auto const& pair : sObjectMgr->GetAllGOData())
        {
            if (pair.second.id != entry)
            {
                continue;
            }

            if (!spawnpoint)
            {
                spawnpoint = &pair.second;
            }
            else
            {
                handler->SendSysMessage(LANG_COMMAND_GOCREATMULTIPLE);
                break;
            }
        }

        return spawnpoint;
    }
};

void AddSC_go_commandscript()
{
    new go_commandscript();
}
