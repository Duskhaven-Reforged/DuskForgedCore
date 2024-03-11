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

#include "InstanceSaveMgr.h"
#include "DBCStores.h"
#include "Common.h"
#include "Config.h"
#include "GameTime.h"
#include "GridNotifiers.h"
#include "Group.h"
#include "InstanceScript.h"
#include "Log.h"
#include "Map.h"
#include "MapInstanced.h"
#include "MapMgr.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "Timer.h"
#include "Transport.h"
#include "World.h"

uint16 InstanceSaveMgr::ResetTimeDelay[] = {3600, 900, 300, 60, 0};

InstanceSaveMgr::~InstanceSaveMgr()
{
    lock_instLists = true;
    // pussywizard: crashes on calling function in destructor (PlayerUnbindInstance), completely not needed anyway
    /*for (InstanceSaveHashMap::iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end(); ++itr)
    {
        InstanceSave* save = itr->second;

        InstanceSave::PlayerListType &pList = save->m_playerList;
        while (!pList.empty())
            PlayerUnbindInstance(*(pList.begin()), save->GetMapId(), save->GetDifficulty(), false);

        delete save;
    }*/
}

InstanceSaveMgr* InstanceSaveMgr::instance()
{
    static InstanceSaveMgr instance;
    return &instance;
}

void InstanceSaveMgr::Unload()
{
    lock_instLists = true;
    for (InstanceSaveHashMap::iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end(); ++itr)
    {
        InstanceSave* save = itr->second;

        for (InstanceSave::PlayerListType::iterator itr2 = save->m_playerList.begin(), next = itr2; itr2 != save->m_playerList.end(); itr2 = next)
        {
            ++next;
            (*itr2)->UnbindInstance(save->GetMapId(), save->GetDifficultyID(), true);
        }

        for (InstanceSave::GroupListType::iterator itr2 = save->m_groupList.begin(), next = itr2; itr2 != save->m_groupList.end(); itr2 = next)
        {
            ++next;
            (*itr2)->UnbindInstance(save->GetMapId(), save->GetDifficultyID(), true);
        }

        delete save;
    }
}

/*
- adding instance into manager
*/
InstanceSave* InstanceSaveMgr::AddInstanceSave(uint32 mapId, uint32 instanceId, Difficulty difficulty, time_t resetTime, uint32 entranceId, bool canReset, bool startup /*=false*/)
{
    if (InstanceSave* old_save = GetInstanceSave(instanceId))
        return old_save;

    MapEntry const* entry = sMapStore.LookupEntry(mapId);
    if (!entry)
    {
        LOG_ERROR("instance.save", "InstanceSaveMgr::AddInstanceSave: wrong mapid = {}, instanceid = {}!", mapId, instanceId);
        return nullptr;
    }

    if (instanceId == 0)
    {
        LOG_ERROR("instance.save", "InstanceSaveMgr::AddInstanceSave: mapid = {}, wrong instanceid = {}!", mapId, instanceId);
        return nullptr;
    }

    if (difficulty >= (entry->IsRaid() ? MAX_RAID_DIFFICULTY : MAX_DUNGEON_DIFFICULTY))
    {
        LOG_ERROR("instance.save", "InstanceSaveMgr::AddInstanceSave: mapid = {}, instanceid = {}, wrong dificalty {}!", mapId, instanceId, difficulty);
        return nullptr;
    }

    if (!resetTime)
    {
        // initialize reset time
        // for normal instances if no creatures are killed the instance will reset in two hours
        if (entry->IsRaid() || difficulty > Difficulty::DUNGEON_DIFFICULTY_HEROIC)
            resetTime = GetResetTimeFor(mapId, difficulty);
        else
        {
            resetTime = time(nullptr) + 2 * HOUR;
            // normally this will be removed soon after in InstanceMap::Add, prevent error
            ScheduleReset(true, resetTime, InstResetEvent(0, mapId, difficulty, instanceId));
        }
    }

    InstanceSave* save = new InstanceSave(mapId, instanceId, difficulty, entranceId, resetTime, canReset);
    if (!startup)
        save->SaveToDB();

    m_instanceSaveById[instanceId] = save;
    return save;
}

InstanceSave* InstanceSaveMgr::GetInstanceSave(uint32 InstanceId)
{
    InstanceSaveHashMap::iterator itr = m_instanceSaveById.find(InstanceId);
    return itr != m_instanceSaveById.end() ? itr->second : nullptr;
}

void InstanceSaveMgr::DeleteInstanceFromDB(uint32 instanceid)
{
    CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_INSTANCE_BY_INSTANCE);
    stmt->SetData(0, instanceid);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_CHAR_INSTANCE_BY_INSTANCE);
    stmt->SetData(0, instanceid);
    trans->Append(stmt);

    stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_INSTANCE_BY_INSTANCE);
    stmt->SetData(0, instanceid);
    trans->Append(stmt);

    CharacterDatabase.CommitTransaction(trans);
    // Respawn times should be deleted only when the map gets unloaded
}

void InstanceSaveMgr::RemoveInstanceSave(uint32 InstanceId)
{
    InstanceSaveHashMap::iterator itr = m_instanceSaveById.find(InstanceId);
    if (itr != m_instanceSaveById.end())
    {
        // save the resettime for normal instances only when they get unloaded
        if (time_t resettime = itr->second->GetResetTimeForDB())
        {
            CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_INSTANCE_RESETTIME);

            stmt->SetData(0, uint64(resettime));
            stmt->SetData(1, InstanceId);

            CharacterDatabase.Execute(stmt);
        }

        itr->second->SetToDelete(true);
        m_instanceSaveById.erase(itr);
    }
}

void InstanceSaveMgr::UnloadInstanceSave(uint32 InstanceId)
{
    if (InstanceSave* save = GetInstanceSave(InstanceId))
        save->UnloadIfEmpty();
}

InstanceSave::InstanceSave(uint16 MapId, uint32 InstanceId, Difficulty difficulty, uint32 entranceId, time_t resetTime, bool canReset)
    : m_resetTime(resetTime), m_instanceid(InstanceId), m_mapid(MapId),
    m_difficulty(difficulty), m_entranceId(entranceId), m_canReset(canReset), m_toDelete(false) { }

InstanceSave::~InstanceSave()
{
    // the players and groups must be unbound before deleting the save
    ASSERT(m_playerList.empty() && m_groupList.empty());
}

void InstanceSave::SaveToDB()
{
    std::string data;
    uint32 completedEncounters = 0;

    Map* map = sMapMgr->FindMap(GetMapId(), m_instanceid);
    if (map)
    {
        ASSERT(map->IsDungeon());
        if (InstanceScript* instanceScript = map->ToInstanceMap()->GetInstanceScript())
        {
            data = instanceScript->GetSaveData();
            completedEncounters = instanceScript->GetCompletedEncounterMask();
            m_entranceId = instanceScript->GetEntranceLocation();
        }
    }

    CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_INS_INSTANCE_SAVE);
    stmt->SetData(0, m_instanceid);
    stmt->SetData(1, GetMapId());
    stmt->SetData(2, uint32(GetResetTimeForDB()));
    stmt->SetData(3, uint8(GetDifficultyID()));
    stmt->SetData(4, completedEncounters);
    stmt->SetData(5, data);
    stmt->SetData(6, m_entranceId);
    CharacterDatabase.Execute(stmt);

    sScriptMgr->OnInstanceSave(this);
}

time_t InstanceSave::GetResetTimeForDB()
{
    // only save the reset time for normal instances
    MapEntry const* entry = sMapStore.LookupEntry(GetMapId());
    if (!entry || entry->map_type == MAP_RAID || GetDifficultyID() == DUNGEON_DIFFICULTY_HEROIC)
        return 0;
    else
        return GetResetTime();
}

// to cache or not to cache, that is the question
InstanceTemplate const* InstanceSave::GetTemplate()
{
    return sObjectMgr->GetInstanceTemplate(m_mapid);
}

MapEntry const* InstanceSave::GetMapEntry()
{
    return sMapStore.LookupEntry(m_mapid);
}

void InstanceSave::DeleteFromDB()
{
    InstanceSaveMgr::DeleteInstanceFromDB(GetInstanceId());
}

/* true if the instance save is still valid */
bool InstanceSave::UnloadIfEmpty()
{
    if (m_playerList.empty() && m_groupList.empty())
    {
        // don't remove the save if there are still players inside the map
        if (Map* map = sMapMgr->FindMap(GetMapId(), GetInstanceId()))
            if (map->HavePlayers())
                return true;

        if (!sInstanceSaveMgr->lock_instLists)
            sInstanceSaveMgr->RemoveInstanceSave(GetInstanceId());

        return false;
    }
    else
        return true;
}

void InstanceSaveMgr::LoadInstances()
{
    uint32 oldMSTime = getMSTime();

    // Delete character_instance for non-existent character
    CharacterDatabase.DirectExecute("DELETE ci.* FROM character_instance AS ci LEFT JOIN characters AS c ON ci.guid = c.guid WHERE c.guid IS NULL");

    // Delete expired normal instances (normals expire after 3 days even if someone is still bound to them, cleared on startup)
    CharacterDatabase.DirectExecute("DELETE FROM instance WHERE resettime > 0 AND resettime < UNIX_TIMESTAMP()");

    // Delete instance with no binds
    CharacterDatabase.DirectExecute("DELETE i.* FROM instance AS i LEFT JOIN character_instance AS ci ON i.id = ci.instance WHERE ci.guid IS NULL");

    // Delete creature_respawn, gameobject_respawn and creature_instance for non-existent instance
    CharacterDatabase.DirectExecute("DELETE FROM creature_respawn WHERE instanceId > 0 AND instanceId NOT IN (SELECT id FROM instance)");
    CharacterDatabase.DirectExecute("DELETE FROM gameobject_respawn WHERE instanceId > 0 AND instanceId NOT IN (SELECT id FROM instance)");
    CharacterDatabase.DirectExecute("DELETE tmp.* FROM character_instance AS tmp LEFT JOIN instance ON tmp.instance = instance.id WHERE tmp.instance > 0 AND instance.id IS NULL");

    // Clean invalid references to instance
    CharacterDatabase.DirectExecute("UPDATE corpse SET instanceId = 0 WHERE instanceId > 0 AND instanceId NOT IN (SELECT id FROM instance)");
    CharacterDatabase.DirectExecute("UPDATE characters AS tmp LEFT JOIN instance ON tmp.instance_id = instance.id SET tmp.instance_id = 0 WHERE tmp.instance_id > 0 AND instance.id IS NULL");

    // Initialize instance id storage (Needs to be done after the trash has been clean out)
    sMapMgr->InitInstanceIds();

    // Load reset times and clean expired instances
    sInstanceSaveMgr->LoadResetTimes();

    LOG_INFO("server.loading", ">> Loaded Instances And Binds in {} ms", GetMSTimeDiffToNow(oldMSTime));
    LOG_INFO("server.loading", " ");
}

void InstanceSaveMgr::LoadResetTimes()
{
    time_t now = GameTime::GetGameTime().count();
    time_t today = (now / DAY) * DAY;

    // get the current reset times for normal instances (these may need to be updated)
    // these are only kept in memory for InstanceSaves that are loaded later
    // resettime = 0 in the DB for raid/heroic instances so those are skipped
    typedef std::pair<uint32 /*PAIR32(map, difficulty)*/, time_t> ResetTimeMapDiffType;
    typedef std::map<uint32, ResetTimeMapDiffType> InstResetTimeMapDiffType;
    InstResetTimeMapDiffType instResetTime;

    // index instance ids by map/difficulty pairs for fast reset warning send
    typedef std::multimap<uint32 /*PAIR32(map, difficulty)*/, uint32 /*instanceid*/ > ResetTimeMapDiffInstances;
    typedef std::pair<ResetTimeMapDiffInstances::const_iterator, ResetTimeMapDiffInstances::const_iterator> ResetTimeMapDiffInstancesBounds;
    ResetTimeMapDiffInstances mapDiffResetInstances;

    if (QueryResult result = CharacterDatabase.Query("SELECT id, map, difficulty, resettime FROM instance ORDER BY id ASC"))
    {
        do
        {
            Field* fields = result->Fetch();

            uint32 instanceId = fields[0].Get<uint32>();

            // Mark instance id as being used
            sMapMgr->RegisterInstanceId(instanceId);

            if (time_t resettime = time_t(fields[3].Get<uint64>()))
            {
                uint32 mapid = fields[1].Get<uint16>();
                uint32 difficulty = fields[2].Get<uint8>();

                instResetTime[instanceId] = ResetTimeMapDiffType(MAKE_PAIR32(mapid, difficulty), resettime);
                mapDiffResetInstances.insert(ResetTimeMapDiffInstances::value_type(MAKE_PAIR32(mapid, difficulty), instanceId));
            }
        } while (result->NextRow());

        // schedule the reset times
        for (InstResetTimeMapDiffType::iterator itr = instResetTime.begin(); itr != instResetTime.end(); ++itr)
            if (itr->second.second > now)
                ScheduleReset(true, itr->second.second, InstResetEvent(0, PAIR32_LOPART(itr->second.first), Difficulty(PAIR32_HIPART(itr->second.first)), itr->first));
    }

    // load the global respawn times for raid/heroic instances
    uint32 diff = sWorld->getIntConfig(CONFIG_INSTANCE_RESET_TIME_HOUR) * HOUR;
    QueryResult result = CharacterDatabase.Query("SELECT mapid, difficulty, resettime FROM instance_reset");
    if (result)
    {
        do
        {
            Field* fields = result->Fetch();
            uint32 mapid = fields[0].Get<uint16>();
            Difficulty difficulty = Difficulty(fields[1].Get<uint8>());
            uint64 resettime = fields[2].Get<uint64>();

            MapDifficulty const* mapDiff = GetMapDifficultyData(mapid, difficulty);
            if (!mapDiff)
            {
                LOG_ERROR("instance.save", "InstanceSaveMgr::LoadResetTimes: invalid mapid({})/difficulty({}) pair in instance_reset!", mapid, difficulty);
                CharacterDatabase.DirectExecute("DELETE FROM instance_reset WHERE mapid = '{}' AND difficulty = '{}'", mapid, difficulty);
                continue;
            }

            // update the reset time if the hour in the configs changes
            uint64 newresettime = (resettime / DAY) * DAY + diff;
            if (resettime != newresettime)
            {
                CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GLOBAL_INSTANCE_RESETTIME);
                stmt->SetData(0, uint64(newresettime));
                stmt->SetData(1, uint16(mapid));
                stmt->SetData(2, uint8(difficulty));
                CharacterDatabase.Execute(stmt);
            }

            InitializeResetTimeFor(mapid, difficulty, newresettime);
        } while (result->NextRow());
    }

    // calculate new global reset times for expired instances and those that have never been reset yet
    // add the global reset times to the priority queue
    for (MapDifficultyMap::const_iterator itr = sMapDifficultyMap.begin(); itr != sMapDifficultyMap.end(); ++itr)
    {
        uint32 map_diff_pair = itr->first;
        uint32 mapid = PAIR32_LOPART(map_diff_pair);
        Difficulty difficulty = Difficulty(PAIR32_HIPART(map_diff_pair));
        MapDifficulty const* mapDiff = &itr->second;
        if (!mapDiff->resetTime)
            continue;

        // the reset_delay must be at least one day
        uint32 period = uint32(((mapDiff->resetTime * sWorld->getRate(RATE_INSTANCE_RESET_TIME)) / DAY) * DAY);
        if (period < DAY)
            period = DAY;

        time_t t = GetResetTimeFor(mapid, difficulty);
        if (!t)
        {
            // initialize the reset time
            t = today + period + diff;
            SetResetTimeFor(mapid, difficulty, t);
            CharacterDatabase.DirectExecute("INSERT INTO instance_reset VALUES ('{}', '{}', '{}')", mapid, difficulty, (uint32)t);
        }

        if (t < now)
        {
            // assume that expired instances have already been cleaned
            // calculate the next reset time
            t = (t * DAY) / DAY;
            t += ((today - t) / period + 1) * period + diff;
            CharacterDatabase.DirectExecute("UPDATE instance_reset SET resettime = '{}' WHERE mapid = '{}' AND difficulty = '{}'", (uint32)t, mapid, difficulty);
        }

        InitializeResetTimeFor(mapid, difficulty, t);

        // schedule the global reset/warning
        uint8 type;
        for (type = 1; type < 4; ++type)
            if (t - ResetTimeDelay[type - 1] > now)
                break;

        ScheduleReset(true, t - ResetTimeDelay[type - 1], InstResetEvent(type, mapid, difficulty, 0));

        ResetTimeMapDiffInstancesBounds range = mapDiffResetInstances.equal_range(MAKE_PAIR32(mapid, difficulty));
        for (; range.first != range.second; ++range.first)
            ScheduleReset(true, t - ResetTimeDelay[type - 1], InstResetEvent(type, mapid, difficulty, range.first->second));
    }
}

time_t InstanceSaveMgr::GetSubsequentResetTime(uint32 mapid, Difficulty difficulty, time_t resetTime) const
{
    MapDifficulty const* mapDiff = GetMapDifficultyData(mapid, difficulty);
    if (!mapDiff || !mapDiff->resetTime)
    {
        LOG_ERROR("misc", "InstanceSaveManager::GetSubsequentResetTime: not valid difficulty or no reset delay for map %u", mapid);
        return 0;
    }

    time_t diff = sWorld->getIntConfig(CONFIG_INSTANCE_RESET_TIME_HOUR) * HOUR;
    time_t period = uint32(((mapDiff->resetTime * sWorld->getRate(RATE_INSTANCE_RESET_TIME)) / DAY) * DAY);
    if (period < DAY)
        period = DAY;

    return ((resetTime + MINUTE) / DAY * DAY) + period + diff;
}

void InstanceSaveMgr::SetResetTimeFor(uint32 mapid, Difficulty d, time_t t)
{
    ResetTimeByMapDifficultyMap::iterator itr = m_resetTimeByMapDifficulty.find(MAKE_PAIR64(mapid, d));
    ASSERT(itr != m_resetTimeByMapDifficulty.end());
    itr->second = t;
}

void InstanceSaveMgr::ScheduleReset(bool add, time_t time, InstResetEvent event)
{
    if (!add)
    {
        // find the event in the queue and remove it
        ResetTimeQueue::iterator itr;
        std::pair<ResetTimeQueue::iterator, ResetTimeQueue::iterator> range;
        range = m_resetTimeQueue.equal_range(time);
        for (itr = range.first; itr != range.second; ++itr)
        {
            if (itr->second == event)
            {
                m_resetTimeQueue.erase(itr);
                return;
            }
        }

        // in case the reset time changed (should happen very rarely), we search the whole queue
        if (itr == range.second)
        {
            for (itr = m_resetTimeQueue.begin(); itr != m_resetTimeQueue.end(); ++itr)
            {
                if (itr->second == event)
                {
                    m_resetTimeQueue.erase(itr);
                    return;
                }
            }

            if (itr == m_resetTimeQueue.end())
                LOG_ERROR("misc", "InstanceSaveManager::ScheduleReset: cannot cancel the reset, the event(%d, %d, %d) was not found!", event.type, event.mapid, event.instanceId);
        }
    }
    else
        m_resetTimeQueue.insert(std::pair<time_t, InstResetEvent>(time, event));
}

void InstanceSaveMgr::ForceGlobalReset(uint32 mapId, Difficulty difficulty)
{
    if (!GetDownscaledMapDifficultyData(mapId, difficulty))
        return;
    // remove currently scheduled reset times
    ScheduleReset(false, 0, InstResetEvent(1, mapId, difficulty, 0));
    ScheduleReset(false, 0, InstResetEvent(4, mapId, difficulty, 0));
    // force global reset on the instance
    _ResetOrWarnAll(mapId, difficulty, false, time(nullptr));
}

void InstanceSaveMgr::Update()
{
    time_t now = GameTime::GetGameTime().count();
    time_t t;
    bool resetOccurred = false;

    while (!m_resetTimeQueue.empty())
    {
        t = m_resetTimeQueue.begin()->first;
        if (t >= now)
            break;

        InstResetEvent& event = m_resetTimeQueue.begin()->second;
        if (event.type == 0)
        {
            // for individual normal instances, max creature respawn + X hours
            _ResetInstance(event.mapid, event.instanceId);
            m_resetTimeQueue.erase(m_resetTimeQueue.begin());
        }
        else
        {
            // global reset/warning for a certain map
            time_t resetTime = GetResetTimeFor(event.mapid, event.difficulty);
            _ResetOrWarnAll(event.mapid, event.difficulty, event.type != 4, resetTime);
            if (event.type != 4)
            {
                // schedule the next warning/reset
                ++event.type;
                ScheduleReset(true, resetTime - ResetTimeDelay[event.type - 1], event);
            }
            m_resetTimeQueue.erase(m_resetTimeQueue.begin());
        }
    }

    // pussywizard: send updated calendar and raid info
    if (resetOccurred)
    {
        LOG_INFO("instance.save", "Instance ID reset occurred, sending updated calendar and raid info to all players!");
        WorldPacket dummy;
        for (SessionMap::const_iterator itr = sWorld->GetAllSessions().begin(); itr != sWorld->GetAllSessions().end(); ++itr)
            if (Player* plr = itr->second->GetPlayer())
            {
                itr->second->HandleCalendarGetCalendar(dummy);
                plr->SendRaidInfo();
            }
    }
}

void InstanceSaveMgr::_ResetSave(InstanceSaveHashMap::iterator& itr)
{
    lock_instLists = true;

    bool shouldDelete = true;
    InstanceSave::PlayerListType& pList = itr->second->m_playerList;
    std::vector<Player*> temp; // list of expired binds that should be unbound

    for (Player* player : pList)
    {
        if (InstancePlayerBind* bind = player->GetBoundInstance(itr->second->GetMapId(), itr->second->GetDifficultyID()))
        {
            ASSERT(bind->save == itr->second);
            if (bind->perm && bind->extendState) // permanent and not already expired
            {
                // actual promotion in DB already happened in caller
                bind->extendState = bind->extendState == EXTEND_STATE_EXTENDED ? EXTEND_STATE_NORMAL : EXTEND_STATE_EXPIRED;
                shouldDelete = false;
                continue;
            }
        }
        temp.push_back(player);
    }
    for (Player* player : temp)
    {
        player->UnbindInstance(itr->second->GetMapId(), itr->second->GetDifficultyID(), true);
    }

    InstanceSave::GroupListType& gList = itr->second->m_groupList;
    while (!gList.empty())
    {
        Group* group = *(gList.begin());
        group->UnbindInstance(itr->second->GetMapId(), itr->second->GetDifficultyID(), true);
    }

    if (shouldDelete)
    {
        delete itr->second;
        itr = m_instanceSaveById.erase(itr);
    }
    else
        ++itr;

    lock_instLists = false;
}

void InstanceSaveMgr::_ResetInstance(uint32 mapid, uint32 instanceId)
{
    LOG_DEBUG("maps", "InstanceSaveMgr::_ResetInstance %u, %u", mapid, instanceId);
    Map const* map = sMapMgr->CreateBaseMap(mapid);
    if (!map->Instanceable())
        return;

    InstanceSaveHashMap::iterator itr = m_instanceSaveById.find(instanceId);
    if (itr != m_instanceSaveById.end())
        _ResetSave(itr);

    DeleteInstanceFromDB(instanceId);                       // even if save not loaded

    Map* iMap = ((MapInstanced*)map)->FindInstanceMap(instanceId);

    if (iMap && iMap->IsDungeon())
        ((InstanceMap*)iMap)->Reset(INSTANCE_RESET_RESPAWN_DELAY);

    if (iMap)
    {
        iMap->DeleteRespawnTimes();
        iMap->DeleteCorpseData();
    }
    else
        Map::DeleteRespawnTimesInDB(mapid, instanceId);

    // Free up the instance id and allow it to be reused
    sMapMgr->FreeInstanceId(instanceId);
}

void InstanceSaveMgr::_ResetOrWarnAll(uint32 mapid, Difficulty difficulty, bool warn, time_t resetTime)
{
    // global reset for all instances of the given map
    MapEntry const* mapEntry = sMapStore.LookupEntry(mapid);
    if (!mapEntry->Instanceable())
        return;

    time_t now = GameTime::GetGameTime().count();

    if (!warn)
    {
        MapDifficulty const* mapDiff = GetMapDifficultyData(mapid, difficulty);
        if (!mapDiff || !mapDiff->resetTime)
        {
            LOG_ERROR("instance.save", "InstanceSaveMgr::ResetOrWarnAll: not valid difficulty or no reset delay for map {}", mapid);
            return;
        }

        time_t next_reset = GetSubsequentResetTime(mapid, difficulty, resetTime);
        if (!next_reset)
            return;

        // delete/promote instance binds from the DB, even if not loaded
        CharacterDatabaseTransaction trans = CharacterDatabase.BeginTransaction();

        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_EXPIRED_CHAR_INSTANCE_BY_MAP_DIFF);
        stmt->SetData(0, uint16(mapid));
        stmt->SetData(1, uint8(difficulty));
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_GROUP_INSTANCE_BY_MAP_DIFF);
        stmt->SetData(0, uint16(mapid));
        stmt->SetData(1, uint8(difficulty));
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_DEL_EXPIRED_INSTANCE_BY_MAP_DIFF);
        stmt->SetData(0, uint16(mapid));
        stmt->SetData(1, uint8(difficulty));
        trans->Append(stmt);

        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_EXPIRE_CHAR_INSTANCE_BY_MAP_DIFF);
        stmt->SetData(0, uint16(mapid));
        stmt->SetData(1, uint8(difficulty));
        trans->Append(stmt);

        CharacterDatabase.CommitTransaction(trans);

        // promote loaded binds to instances of the given map
        for (InstanceSaveHashMap::iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end();)
        {
            if (itr->second->GetMapId() == mapid && itr->second->GetDifficultyID() == difficulty)
                _ResetSave(itr);
            else
                ++itr;
        }

        SetResetTimeFor(mapid, difficulty, next_reset);
        ScheduleReset(true, time_t(next_reset - 3600), InstResetEvent(1, mapid, difficulty, 0));

        // Update it in the DB
        stmt = CharacterDatabase.GetPreparedStatement(CHAR_UPD_GLOBAL_INSTANCE_RESETTIME);

        stmt->SetData(0, uint64(next_reset));
        stmt->SetData(1, uint16(mapid));
        stmt->SetData(2, uint8(difficulty));

        CharacterDatabase.Execute(stmt);
    }

    // now loop all existing maps to warn / reset
    Map const* map = sMapMgr->CreateBaseMap(mapid);
    MapInstanced::InstancedMaps& instMaps = ((MapInstanced*)map)->GetInstancedMaps();
    MapInstanced::InstancedMaps::iterator mitr;
    uint32 timeLeft;

    for (mitr = instMaps.begin(); mitr != instMaps.end(); ++mitr)
    {
        Map* map2 = mitr->second;
        if (!map2->IsDungeon() || map2->GetDifficulty() != difficulty)
            continue;

        if (warn)
        {
            if (now >= resetTime)
                timeLeft = 0;
            else
                timeLeft = uint32(resetTime - now);

            map2->ToInstanceMap()->SendResetWarnings(timeLeft);
        }
        else
        {
            map2->ToInstanceMap()->Reset(INSTANCE_RESET_GLOBAL);
        }
    }
}

uint32 InstanceSaveMgr::GetNumBoundPlayersTotal() const
{
    uint32 ret = 0;
    for (InstanceSaveHashMap::const_iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end(); ++itr)
        ret += itr->second->GetPlayerCount();

    return ret;
}

uint32 InstanceSaveMgr::GetNumBoundGroupsTotal() const
{
    uint32 ret = 0;
    for (InstanceSaveHashMap::const_iterator itr = m_instanceSaveById.begin(); itr != m_instanceSaveById.end(); ++itr)
        ret += itr->second->GetGroupCount();

    return ret;
}
