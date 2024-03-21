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

#ifndef _INSTANCESAVEMGR_H
#define _INSTANCESAVEMGR_H

#include "DBCEnums.h"
#include "DatabaseEnv.h"
#include "Define.h"
#include "ObjectDefines.h"
#include "ObjectGuid.h"
#include <list>
#include <map>
#include <mutex>
#include <unordered_map>

struct InstanceTemplate;
struct MapEntry;
class Player;
class Group;
class InstanceSaveMgr;
class InstanceSave;

class InstanceSave
{
    friend class InstanceSaveMgr;
public:
    InstanceSave(uint16 MapId, uint32 InstanceId, Difficulty difficulty, uint32 entranceId, time_t resetTime, bool canReset);
    ~InstanceSave();

    uint32 GetPlayerCount() const { return uint32(m_playerList.size()); }
    uint32 GetGroupCount() const { return uint32(m_groupList.size()); }

    /* A map corresponding to the InstanceId/MapId does not always exist.
    InstanceSave objects may be created on player logon but the maps are
    created and loaded only when a player actually enters the instance. */
    uint32 GetInstanceId() const { return m_instanceid; }
    uint32 GetMapId() const { return m_mapid; }

    /* Saved when the instance is generated for the first time */
    void SaveToDB();
    /* When the instance is being reset (permanently deleted) */
    void DeleteFromDB();

    /* for normal instances this corresponds to max(creature respawn time) + X hours
       for raid/heroic instances this caches the global respawn time for the map */
    time_t GetResetTime() const { return m_resetTime; }
    void SetResetTime(time_t resetTime) { m_resetTime = resetTime; }
    time_t GetResetTimeForDB();

    InstanceTemplate const* GetTemplate();
    MapEntry const* GetMapEntry();

    void AddPlayer(Player* player)
    {
        std::lock_guard<std::mutex> lock(_lock);
        m_playerList.push_back(player);
    }

    bool RemovePlayer(Player* player)
    {
        _lock.lock();
        m_playerList.remove(player);
        bool isStillValid = UnloadIfEmpty();
        _lock.unlock();

        //delete here if needed, after releasing the lock
        if (m_toDelete)
            delete this;

        return isStillValid;
    }
    /* all groups bound to the instance */
    void AddGroup(Group* group) { m_groupList.push_back(group); }
    bool RemoveGroup(Group* group)
    {
        m_groupList.remove(group);
        bool isStillValid = UnloadIfEmpty();
        if (m_toDelete)
            delete this;
        return isStillValid;
    }

    /* instances cannot be reset (except at the global reset time)
           if there are players permanently bound to it
           this is cached for the case when those players are offline */
    bool CanReset() const { return m_canReset; }
    void SetCanReset(bool canReset) { m_canReset = canReset; }

    /* currently it is possible to omit this information from this structure
       but that would depend on a lot of things that can easily change in future */
    Difficulty GetDifficultyID() const { return m_difficulty; }

    /* used to flag the InstanceSave as to be deleted, so the caller can delete it */
    void SetToDelete(bool toDelete)
    {
        m_toDelete = toDelete;
    }

    [[nodiscard]] std::string GetInstanceData() const { return m_instanceData; }
    void SetInstanceData(std::string str) { m_instanceData = str; }
    [[nodiscard]] uint32 GetCompletedEncounterMask() const { return m_completedEncounterMask; }
    void SetCompletedEncounterMask(uint32 mask) { m_completedEncounterMask = mask; }

    typedef std::list<Player*> PlayerListType;
    typedef std::list<Group*> GroupListType;
private:
    bool UnloadIfEmpty();
    /* the only reason the instSave-object links are kept is because
       the object-instSave links need to be broken at reset time */
       /// @todo: Check if maybe it's enough to just store the number of players/groups
    PlayerListType m_playerList;
    GroupListType m_groupList;
    time_t m_resetTime;
    uint32 m_instanceid;
    uint32 m_mapid;
    Difficulty m_difficulty;
    uint32 m_entranceId;
    bool m_canReset;
    bool m_toDelete;

    std::string m_instanceData;
    uint32 m_completedEncounterMask;

    std::mutex _lock;
};

typedef std::unordered_map<uint32 /*PAIR32(map, difficulty)*/, time_t /*resetTime*/> ResetTimeByMapDifficultyMap;

class InstanceSaveMgr
{
    friend class InstanceSave;

private:
    InstanceSaveMgr()  = default;;
    ~InstanceSaveMgr();

public:
    static InstanceSaveMgr* instance();
    typedef std::unordered_map<uint32 /*InstanceId*/, InstanceSave*> InstanceSaveHashMap;

    void Unload();

    struct InstResetEvent
    {
        uint8 type;
        Difficulty difficulty : 8;
        uint32 mapid;
        uint32 instanceId;

        InstResetEvent() : type(0), difficulty(Difficulty::RAID_DIFFICULTY_25MAN_NORMAL), mapid(0), instanceId(0) { }
        InstResetEvent(uint8 t, uint32 _mapid, Difficulty d, uint32 _instanceid)
            : type(t), difficulty(d), mapid(_mapid), instanceId(_instanceid) { }
        bool operator==(InstResetEvent const& e) const { return e.instanceId == instanceId; }
    };
    typedef std::multimap<time_t /*resetTime*/, InstResetEvent> ResetTimeQueue;

    void LoadInstances();

    void LoadResetTimes();
    time_t GetResetTimeFor(uint32 mapid, Difficulty d) const
    {
        ResetTimeByMapDifficultyMap::const_iterator itr = m_resetTimeByMapDifficulty.find(MAKE_PAIR64(mapid, d));
        return itr != m_resetTimeByMapDifficulty.end() ? itr->second : 0;
    }
    time_t GetSubsequentResetTime(uint32 mapid, Difficulty difficulty, time_t resetTime) const;

    // Use this on startup when initializing reset times
    void InitializeResetTimeFor(uint32 mapid, Difficulty d, time_t t)
    {
        m_resetTimeByMapDifficulty[MAKE_PAIR64(mapid, d)] = t;
    }

    // Use this only when updating existing reset times
    void SetResetTimeFor(uint32 mapid, Difficulty d, time_t t);

    [[nodiscard]] ResetTimeByMapDifficultyMap const& GetResetTimeMap() const
    {
        return m_resetTimeByMapDifficulty;
    }
    void ScheduleReset(bool add, time_t time, InstResetEvent event);
    void ForceGlobalReset(uint32 mapId, Difficulty difficulty);

    void Update();

    InstanceSave* AddInstanceSave(uint32 mapId, uint32 instanceId, Difficulty difficulty, time_t resetTime, uint32 entranceId,
        bool canReset, bool load = false);
    void RemoveInstanceSave(uint32 InstanceId);
    void UnloadInstanceSave(uint32 InstanceId);
    static void DeleteInstanceFromDB(uint32 instanceid);

    InstanceSave* GetInstanceSave(uint32 InstanceId);

    /* statistics */
    uint32 GetNumInstanceSaves() const { return uint32(m_instanceSaveById.size()); }
    uint32 GetNumBoundPlayersTotal() const;
    uint32 GetNumBoundGroupsTotal() const;
protected:
    static uint16 ResetTimeDelay[];

private:
    void _ResetOrWarnAll(uint32 mapid, Difficulty difficulty, bool warn, time_t resetTime);
    void _ResetInstance(uint32 mapid, uint32 instanceId);
    void _ResetSave(InstanceSaveHashMap::iterator& itr);
    bool lock_instLists{false};
    InstanceSaveHashMap m_instanceSaveById;
    ResetTimeByMapDifficultyMap m_resetTimeByMapDifficulty;
    ResetTimeQueue m_resetTimeQueue;
};

#define sInstanceSaveMgr InstanceSaveMgr::instance()

#endif
