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
            if (talentKvp.second) {
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

                msg = msg + "&" + std::to_string(talentKvp.second->nodeType) + "&"
                    + std::to_string(talentKvp.second->nodeIndex) + "&";

                for (auto& choice : talentKvp.second->Choices)
                {
                    std::string choiceDel = "!";

                    if (j == 0)
                        choiceDel = "";

                    msg = msg + choiceDel + std::to_string(choice.second->spellId);
                    j++;
                }
                i++;
            }
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

void ForgeCommonMessage::SendTalents(Player* player)
{
    ForgeCharacterSpec* spec;
    if (fc->TryGetCharacterActiveSpec(player, spec))
    {
        auto pClass = player->getClass();
        auto cSpec = spec->CharacterSpecTabId;
        for (auto tpt : fc->TALENT_POINT_TYPES) {
            std::string clientMsg = base64_char.substr(tpt+1, 1)+base64_char.substr(cSpec, 1)+base64_char.substr(pClass, 1);
            switch (tpt) {
            case CharacterPointType::TALENT_TREE: {
                if (!sConfigMgr->GetBoolDefault("echos", false)) {
                    uint32 i = 0;
                    auto classTree = spec->Talents[fc->_cacheClassNodeToClassTree[player->getClassMask()]];
                    auto classMap = fc->_cacheClassNodeToSpell[player->getClassMask()];

                    auto specTree = spec->Talents[spec->CharacterSpecTabId];
                    auto specMap = fc->_cacheSpecNodeToSpell[spec->CharacterSpecTabId];

                    if (classTree.size() == classMap.size()) {
                        for (int i = 1; i <= classMap.size(); i++) {
                            clientMsg += base64_char.substr(classTree[classMap[i]]->CurrentRank + 1, 1);
                        }
                    }

                    if (specTree.size() == specMap.size()) {
                        for (int i = 1; i <= specMap.size(); i++) {
                            clientMsg += base64_char.substr(specTree[specMap[i]]->CurrentRank + 1, 1);
                        }
                    }
                }
                else {
                    std::list<ForgeTalentTab*> tabs;
                    if (fc->TryGetForgeTalentTabs(player, tpt, tabs)) {
                        for (auto tab : tabs) {
                            auto specTree = spec->Talents[tab->Id];
                            auto specMap = fc->_cacheSpecNodeToSpell[tab->Id];
                            for (int i = 1; i <= specMap.size(); i++) {
                                clientMsg += base64_char.substr(specTree[specMap[i]]->CurrentRank + 1, 1);
                            }
                        }
                    }
                }

                player->SendForgeUIMsg(ForgeTopic::GET_TALENTS, clientMsg);
                break;
            }
            case CharacterPointType::PRESTIGE_TREE:
            case CharacterPointType::RACIAL_TREE: {
                std::list<ForgeTalentTab*> tabs;
                if (fc->TryGetForgeTalentTabs(player, tpt, tabs)) {

                    for (auto tab : tabs) {
                        auto specTree = spec->Talents[tab->Id];
                        for (auto talent : specTree) {
                            clientMsg += base64_char.substr(talent.second->CurrentRank + 1, 1);
                        }
                    }

                    player->SendForgeUIMsg(ForgeTopic::GET_TALENTS, clientMsg);
                }
                break;
            }
            default:
                break;
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

                msg = msg + delimiter + std::to_string((int)tpt) + "$" + std::to_string(cps->Sum) + "$" + std::to_string(tp->Sum) + "$"  + std::to_string(cps->Max) + "$" + std::to_string(m->Max);
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

void ForgeCommonMessage::SendLoadouts(Player* player)
{
    auto found = fc->_playerTalentLoadouts.find(player->GetGUID().GetCounter());
    if (found != fc->_playerTalentLoadouts.end()) {
        std::string msg;
        std::string delim = "";
        for (auto spec : found->second) {
            msg += delim + std::to_string(spec.first) + "$";
            auto idDelim = "";
            for (auto loadout : spec.second) {
                msg += idDelim + std::to_string(loadout.first) + "^"
                    + std::to_string(loadout.second->active) + "^"
                    + loadout.second->name + "^" + loadout.second->talentString;
                idDelim = "~";
            }
            delim = "*";
        }

        player->SendForgeUIMsg(ForgeTopic::GET_LOADOUTS, msg);
    }
}
