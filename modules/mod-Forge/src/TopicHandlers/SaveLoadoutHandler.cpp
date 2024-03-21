#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>
#include <boost/algorithm/string.hpp>

class SaveLoadoutHandler : public ForgeTopicHandler
{
public:
    SaveLoadoutHandler(ForgeCache* cache, ForgeCommonMessage* fcm) : ForgeTopicHandler(ForgeTopic::SAVE_LOADOUT)
    {
        fc = cache;
        cm = fcm;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {
        if (iam.message.empty())
            return;

        std::vector<std::string> results;
        boost::algorithm::split(results, iam.message, boost::is_any_of(";"));

        if (results.empty() || results.size() != 3 || !fc->isNumber(results[0]))
            return;

        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(iam.player, spec)) {
            uint32 id = static_cast<uint32>(std::stoul(results[0]));
            auto name = results[1];
            auto talentString = results[2];

            auto specId = fc->base64_char.find(talentString.substr(1, 1));
            if (specId != spec->CharacterSpecTabId)
                return;

            auto pClass = fc->base64_char.find(talentString.substr(2, 1));
            if (pClass != iam.player->getClass())
                return;

            auto classMap = fc->_cacheClassNodeToSpell[iam.player->getClassMask()];
            auto nodeMap = fc->_cacheSpecNodeToSpell[specId];
            int reqLen = classMap.size() + nodeMap.size() + fc->META_PREFIX;

            if (name.size() > fc->LOADOUT_NAME_MAX || talentString.size() != reqLen)
                return;

            auto guid = iam.player->GetGUID().GetCounter();
            auto plos = fc->_playerTalentLoadouts.find(guid);
            if (plos != fc->_playerTalentLoadouts.end())
            {
                auto spec = plos->second.find(specId);
                if (spec != plos->second.end()) {
                    CharacterDatabase.DirectExecute("update `forge_character_talent_loadouts` set `active` = 0 where `guid` = {} and `talentTabId` = {}",
                        guid, specId);

                    auto exists = spec->second.find(id);
                    if (exists != spec->second.end()) {
                        exists->second->name = name;
                        exists->second->talentString = talentString;

                        if (fc->_playerActiveTalentLoadouts[guid]->id != exists->second->id)
                            fc->_playerActiveTalentLoadouts[guid] = exists->second;
                    }
                    else {
                        if (spec->second.size() >= fc->MAX_LOADOUTS_PER_SPEC)
                            return;

                        auto active = fc->_playerActiveTalentLoadouts[guid];
                        active->active = false;
                        fc->_playerActiveTalentLoadouts[guid] = active;
                        fc->_playerTalentLoadouts[guid][specId][active->id] = active;
                        iam.player->SaveLoadoutActions(specId, active->id);

                        ForgeCache::PlayerLoadout* plo = new ForgeCache::PlayerLoadout();
                        plo->active = true;
                        plo->id = id;
                        plo->tabId = specId;
                        plo->name = name;
                        plo->talentString = talentString;

                        fc->_playerActiveTalentLoadouts[guid] = plo;
                        fc->_playerTalentLoadouts[guid][specId][id] = plo;
                        fc->LoadLoadoutActions(iam.player);
                    }

                    CharacterDatabase.DirectExecute("REPLACE into `forge_character_talent_loadouts` (`guid`, `id`, `talentTabId`, `name`, `talentString`, `active`) values ({}, {}, {}, '{}', '{}', {})",
                        guid, id, specId, name, talentString, true);
                }
            }

            cm->SendLoadouts(iam.player);
        }
    }

private:

    ForgeCache* fc;
    ForgeCommonMessage* cm;
};
