#include "AnticheatScripts.h"
#include "AnticheatMgr.h"
#include "World.h"

AnticheatScripts::AnticheatScripts(): PlayerScript("AnticheatScripts") {}

void AnticheatScripts::OnLogout(Player* player)
{
    sAnticheatMgr->HandlePlayerLogout(player);
}

void AnticheatScripts::OnLogin(Player* player)
{
    sAnticheatMgr->HandlePlayerLogin(player);
}

void AnticheatScripts::OnUpdate(Player* player, uint32 diff)
{
    if (!sWorld->getBoolConfig(CONFIG_ANTICHEAT_OP_ACK_HACK_ENABLE) && !sWorld->getBoolConfig(CONFIG_ANTICHEAT_ENABLE))
    {
        return;
    }

    sAnticheatMgr->AckUpdate(player, diff);
}
