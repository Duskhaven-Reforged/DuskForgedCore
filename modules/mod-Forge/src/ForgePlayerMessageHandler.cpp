/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "Guild.h"
#include "GuildMgr.h"
#include "Spell.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "Gamemode.h"
#include <ActivateSpecHandler.cpp>
#include <GetTalentTreeHandler.cpp>
#include <GetCharacterSpecsHandler.cpp>
#include <LearnTalentHandler.cpp>
#include "ForgeCommonMessage.h"
#include <RespecTalentsHandler.cpp>
#include <UnlearnTalentHandler.cpp>
#include <UpdateSpecHandler.cpp>
#include <GetTalentsHandler.cpp>
#include <PrestigeHandler.cpp>
#include <UseSkillbook.cpp>
#include <unordered_map>
#include <ForgeCache.cpp>
#include <ForgeCacheCommands.cpp>
#include <ActivateClassSpecHandler.cpp>
#include <GetCollectionsHandler.cpp>
/* #include <ApplyTransmogHandler.cpp>
#include <GetTransmogHandler.cpp>
#include <GetTransmogSetsHandler.cpp>
#include <SaveTransmogSetHandler.cpp> */
#include <StartMythicHandler.cpp>
#include <GetAffixesHandler.cpp>
#include <GetCharacterLoadoutsHandler.cpp>
#include <DeleteLoadoutHandler.cpp>
#include <SaveLoadoutHandler.cpp>

#include <unordered_map>
#include <random>

// Add player scripts
class ForgePlayerMessageHandler : public PlayerScript
{
public:
    ForgePlayerMessageHandler(ForgeCache* cache, ForgeCommonMessage* cmh) : PlayerScript("ForgePlayerMessageHandler")
    {
        fc = cache;
        cm = cmh;
    }

    void OnCreate(Player* player) override
    {
        // setup DB
        player->SetSpecsCount(0);
        fc->AddCharacterSpecSlot(player);
        fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::RACIAL_TREE, fc->GetConfig("InitialPoints", 8));
        fc->UpdateCharacters(player->GetSession()->GetAccountId(), player);

        if (sConfigMgr->GetBoolDefault("echos", false)) {
            fc->EchosDefaultLoadout(player);
        }
        else {
            fc->AddDefaultLoadout(player);
        }
    }

    void OnEquip(Player* player, Item* item, uint8 bag, uint8 slot, bool update) override
    {

    }

    void OnFirstLogin(Player* player) override {
        if (sConfigMgr->GetBoolDefault("StarterGuild.autojoin", false))
            if (Guild* guild = sGuildMgr->GetGuildById(sConfigMgr->GetIntDefault("StarterGuild.id", 0)))
                guild->AddMember(player->GetGUID());
    }

    void OnLogin(Player* player) override
    {
        if (!player)
            return;

        LearnSpellsForLevel(player);
        fc->ApplyAccountBoundTalents(player);
        fc->UnlearnFlaggedSpells(player);

        fc->LoadLoadoutActions(player);
    }

    void OnLogout(Player* player) override
    {
        if (!player)
            return;

        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(player, spec)) {
            auto active = fc->_playerActiveTalentLoadouts.find(player->GetGUID().GetCounter());
            if (active != fc->_playerActiveTalentLoadouts.end())
                player->SaveLoadoutActions(spec->CharacterSpecTabId, active->second->id);
        }
    }

    void OnDelete(ObjectGuid guid, uint32 accountId) override
    {
        fc->DeleteCharacter(guid, accountId);
        fc->UpdateCharacters(accountId, nullptr);
    }

    // receive message from client
    // since we sent the messag to ourselves the server will not route it back to the player.
    void OnBeforeSendChatMessage(Player* player, uint32& type, uint32& lang, std::string& msg) override
    {
        sTopicRouter->Route(player, type, lang, msg);
    }

    void OnLevelChanged(Player* player, uint8 oldlevel) override
    {
        player->SetFreeTalentPoints(0);

        ForgeCharacterSpec* spec;
        if (fc->TryGetCharacterActiveSpec(player, spec))
        {
            uint8 currentLevel = player->getLevel();
            uint32 levelMod = fc->GetConfig("levelMod", 2);
            if (oldlevel < currentLevel) {
                int levelDiff = currentLevel - oldlevel;

                //if (currentLevel == fc->GetConfig("MaxLevel", 80))
                //{
                //    fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::PRESTIGE_TREE, fc->GetConfig("PrestigePointsAtMaxLevel", 5));
                //}

                if (currentLevel >= 10)
                {
                    if (oldlevel < 10 && levelDiff > 1)
                        levelDiff -= (9 - oldlevel);

                    if (levelDiff > 1) {
                        int div = levelDiff / 2;
                        int rem = levelDiff % 2;
                        fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::TALENT_TREE, div);
                        if (rem)
                            div += 1;

                        fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::CLASS_TREE, div);
                    }
                    else {
                        if (currentLevel % 2)
                            fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::TALENT_TREE, 1);
                        else
                            fc->AddCharacterPointsToAllSpecs(player, CharacterPointType::CLASS_TREE, 1);
                    }

                    cm->SendActiveSpecInfo(player);
                    cm->SendTalents(player);
                }
                cm->SendSpecInfo(player);
                fc->UpdateCharacterSpec(player, spec);
                LearnSpellsForLevel(player);
            }
        }
    }

    void OnLearnSpell(Player* player, uint32 spellID) 
    {
        // check if its forged.
        if (auto* fs = fc->GetTalent(player, spellID))
        {
            if (fs->CurrentRank != 0)
            {
                auto* tab = fc->TalentTabs[fs->TabId];
                auto* spell = tab->Talents[fs->SpellId];
                auto fsId = spell->Ranks[fs->CurrentRank];

                for (auto s : spell->UnlearnSpells)
                    player->removeSpell(s, SPEC_MASK_ALL, false);

                if (!player->HasSpell(fsId))
                    player->learnSpell(fsId);
            }
        }

        fc->LearnExtraSpellsIfAny(player, spellID);
    }

    void OnLootItem(Player* player, Item* item, uint32 count, ObjectGuid lootguid) override
    {

    }

    //called after player.additem is called. DO NOT CREATE LOOPS WITH THIS.
    void OnAddItem(Player* player, uint32 item, uint32 count)
    {
        
    }

    void OnCreateItem(Player* player, Item* item, uint32 count) override
    {
        OnAddItem(player, item->GetTemplate()->ItemId, count);
    }

    void OnQuestRewardItem(Player* player, Item* item, uint32 count) override
    {
        OnAddItem(player, item->GetTemplate()->ItemId, count);
    }


    /*void OnGiveXP(Player* player, uint32& amount, Unit* victim) override
    {
        if (Gamemode::HasGameMode(player, GameModeType::CLASSIC))
            return;

        if (player->getLevel() <= 9)
            amount *= fc->GetConfig("Dynamic.XP.Rate.1-9", 2);

        else if (player->getLevel() <= 19)
            amount *= fc->GetConfig("Dynamic.XP.Rate.10-19", 2);

        else if (player->getLevel() <= 29)
            amount *= fc->GetConfig("Dynamic.XP.Rate.20-29", 3);

        else if (player->getLevel() <= 39)
            amount *= fc->GetConfig("Dynamic.XP.Rate.30-39", 3);

        else if (player->getLevel() <= 49)
            amount *= fc->GetConfig("Dynamic.XP.Rate.40-49", 3);

        else if (player->getLevel() <= 59)
            amount *= fc->GetConfig("Dynamic.XP.Rate.50-59", 3);

        else if (player->getLevel() <= 69)
            amount *= fc->GetConfig("Dynamic.XP.Rate.60-69", 4);

        else if (player->getLevel() <= 79)
            amount *= fc->GetConfig("Dynamic.XP.Rate.70-79", 4);
    }*/

    void GenerateItem(CustomItemTemplate* itemProto, Player const* owner) override
    {
        itemProto->SetStackable(1);
        itemProto->Save();
        owner->SendItemQueryPacket(itemProto);
    }

private:
    TopicRouter* Router;
    ForgeCache* fc;
    ForgeCommonMessage* cm;

    void LearnSpellsForLevel(Player* player) {
        if (player->HasUnitState(UNIT_STATE_DIED))
            player->RemoveAurasByType(SPELL_AURA_FEIGN_DEATH);

        auto pClass = fc->_levelClassSpellMap.find(player->getClass());
        if (pClass != fc->_levelClassSpellMap.end()) {
            for (auto race : pClass->second) {
                if (player->getRaceMask() & race.first) {
                    for (auto level : race.second) {
                        if (level.first <= player->getLevel()) {
                            for (auto spell : level.second) {
                                if (player->HasSpell(spell))
                                    continue;

                                player->learnSpell(spell);
                            }
                        }
                    }
                }
            }
        }
    }
};

// Add all scripts in one
void AddForgePlayerMessageHandler()
{
    ForgeCache* cache = ForgeCache::instance();
    ForgeCommonMessage* cm = new ForgeCommonMessage(cache);

    new ForgePlayerMessageHandler(cache, cm);
    sTopicRouter->AddHandler(new ActivateSpecHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTalentsHandler(cache, cm));
    sTopicRouter->AddHandler(new GetCharacterSpecsHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTalentTreeHandler(cache, cm));
    sTopicRouter->AddHandler(new LearnTalentHandler(cache, cm));
    sTopicRouter->AddHandler(new UnlearnTalentHandler(cache, cm));
    sTopicRouter->AddHandler(new RespecTalentsHandler(cache, cm));
    sTopicRouter->AddHandler(new UpdateSpecHandler(cache));
    sTopicRouter->AddHandler(new PrestigeHandler(cache, cm));
    sTopicRouter->AddHandler(new ActivateClassSpecHandler(cache, cm));
/*  sTopicRouter->AddHandler(new GetCollectionsHandler(cache, cm));
    sTopicRouter->AddHandler(new ApplyTransmogHandler(cache, cm));
    sTopicRouter->AddHandler(new SaveTransmogSetHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTransmogSetsHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTransmogHandler(cache, cm)); */
    sTopicRouter->AddHandler(new StartMythicHandler(cache, cm));
    sTopicRouter->AddHandler(new GetAffixesHandler(cache, cm));
    sTopicRouter->AddHandler(new GetCharacterLoadoutsHandler(cache, cm));
    sTopicRouter->AddHandler(new DeleteLoadoutHandler(cache, cm));
    sTopicRouter->AddHandler(new SaveLoadoutHandler(cache, cm));
    //new UseSkillBook();
    new ForgeCacheCommands();   
}
