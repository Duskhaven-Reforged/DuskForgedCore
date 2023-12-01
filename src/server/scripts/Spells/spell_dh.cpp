#include "AreaTrigger.h"
#include "AreaTriggerAI.h"
#include "AreaTriggerTemplate.h"
#include "Player.h"
#include "ScriptMgr.h"
#include "SpellAuraEffects.h"
#include "SpellInfo.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "ScriptMgr.h"

enum DemonHunterSpells
{
    SPELL_DH_SIGIL_OF_FLAME_EXPLOSION = 1410004,
};

// 204596 - Sigil of Flame
// MiscId - 6039
struct at_dh_sigil_of_flame : AreaTriggerAI
{
    at_dh_sigil_of_flame(AreaTrigger* areatrigger) : AreaTriggerAI(areatrigger) { }

    void OnRemove() override
    {
        Unit* caster = at->GetCaster();
        if (!caster)
            return;

        caster->CastSpell(at->GetPositionX(), at->GetPositionY(), at->GetPositionZ(), SPELL_DH_SIGIL_OF_FLAME_EXPLOSION, true);
    }
};

void AddSC_demonhunter_spell_scripts()
{
    RegisterAreaTriggerAI(at_dh_sigil_of_flame);
}
