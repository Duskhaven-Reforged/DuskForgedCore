#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"
#include "WorldPacket.h"
#include "TopicRouter.h"
#include "ForgeCommonMessage.h"
#include <ForgeCache.cpp>

class ActivateClassSpecHandler : public ForgeTopicHandler
{
public:
    ActivateClassSpecHandler(ForgeCache* cache, ForgeCommonMessage* cmh) : ForgeTopicHandler(ForgeTopic::ACTIVATE_CLASS_SPEC)
    {
        fc = cache;
        cm = cmh;
    }

    void HandleMessage(ForgeAddonMessage& iam) override
    {

        if (sConfigMgr->GetBoolDefault("Forge.StrictSpecs", true)) {
            if (!fc->isNumber(iam.message))
                return;

            uint32 tabId = static_cast<uint32>(std::stoul(iam.message));
            ForgeCharacterSpec* spec;

            if (iam.player->IsInCombat() || iam.player->isDead()) {
                iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_SPEC_ERROR, "Now isn't the time for that.");
                return;
            }

            if (fc->TryGetCharacterActiveSpec(iam.player, spec))
            {
                if (spec->CharacterSpecTabId == tabId)
                {
                    iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_SPEC_ERROR, "Class specialization already active");
                    return;
                }

                auto reqLevel = fc->GetConfig("ReqiredSpecializationLevel", 10);
                if (iam.player->getLevel() < reqLevel)
                {
                    iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_SPEC_ERROR, "You must be level " + std::to_string(reqLevel) + " to pick a class specialization");
                    return;
                }

                auto info = sSpellMgr->GetSpellInfo(ACTIVATE_SPEC_SPELL);
                iam.player->SetSpecActivationAllowed(false);
                iam.player->RemoveOperationIfExists(ACTIVATE_SPEC_SPELL);
                iam.player->CastSpell(iam.player, info);
                iam.player->AddTimedDelayedOperation(ACTIVATE_SPEC_SPELL, getMSTime() + info->CastTimeEntry->CastTime+100, [iam, spec, tabId, this](){
                    if (iam.player->GetSpecActivationAllowed()) {
                        ForgeTalentTab* tab;
                        if (fc->TryGetTalentTab(iam.player, tabId, tab)) {
                            fc->ForgetTalents(iam.player, spec, CLASS_TREE);
                            fc->ForgetTalents(iam.player, spec, TALENT_TREE);
                            spec->CharacterSpecTabId = tabId;
                            LearnInitialSpecSpellsAndTalents(iam.player, tabId);
                            CharacterDatabase.Execute("update acore_characters.forge_character_specs set charspec = {}, active = 1 where guid = {}", tabId, iam.player->GetGUID().GetCounter());
                            
                            cm->SendSpecInfo(iam.player);
                            cm->SendTalents(iam.player);

                            iam.player->SendPlaySpellVisual(179); // 53 SpellCastDirected
                            iam.player->SendPlaySpellImpact(iam.player->GetGUID(), 362); // 113 EmoteSalute
                        }
                        else
                            iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_CLASS_SPEC_ERROR, "Invalid tab id for class.");

                        iam.player->SetSpecActivationAllowed(false);
                        iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_CLASS_SPEC, std::to_string(spec->CharacterSpecTabId));
                    }
                    });
            }
        }
        else
            iam.player->SendForgeUIMsg(ForgeTopic::ACTIVATE_CLASS_SPEC_ERROR, "Unknown Spec");
    }

private:
    ForgeCache* fc;
    ForgeCommonMessage* cm;
    uint32 const ACTIVATE_SPEC_SPELL = 63645;

    void LearnInitialSpecSpellsAndTalents(Player* player, uint32 tabId) {
        ForgeCharacterSpec* charSpec;
        if (fc->TryGetCharacterActiveSpec(player, charSpec)) {
            for (int i = 10; i < player->getLevel() + 1; i++) {
                auto level = fc->_levelClassSpellMap.find(i);
                if (level != fc->_levelClassSpellMap.end()) {
                    auto classSpells = level->second.find(player->getClass());
                    if (classSpells != level->second.end()) {
                        auto spec = classSpells->second.find(tabId);
                        if (spec != classSpells->second.end()) {
                            for (auto spell : spec->second) {
                                if (!player->HasSpell(spell)) {
                                    if (auto info = sSpellMgr->GetSpellInfo(spell)) {
                                        if (info->HasEffect(SPELL_EFFECT_LEARN_SPELL)) {
                                            for (uint8 i = 0; i < MAX_SPELL_EFFECTS; ++i)
                                                player->learnSpell(info->Effects[i].TriggerSpell);
                                        }
                                        else
                                            player->learnSpell(spell);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};
