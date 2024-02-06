/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license: https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
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
#include <ApplyTransmogHandler.cpp>
#include <GetTransmogHandler.cpp>
#include <GetTransmogSetsHandler.cpp>
#include <SaveTransmogSetHandler.cpp>
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
        if (sConfigMgr->GetBoolDefault("echos", false)) {
            if (auto pProto = item->GetTemplate()) {
                if (pProto->Quality >= ITEM_QUALITY_UNCOMMON && (pProto->Class == ITEM_CLASS_ARMOR || pProto->Class == ITEM_CLASS_WEAPON)
                    && slot != EQUIPMENT_SLOT_TABARD) {
                    CustomItemTemplate custom = GetItemTemplate(pProto->ItemId);
                    custom->AdjustForLevel(player);
                }
            }
        }
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

        if (sConfigMgr->GetBoolDefault("echos", false)) {
            for (uint8 i = EQUIPMENT_SLOT_START; i < EQUIPMENT_SLOT_END; ++i) {
                if (Item* item = player->GetItemByPos(INVENTORY_SLOT_BAG_0, i)) {
                    if (ItemTemplate const* temp = item->GetTemplate()) {
                        if (temp->Quality >= ITEM_QUALITY_UNCOMMON && (temp->Class == ITEM_CLASS_ARMOR || temp->Class == ITEM_CLASS_WEAPON)) {
                            CustomItemTemplate custom = GetItemTemplate(temp->ItemId);
                            custom->AdjustForLevel(player);
                        }
                    }
                }
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
        OnAddItem(player, item->GetTemplate()->ItemId, count);
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

    void GenerateItem(CustomItemTemplate* itemProto, Player const* owner) override
    {
        std::random_device rd; // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        std::uniform_int_distribution<> coinflip(0, 1);

        itemProto->MakeBlankSlate();
        itemProto->SetBonding(NO_BIND);

        auto e = 2.7183;
        auto invType = itemProto->GetInventoryType();

        ItemModType stats[MAINSTATS] = { ITEM_MOD_STRENGTH, ITEM_MOD_AGILITY, ITEM_MOD_INTELLECT };

        auto slot = fc->_forgeItemSlotValues.find(InventoryType(invType));
        if (slot != fc->_forgeItemSlotValues.end()) {
            float slotmod = slot->second;
            uint32 maxIlvlBase = sConfigMgr->GetIntDefault("WorldTier.base.level", 60);

            float ilvl = float(std::min(maxIlvlBase, itemProto->GetItemLevel())) + uint8(owner->GetWorldTier()) * 10.f;
            itemProto->SetItemLevel(ilvl);
            itemProto->SetRequiredLevel(1);
            auto qual = itemProto->GetQuality();
            auto formula = 0.f;
            auto secondaryRolls = 0;

            switch (qual) {
            case ITEM_QUALITY_UNCOMMON:
                formula = (1.3f * ilvl + 180.f) / 70.f;
                secondaryRolls = 1;
                break;
            case ITEM_QUALITY_RARE:
                formula = (1.3f * ilvl + 180.f) / 65.f;
                secondaryRolls = 2;
                break;
            default: // epic+
                formula = (1.3f * ilvl + 180.f) / 60.f;
                secondaryRolls = 3;
                break;
            }
            auto itemSlotVal = pow(e, formula);
            itemProto->SetItemSlotValue(itemSlotVal);
            auto itemValue = itemSlotVal * slot->second;
            auto curValue = itemValue;

            auto statCount = 2;
            if (itemValue) {
                itemProto->SetBonding(BIND_WHEN_PICKED_UP);

                auto mainStat = itemProto->GenerateMainStatForItem();
                bool tankDist = itemProto->CanRollTank() ? coinflip(gen) ? true : false : false;

                float amountForAttributes = itemValue * .58f;
                float amountForStam = amountForAttributes / 2;
                itemProto->SetStatType(statCount - 1, ITEM_MOD_STAMINA);
                auto amount = amountForStam / fc->_forgeItemStatValues[ITEM_MOD_STAMINA];
                itemProto->SetStatValue(statCount - 1, amount);
                itemProto->SetStatValueMax(statCount - 1, amount);
                curValue -= amountForStam;

                auto amountForMainStat = amountForAttributes - amountForStam;
                itemProto->SetStatsCount(statCount);
                itemProto->SetStatType(statCount - 2, mainStat);
                amount = amountForMainStat / fc->_forgeItemStatValues[mainStat];
                itemProto->SetStatValue(statCount - 2, amount);
                itemProto->SetStatValueMax(statCount - 2, amount);
                curValue -= amountForMainStat;

                std::vector<ItemModType> rolled = {};
                if (itemProto->IsWeapon()) {
                    float dps = itemProto->CalculateDps();
                    if (dps) {

                        if (mainStat == ITEM_MOD_INTELLECT) {
                            auto sp = itemSlotVal * 2.435;

                            itemProto->SetStatsCount(statCount);
                            itemProto->SetStatType(statCount, ITEM_MOD_SPELL_POWER);
                            itemProto->SetStatValue(statCount, sp);
                            itemProto->SetStatValueMax(statCount, sp);
                            statCount++;
                            dps -= sp / 4;

                            rolled.push_back(ITEM_MOD_SPELL_POWER);
                        }
                        std::uniform_int_distribution<> dpsdistr(18, 36);

                        float range = (dps * float(itemProto->GetDelay() / 1000.f)) * 2;
                        float split = range / 2.f;
                        float var = float(dpsdistr(gen) / 100.f) * split;
                        int low = split - var;
                        int top = split + var;

                        itemProto->SetDamageMinA(low);
                        itemProto->SetMaxDamageMinA(low);
                        itemProto->SetDamageMaxA(top);
                        itemProto->SetMaxDamageMaxA(top);
                    }
                    else { return; }
                }
                else /*armor*/ {
                    switch (itemProto->GetSubClass()) {
                    case ITEM_SUBCLASS_ARMOR_PLATE: {
                        auto baseArmor = itemProto->GetItemLevel() * 8.f * slotmod;
                        auto amountForArmor = curValue / 3;
                        auto bonusArmor = amountForArmor / fc->_forgeItemStatValues[ITEM_MOD_RESILIENCE_RATING];

                        itemProto->SetArmor(int32(baseArmor + bonusArmor));
                        itemProto->SetArmorDamageModifier(int32(bonusArmor));
                        secondaryRolls--;

                        auto tankRoll = fc->_forgeItemSecondaryStatPools[ITEM_MOD_STAMINA];
                        std::uniform_int_distribution<> statdistr(1, tankRoll.size());
                        std::uniform_int_distribution<> secondarydistr(0, 8);

                        auto roll = tankRoll[statdistr(gen) - 1];
                        auto valueForThis = (curValue / secondaryRolls) * (1.f + (secondarydistr(gen) / 100.f));
                        auto amountForTankStat = valueForThis / fc->_forgeItemStatValues[roll];
                        statCount++;
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, roll);
                        itemProto->SetStatValue(statCount - 1, amountForTankStat);
                        itemProto->SetStatValueMax(statCount - 1, amountForTankStat);
                        secondaryRolls--;
                        curValue -= amountForTankStat;
                        rolled.push_back(roll);
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_MAIL: {
                        auto baseArmor = itemProto->GetItemLevel() * 4.5f * slotmod;
                        auto amountForArmor = curValue / 6;
                        auto bonusArmor = amountForArmor / fc->_forgeItemStatValues[ITEM_MOD_RESILIENCE_RATING];

                        itemProto->SetArmor(int32(baseArmor + bonusArmor));
                        itemProto->SetArmorDamageModifier(int32(bonusArmor));
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_LEATHER: {
                        itemProto->SetArmor(itemProto->GetItemLevel() * 2.15 * slotmod);
                        auto amountForAP = (curValue / 3) / fc->_forgeItemStatValues[ITEM_MOD_SPELL_POWER];
                        statCount++;
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, ITEM_MOD_ATTACK_POWER);
                        itemProto->SetStatValue(statCount - 1, amountForAP);
                        itemProto->SetStatValueMax(statCount - 1, amountForAP);
                        secondaryRolls--;
                        curValue -= amountForAP;
                        rolled.push_back(ITEM_MOD_ATTACK_POWER);
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_CLOTH: {
                        itemProto->SetArmor(itemProto->GetItemLevel() * 1.08 * slotmod);
                        auto amountForSP = (curValue / 3) / fc->_forgeItemStatValues[ITEM_MOD_SPELL_POWER];
                        statCount++;
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, ITEM_MOD_SPELL_POWER);
                        itemProto->SetStatValue(statCount - 1, amountForSP);
                        itemProto->SetStatValueMax(statCount - 1, amountForSP);
                        curValue -= amountForSP;
                        secondaryRolls--;
                        rolled.push_back(ITEM_MOD_SPELL_POWER);
                        break;
                    }
                    case ITEM_SUBCLASS_ARMOR_SHIELD: {
                        itemProto->SetArmor(itemProto->GetItemLevel() * 36.5f * slotmod);
                        itemProto->SetBlock(1.f * ilvl);
                        auto amountForBR = (curValue / 3) / fc->_forgeItemStatValues[ITEM_MOD_BLOCK_RATING];
                        statCount++;
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, ITEM_MOD_BLOCK_RATING);
                        itemProto->SetStatValue(statCount - 1, amountForBR);
                        itemProto->SetStatValueMax(statCount - 1, amountForBR);
                        secondaryRolls--;
                        curValue -= amountForBR;
                        rolled.push_back(ITEM_MOD_BLOCK_RATING);
                    }
                    }
                }


                std::uniform_int_distribution<> secondarydistr(0, 8);

                for (int i = 0; i < secondaryRolls;) {
                    auto rolledTank = tankDist ? coinflip(gen) ? ITEM_MOD_STAMINA : mainStat : mainStat;
                    auto statsToRoll = fc->_forgeItemSecondaryStatPools[rolledTank];
                    std::uniform_int_distribution<> statdistr(1, statsToRoll.size());

                    auto roll = statdistr(gen) - 1;
                    auto generated = statsToRoll[roll];
                    if (std::find(rolled.begin(), rolled.end(), generated) == rolled.end()) {
                        float split = secondaryRolls > 1 ? secondarydistr(gen) : 0;
                        auto valueForThis = curValue / (secondaryRolls - i) * (1.f + split / 100.f);
                        auto amount = valueForThis / fc->_forgeItemStatValues[generated];

                        statCount++;
                        itemProto->SetStatsCount(statCount);
                        itemProto->SetStatType(statCount - 1, generated);
                        itemProto->SetStatValue(statCount - 1, amount);
                        itemProto->SetStatValueMax(statCount - 1, amount);

                        curValue -= valueForThis;
                        rolled.push_back(generated);
                        i++;
                    }
                }

            }

            itemProto->Save();
            owner->SendItemQueryPacket(itemProto);
        }
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
    sTopicRouter->AddHandler(new GetCollectionsHandler(cache, cm));
    sTopicRouter->AddHandler(new ApplyTransmogHandler(cache, cm));
    sTopicRouter->AddHandler(new SaveTransmogSetHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTransmogSetsHandler(cache, cm));
    sTopicRouter->AddHandler(new GetTransmogHandler(cache, cm));
    sTopicRouter->AddHandler(new StartMythicHandler(cache, cm));
    sTopicRouter->AddHandler(new GetAffixesHandler(cache, cm));
    sTopicRouter->AddHandler(new GetCharacterLoadoutsHandler(cache, cm));
    sTopicRouter->AddHandler(new DeleteLoadoutHandler(cache, cm));
    sTopicRouter->AddHandler(new SaveLoadoutHandler(cache, cm));
    //new UseSkillBook();
    new ForgeCacheCommands();   
}
