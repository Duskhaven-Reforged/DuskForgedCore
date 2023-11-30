#include "ForgeCommonMessage.h"

ForgeCommonMessage::ForgeCommonMessage(ForgeCache* cache)
{
    fc = cache;
}

ForgeCache* fc;

void ForgeCommonMessage::SendTalentTreeLayout(Player* player)
{

    for (auto tpt : fc->TALENT_POINT_TYPES)
    {
        std::list<ForgeTalentTab*> tabs;

        if (fc->TryGetForgeTalentTabs(player, tpt, tabs))
        {
            BuildTree(player, tpt, tabs);
        }
    }

    std::list<ForgeTalentTab*> tabs;

    if (fc->TryGetForgeTalentTabs(player, CharacterPointType::SKILL_PAGE, tabs))
    {
        BuildTree(player, CharacterPointType::SKILL_PAGE, tabs);
    }
}

void ForgeCommonMessage::SendTalentTreeLayout(Player* player, uint32 tab)
{
    ForgeTalentTab* tabObj;
    std::list<ForgeTalentTab*> tabs;

    if (fc->TryGetTalentTab(player, tab, tabObj))
    {
        tabs.push_back(tabObj);

        BuildTree(player, CharacterPointType::SKILL_PAGE, tabs);
    }

}

std::string ForgeCommonMessage::BuildTree(Player* player, CharacterPointType pointType, std::list<ForgeTalentTab*> tabs)
{

    for (const auto& tab : tabs)
    {
        std::string msg;

        msg = msg + std::to_string(tab->Id) + "^" +
            tab->Name + "^" + std::to_string(tab->SpellIconId) + "^" +
            tab->Background + "^" +
            tab->Description + "^" +
            std::to_string(tab->Role) + "^" +
            tab->SpellString + "^" +
            std::to_string((int)tab->TalentType) + "^" +
            std::to_string(tab->TabIndex) + "^";

        int i = 0;

        for (auto& talentKvp : tab->Talents)
        {
            std::string delimiter = "*";

            if (i == 0)
                delimiter = "";

            msg = msg + delimiter + std::to_string(tab->Id) + "&" +
                std::to_string(talentKvp.second->SpellId) + "&" +
                std::to_string(talentKvp.second->ColumnIndex) + "&" +
                std::to_string(talentKvp.second->RowIndex) + "&" +
                std::to_string(talentKvp.second->RankCost) + "&" +
                std::to_string(talentKvp.second->RequiredLevel) + "&" +
                std::to_string(talentKvp.second->TabPointReq) + "&" +
                std::to_string(talentKvp.second->NumberOfRanks) + "&" +
                std::to_string((int)talentKvp.second->PreReqType) + "&";

            int j = 0;

            for (auto& preReq : talentKvp.second->Prereqs)
            {
                std::string reqDel = "@";

                if (j == 0)
                    reqDel = "";

                msg = msg + reqDel + std::to_string(preReq->Talent) + "$" +
                    std::to_string(preReq->TalentTabId) + "$" +
                    std::to_string(preReq->RequiredRank);

                j++;
            }

            msg = msg + "&"; // delimit the field

            j = 0;

            for (auto& preReq : talentKvp.second->Ranks)
            {
                std::string reqDel = "%";

                if (j == 0)
                    reqDel = "";

                msg = msg + reqDel + std::to_string(preReq.first) + "~" +
                    std::to_string(preReq.second);

                j++;
            }

            msg = msg + "&"; // delimit the field

            j = 0;

            for (auto& preReq : talentKvp.second->UnlearnSpells)
            {
                std::string reqDel = "`";

                if (j == 0)
                    reqDel = "";

                msg = msg + reqDel + std::to_string(preReq);

                j++;
            }

            msg = msg + "&" + std::to_string(talentKvp.second->nodeType) + "&";

            for (auto& choice : talentKvp.second->Choices)
            {
                std::string choiceDel = "!";

                if (j == 0)
                    choiceDel = "";

                msg = msg + choiceDel + std::to_string(choice->spellId);
                j++;
            }

            i++;
        }

        player->SendForgeUIMsg(ForgeTopic::TALENT_TREE_LAYOUT, msg);
    }

    return "";
}

void ForgeCommonMessage::ApplyKnownForgeSpells(Player* player)
{
    ForgeCharacterSpec* spec;

    if (fc->TryGetCharacterActiveSpec(player, spec))
    {
        for (auto& tabkvp : spec->Talents)
        {

            for (auto& spellKvp : tabkvp.second)
            {
                ForgeTalentTab* tab = fc->TalentTabs[spellKvp.second->TabId];
                auto spell = tab->Talents[spellKvp.second->SpellId];
                auto fsId = spell->Ranks[spellKvp.second->CurrentRank];

                player->removeSpell(fsId, SPEC_MASK_ALL, false);

                for (auto s : spell->UnlearnSpells)
                    player->removeSpell(s, SPEC_MASK_ALL, false);

                if (!player->HasSpell(fsId))
                    player->learnSpell(fsId, false, false);
            }

        }

        player->SendInitialSpells();
    }
}

bool ForgeCommonMessage::CanLearnTalent(Player* player, uint32 tabId, uint32 spellId)
{
    ForgeTalentTab* tab;
    ForgeCharacterSpec* spec;
    CharacterPointType tabType;

    if (fc->TryGetTabPointType(tabId, tabType) &&
        fc->TryGetTalentTab(player, tabId, tab) &&
        fc->TryGetCharacterActiveSpec(player, spec))
    {

        ForgeCharacterPoint* curPoints = fc->GetSpecPoints(player, tabType, spec->Id);

        if (curPoints->Sum == 0)
            return false;

        // check dependants, rank requirements
        auto talItt = tab->Talents.find(spellId);

        if (talItt == tab->Talents.end())
            return false;

        auto ft = talItt->second;

        if (ft->RequiredLevel > player->getLevel())
            return false;

        if (ft->TabPointReq > spec->PointsSpent[tabId])
            return false;

        if (curPoints->Sum < ft->RankCost)
            return false;
        
        ForgeCharacterPoint* pointMax = fc->GetMaxPointDefaults(tabType);

        if (pointMax->Sum != 0 && spec->PointsSpent[tabId] + ft->RankCost > pointMax->Sum)
            return false;


        auto sklTItt = spec->Talents.find(tabId);
        std::unordered_map<uint32, ForgeCharacterTalent*> skillTabs;

        if (sklTItt == spec->Talents.end())
            skillTabs = spec->Talents[tabId];
        else
            skillTabs = sklTItt->second;

        int reqsNotMet = 0;
        bool anyPrereq = false;
        for (auto& preReq : ft->Prereqs)
        {
            if (preReq->RequiredRank == 0)
                continue;

            auto preReqTalent = tab->Talents.find(preReq->Talent);

            if (preReqTalent == tab->Talents.end())
                continue;

            if (preReqTalent->second->NumberOfRanks == 0)
                continue;

            if (tabId == preReq->TalentTabId)
            {
                auto skillItt = skillTabs.find(preReq->Talent);

                if ((skillItt == skillTabs.end() || preReq->RequiredRank > skillItt->second->CurrentRank) && !anyPrereq)
                {
                    continue;
                }
                anyPrereq = true;
            }
            else
            {
                auto fsItt = spec->Talents.find(tabId);

                if (fsItt == spec->Talents.end())
                {
                    auto tsItt = spec->Talents.find(tabId);

                    if (tsItt == spec->Talents.end())
                    {
                        reqsNotMet++;
                        continue;
                    }

                    auto typeItt = tsItt->second.find(preReq->reqId);

                    if (typeItt == tsItt->second.end())
                    {
                        reqsNotMet++;
                        continue;
                    }

                    if (typeItt->second->CurrentRank < preReq->RequiredRank)
                    {
                        reqsNotMet++;
                        continue;
                    }
                }
                else
                {
                    auto typeItt = fsItt->second.find(preReq->reqId);

                    if (typeItt == fsItt->second.end())
                    {
                        reqsNotMet++;
                        continue;
                    }

                    if (typeItt->second->CurrentRank < preReq->RequiredRank)
                    {
                        reqsNotMet++;
                        continue;
                    }
                }
            }
        }

        if (!anyPrereq)
            reqsNotMet++;

        if (reqsNotMet)
        {
            if (ft->PreReqType == PereqReqirementType::ALL)
            {
                return false;
            }
            else if (ft->Prereqs.size() == reqsNotMet)
            {
                return false;
            }
        }

        return true;
    }

    return false;
}

void ForgeCommonMessage::SendTalents(Player* player, uint32 tabId)
{
    std::unordered_map<uint32, ForgeCharacterTalent*> spec;
    CharacterPointType pt;

    if (fc->TryGetTabPointType(tabId, pt))
    {
        fc->TryGetCharacterTalents(player, tabId, spec);

        std::string clientMsg = std::to_string(tabId) + "^" + std::to_string((int)pt) + "^";

        clientMsg = DoBuildRanks(spec, player, clientMsg, tabId);

        player->SendForgeUIMsg(ForgeTopic::GET_TALENTS, clientMsg);
    }
    else
        player->SendForgeUIMsg(ForgeTopic::GET_TALENT_ERROR, "Unknown Tab");
}


void ForgeCommonMessage::SendTalents(Player* player)
{
    ForgeCharacterSpec* spec;
    if (fc->TryGetCharacterActiveSpec(player, spec))
    {
        uint32 i = 0;

        for (auto tpt : fc->TALENT_POINT_TYPES)
        {
            std::list<ForgeTalentTab*> tabs;
            if (fc->TryGetForgeTalentTabs(player, tpt, tabs))
            {
                std::string clientMsg;
                for (auto* tab : tabs)
                {
                    std::string delimiter = ";";

                    if (i == 0)
                        delimiter = "";
                    //clientMsg = clientMsg + delimiter + std::to_string(tab->Id) + "^" + std::to_string((int)tab->TalentType) + "^";
                    clientMsg = std::to_string(tab->Id) + "^" + std::to_string((int)tab->TalentType) + "^";
                    std::unordered_map<uint32, ForgeCharacterTalent*> spec;
                    fc->TryGetCharacterTalents(player, tab->Id, spec);
                    clientMsg = DoBuildRanks(spec, player, clientMsg, tab->Id);

                    player->SendForgeUIMsg(ForgeTopic::GET_TALENTS, clientMsg);
                    i++;
                }
            }
        }
        
    }
}

void ForgeCommonMessage::SendXmogSet(Player* player, uint8 setId) {
    std::string clientMsg = std::to_string(setId) + "^";
    if (setId < (uint8)sConfigMgr->GetIntDefault("Transmogrification.MaxSets", 10)) {
        clientMsg += fc->BuildXmogFromSet(player, setId);
    }
    else {
        clientMsg += fc->BuildXmogFromEquipped(player);
    }
    player->SendForgeUIMsg(ForgeTopic::LOAD_XMOG_SET, clientMsg);
}

void ForgeCommonMessage::SendXmogSets(Player* player) {
    auto msg = fc->BuildXmogSetsMsg(player);
    player->SendForgeUIMsg(ForgeTopic::GET_XMOG_SETS, msg);
}

std::string ForgeCommonMessage::DoBuildRanks(std::unordered_map<uint32, ForgeCharacterTalent*>& spec, Player* player, std::string clientMsg, uint32 tabId)
{
    int i = 0;
    ForgeTalentTab* tab;
    ForgeCharacterSpec* fs;

    if (fc->TryGetTalentTab(player, tabId, tab) && fc->TryGetCharacterActiveSpec(player, fs))
    {
        for(auto& sp : tab->Talents)
        {
            std::string delimiter =  i ? "*" : "";
            
            auto itt = spec.find(sp.first);

            if (itt != spec.end()) {
                if (itt->second->type == NodeType::CHOICE)
                    if (itt->second->CurrentRank == 0)
                        clientMsg = clientMsg + delimiter + std::to_string(sp.second->SpellId) + "~0";
                    else
                        clientMsg = clientMsg + delimiter + std::to_string(itt->second->SpellId) + "~" + std::to_string(fs->ChoiceNodesChosen.at(itt->second->SpellId));
                else
                    clientMsg = clientMsg + delimiter + std::to_string(itt->second->SpellId) + "~" + std::to_string(itt->second->CurrentRank);
            }
            else
                clientMsg = clientMsg + delimiter + std::to_string(sp.second->SpellId) + "~0";

            i++;
        }
    }

    return clientMsg;
}


void ForgeCommonMessage::SendSpecInfo(Player* player)
{
    std::list<ForgeCharacterSpec*> specs;
    if (fc->TryGetAllCharacterSpecs(player, specs))
    {
        std::string msg;
        int i = 0;

        for (auto& spec : specs)
        {
            std::string delimiter = ";";

            if (i == 0)
                delimiter = "";

            msg = msg + delimiter + std::to_string(spec->Id) + "^" +
                spec->Name + "^" +
                spec->Description + "^" +
                std::to_string((int)spec->Active) + "^" +
                std::to_string(spec->SpellIconId) + "^" +
                std::to_string((int)spec->Visability) + "^" +
                std::to_string((int)spec->CharacterSpecTabId) + "^";

            int j = 0;

            for (auto& kvp : spec->PointsSpent)
            {
                delimiter = "%";

                if (j == 0)
                    delimiter = "";

                msg = msg + delimiter + std::to_string(kvp.first) + "~" + std::to_string(kvp.second);

                j++;
            }

            msg = msg + "^";
            int k = 0;

            for (auto tpt : fc->TALENT_POINT_TYPES)
            {
                delimiter = "@";

                if (k == 0)
                    delimiter = "";

                ForgeCharacterPoint* m = fc->GetMaxPointDefaults(tpt);
                ForgeCharacterPoint* tp = fc->GetCommonCharacterPoint(player, tpt);
                ForgeCharacterPoint* cps = fc->GetSpecPoints(player, tpt, spec->Id);

                msg = msg + delimiter + std::to_string((int)tpt) + "$" + std::to_string(cps->Sum) + "$" + std::to_string(tp->Sum) + "$"  + std::to_string(m->Sum) + "$" + std::to_string(m->Max);
                k++;
            }

            i++;
        }

        player->SendForgeUIMsg(ForgeTopic::GET_CHARACTER_SPECS, msg);
    }
}

void ForgeCommonMessage::SendActiveSpecInfo(Player* player)
{
    ForgeCharacterSpec* spec;

    if (fc->TryGetCharacterActiveSpec(player, spec))
    {
        std::string msg;
        std::string delimiter = "";

        msg = msg + std::to_string(spec->Id) + "^" +
            spec->Name + "^" +
            spec->Description + "^" +
            std::to_string((int)spec->Active) + "^" +
            std::to_string(spec->SpellIconId) + "^" +
            std::to_string((int)spec->Visability) + "^" +
            std::to_string((int)spec->CharacterSpecTabId) + "^";

        int j = 0;

        for (auto& kvp : spec->PointsSpent)
        {
            delimiter = "%";

            if (j == 0)
                delimiter = "";

            msg = msg + delimiter + std::to_string(kvp.first) + "~" + std::to_string(kvp.second);

            j++;
        }

        msg = msg + "^";
        int k = 0;

        for (auto tpt : fc->TALENT_POINT_TYPES)
        {
            delimiter = "@";

            if (k == 0)
                delimiter = "";

            ForgeCharacterPoint* m = fc->GetMaxPointDefaults(tpt);
            ForgeCharacterPoint* tp = fc->GetCommonCharacterPoint(player, tpt);
            ForgeCharacterPoint* cps = fc->GetSpecPoints(player, tpt, spec->Id);

            msg = msg + delimiter + std::to_string((int)tpt) + "$" + std::to_string(cps->Sum) + "$" + std::to_string(tp->Sum) + "$" + std::to_string(m->Sum) + "$" + std::to_string(m->Max);
            k++;
        }

        player->SendForgeUIMsg(ForgeTopic::GET_CHARACTER_SPECS, msg);
    }
}

std::string ForgeCommonMessage::EncodeTalentString(Player* player) {
    std::string export_str = "";
    ForgeCharacterSpec* spec;

    if (fc->TryGetCharacterActiveSpec(player, spec))
    {
        auto talents = spec->Talents.find(spec->CharacterSpecTabId);
        if (talents != spec->Talents.end()) {
            if (talents->second.empty())
                return export_str;

            size_t head = 0;
            size_t byte = 0;
            auto put_bit = [&export_str, &head, &byte, this](size_t bits, unsigned value) {
                for (size_t i = 0; i < bits; i++)
                {
                    size_t bit = head % byte_size;
                    byte += (value >> i & 0b1) << bit;
                    if (bit == byte_size - 1)
                    {
                        export_str += base64_char[byte];
                        byte = 0;
                    }
                    head++;
                }
                };

            put_bit(version_bits, LOADOUT_SERIALIZATION_VERSION);
            put_bit(spec_bits, static_cast<unsigned>(spec->CharacterSpecTabId));
            put_bit(tree_bits, 0);

            //std::map<unsigned, std::vector<std::pair<const trait_data_t*, unsigned>>> tree_nodes;

        }
    }
}