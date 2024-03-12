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

#include "CreatureScript.h"
#include "Pet.h"
#include "Player.h"
#include "SpellAuraEffects.h"
#include "SpellMgr.h"
#include "SpellScript.h"
#include "SpellScriptLoader.h"
#include "TemporarySummon.h"
/*
 * Scripts for spells with SPELLFAMILY_MAGE and SPELLFAMILY_GENERIC spells used by mage players.
 * Ordered alphabetically using scriptname.
 * Scriptnames of files in this file should be prefixed with "spell_mage_".
 */

enum MageSpells
{
    SPELL_MAGE_BLAZING_SPEED                       = 31643,
    SPELL_MAGE_BURNOUT_TRIGGER                     = 44450,
    SPELL_MAGE_IMPROVED_BLIZZARD_CHILLED           = 12486,
    SPELL_MAGE_COMBUSTION                          = 11129,
    SPELL_MAGE_COLD_SNAP                           = 11958,
    SPELL_MAGE_FOCUS_MAGIC_PROC                    = 54648,
    SPELL_MAGE_FROST_WARDING_R1                    = 11189,
    SPELL_MAGE_FROST_WARDING_TRIGGERED             = 57776,
    SPELL_MAGE_INCANTERS_ABSORBTION                = 44396,
    SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED      = 44413,
    SPELL_MAGE_IGNITE                              = 12654,
    SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE         = 29077,
    SPELL_MAGE_SQUIRREL_FORM                       = 32813,
    SPELL_MAGE_GIRAFFE_FORM                        = 32816,
    SPELL_MAGE_SERPENT_FORM                        = 32817,
    SPELL_MAGE_DRAGONHAWK_FORM                     = 32818,
    SPELL_MAGE_WORGEN_FORM                         = 32819,
    SPELL_MAGE_SHEEP_FORM                          = 32820,
    SPELL_MAGE_GLYPH_OF_ETERNAL_WATER              = 70937,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT    = 70908,
    SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY    = 70907,
    SPELL_MAGE_GLYPH_OF_BLAST_WAVE                 = 62126,
    SPELL_MAGE_CHILLED                             = 12484,
    SPELL_MAGE_MANA_SURGE                          = 37445,
    SPELL_MAGE_MAGIC_ABSORPTION_MANA               = 29442,
    SPELL_MAGE_ARCANE_POTENCY_RANK_1               = 57529,
    SPELL_MAGE_ARCANE_POTENCY_RANK_2               = 57531,
    SPELL_MAGE_COMBUSTION_PROC                     = 28682,
    SPELL_MAGE_EMPOWERED_FIRE_PROC                 = 67545,
    SPELL_MAGE_T10_2P_BONUS                        = 70752,
    SPELL_MAGE_T10_2P_BONUS_EFFECT                 = 70753,
    SPELL_MAGE_T8_4P_BONUS                         = 64869,
    SPELL_MAGE_MISSILE_BARRAGE                     = 44401,
    SPELL_MAGE_FINGERS_OF_FROST_AURASTATE_AURA     = 44544,
    SPELL_MAGE_FINGERS_OF_FROST                    = 44543,
                                                   
    // Duskhaven                                   
    SPELL_MAGE_ARCANE_ASCENDANCE_AURA              = 1280031,
    SPELL_MAGE_ARCANE_ASCENDANCE_PROC              = 1280032,
    SPELL_MAGE_ARCANE_BARRAGE                      = 1310029,
    SPELL_MAGE_ARCANE_BARRAGE2                     = 1310077,
    SPELL_MAGE_ARCANE_BARRAGE_ACCELERATION         = 1310036,
    SPELL_MAGE_ARCANE_BARRAGE_SLOW                 = 1310035,
    SPELL_MAGE_ARCANE_EXPLOSION                    = 1280038,
    SPELL_MAGE_ARCANE_FEEDBACK                     = 36032,
    SPELL_MAGE_ARCANE_MISSILES                     = 1310006,
    SPELL_MAGE_ARCANE_SURGE                        = 1310037,
    SPELL_MAGE_ARCANIST_MIND_AURA                  = 1310020,
    SPELL_MAGE_ARCANIST_MIND_BUFF                  = 1310021,
    SPELL_MAGE_AVALANCHE_AURA                      = 1290030,
    SPELL_MAGE_AVALANCHE_COMET_PROC                = 1290031,
    SPELL_MAGE_BLAZING_BARRIER                     = 1280004,
    SPELL_MAGE_BLAZING_BARRIER_DAMAGE_AOE          = 1280008,
    SPELL_MAGE_BLINK                               = 1953,
    SPELL_MAGE_CHAIN_REACTION_AURA                 = 1290016,
    SPELL_MAGE_CHAIN_REACTION_PROC                 = 1290017,
    SPELL_MAGE_CHILLED_TO_THE_BONE                 = 1290046,
    SPELL_MAGE_CASCADING_POWER_BUFF                = 1310073,
    SPELL_MAGE_CLEARCASTING                        = 12536,
    SPELL_MAGE_COMET_STORM_COMET                   = 1290027,
    SPELL_MAGE_COMET_STORM_SPELL                   = 1290026,
    SPELL_MAGE_CONE_OF_COLD                        = 1290063,
    SPELL_MAGE_CONFLAGRATION_AURA                  = 1300034,
    SPELL_MAGE_CRYSTALLIZE_AURA                    = 1290048,
    SPELL_MAGE_CRYSTALLIZE_SPELL                   = 1290047,
    SPELL_MAGE_DEEP_FREEZE_PROC1                   = 1290040,
    SPELL_MAGE_DEEP_FREEZE_PROC2                   = 1290041,
    SPELL_MAGE_DIAMOND_DUST_AURA                   = 1290019,
    SPELL_MAGE_DISPLACEMENT_ALLOW_CAST_AURA        = 1280061,
    SPELL_MAGE_DISPLACEMENT_SUMMON                 = 1280055,
    SPELL_MAGE_DISPLACEMENT_TALENT_AURA            = 1280056,
    SPELL_MAGE_DISPLACEMENT_TELEPORT               = 1280054,
    SPELL_MAGE_DIVERTED_ENERGY_AURA                = 1280018,
    SPELL_MAGE_DIVERTED_ENERGY_PROC                = 1280019,
    SPELL_MAGE_DOUBLE_TIME_AURA                    = 1310067,
    SPELL_MAGE_DRAGONS_BREATH                      = 1300020,
    SPELL_MAGE_EBONBOLT                            = 1290021,
    SPELL_MAGE_ECHO_OF_ANTONIDAS_AURA              = 1310061,
    SPELL_MAGE_ECHO_OF_ANTONIDAS_TOUCH_OF_THE_MAGI = 1310062,
    SPELL_MAGE_EVOCATION                           = 1310011,
    SPELL_MAGE_FINGERS_OF_FROST_PROC               = 1290010,
    SPELL_MAGE_FIREBALL                            = 1300011,
    SPELL_MAGE_FIRE_BLAST                          = 1300003,
    SPELL_MAGE_FIRESTARTER_AURA                    = 1300021,
    SPELL_MAGE_FIRESTARTER_BUFF                    = 1300022,
    SPELL_MAGE_FLAMECANNON                         = 1300025,
    SPELL_MAGE_FLAME_CONVERGENCE_AURA              = 1310047,
    SPELL_MAGE_FLAME_CONVERGENCE_PROC              = 1310048,
    SPELL_MAGE_FLAMESTRIKE                         = 1300015,
    SPELL_MAGE_FLASH_FREEZE_AURA                   = 1290018,
    SPELL_MAGE_FOCUS_MAGIC_ALLY                    = 54646,
    SPELL_MAGE_FORCE_BARRIER_AURA                  = 1280012,
    SPELL_MAGE_FROST_BARRIER                       = 1280005,
    SPELL_MAGE_FROSTBOLT                           = 1290057,
    SPELL_MAGE_FROST_BOMB                          = 1290025,
    SPELL_MAGE_FROSTFIRE_BOLT                      = 1280057,
    SPELL_MAGE_GLACIAL_ASSAULT_AURA                = 1290052,
    SPELL_MAGE_GLACIAL_ASSAULT_GLACIAL_SPIKE       = 1290055,
    SPELL_MAGE_GLACIAL_ASSAULT_PROC                = 1290053,
    SPELL_MAGE_GLACIAL_SPIKE                       = 1290020,
    SPELL_MAGE_GREATER_PYROBLAST_AURA              = 1300036,
    SPELL_MAGE_GREATER_PYROBLAST_COUNTER           = 1300037,
    SPELL_MAGE_HOT_STREAK_AURA                     = 44448,
    SPELL_MAGE_HOT_STREAK_PROC                     = 48108,
    SPELL_MAGE_ICE_BLADES_AURA                     = 1290028,
    SPELL_MAGE_ICE_BLADES_ICE_LANCE                = 1290029,
    SPELL_MAGE_ICE_FORM_AURA                       = 1290050,
    SPELL_MAGE_ICE_FORM_TALENT                     = 1290049,
    SPELL_MAGE_ICE_LANCE                           = 1290008,
    SPELL_MAGE_ICICLE_AURA                         = 1290001,
    SPELL_MAGE_ICICLE_SPELL                        = 1290007,
    SPELL_MAGE_ICICLE_VISUAL1                      = 1290002,
    SPELL_MAGE_ICICLE_VISUAL2                      = 1290003,
    SPELL_MAGE_ICICLE_VISUAL3                      = 1290004,
    SPELL_MAGE_ICICLE_VISUAL4                      = 1290005,
    SPELL_MAGE_ICICLE_VISUAL5                      = 1290006,
    SPELL_MAGE_ICY_VEINS_AURA                      = 12472,
    SPELL_MAGE_ICY_VEINS_ICE_FORM                  = 1290051,
    SPELL_MAGE_IMPACT_ZONE_AURA                    = 1300026,
    SPELL_MAGE_IMPROVED_SCORCH_DEBUFF              = 22959,
    SPELL_MAGE_MASTER_OF_MAGIC_ARCANE_AURA         = 1280046,
    SPELL_MAGE_MASTER_OF_MAGIC_COUNTER             = 1280047,
    SPELL_MAGE_MASTER_OF_MAGIC_FIRE_AURA           = 1280045,
    SPELL_MAGE_MASTER_OF_MAGIC_FROST_AURA          = 1280044,
    SPELL_MAGE_MASTER_OF_MAGIC_ICD_AURA            = 1280049,
    SPELL_MAGE_MASTER_OF_MAGIC_PROC_AURA           = 1280048,
    SPELL_MAGE_MASTERY_ARCANE_MASTERY              = 1310000,
    SPELL_MAGE_MASTERY_FLASHBURN                   = 1300000,
    SPELL_MAGE_MASTERY_ICICLES                     = 1290000,
    SPELL_MAGE_METEOR_AURA                         = 1300023,
    SPELL_MAGE_METEOR_PROC                         = 1300024,
    SPELL_MAGE_MIRROR_IMAGE_AURA                   = 55342,
    SPELL_MAGE_MIRROR_IMAGE_BUFF_AURA              = 1280030,
    SPELL_MAGE_MISSILE_BARRAGE_AURA                = 54490,
    SPELL_MAGE_MISSILE_BARRAGE_BUFF                = 44401,
    SPELL_MAGE_MISSILE_BARRAGE_NOSTACK             = 1310018,
    SPELL_MAGE_MISSILE_BARRAGE_STACK               = 1310019,
    SPELL_MAGE_MISSILE_BARRAGE_TRIGGER             = 1310017,
    SPELL_MAGE_PARTICLE_DISINTEGRATION_DEBUFF      = 1310075,
    SPELL_MAGE_PERMAFROST_PROC                     = 1290033,
    SPELL_MAGE_POWER_OF_THE_MAD_PRINCE_AURA        = 1310065,
    SPELL_MAGE_POWER_OF_THE_MAD_PRINCE_BUFF        = 1310066,
    SPELL_MAGE_PRISMATIC_BARRIER                   = 1280003,
    SPELL_MAGE_PRISMATIC_BARRIER_MANA_REGEN        = 1280006,
    SPELL_MAGE_PYROBLAST                           = 1300009,
    SPELL_MAGE_RAPID_DECOMPOSITION_AURA            = 1310060,
    SPELL_MAGE_RAY_OF_FROST_BUFF                   = 1290045,
    SPELL_MAGE_RAY_OF_FROST_MAIN                   = 1290042,
    SPELL_MAGE_RESONANCE_AURA_R1                   = 1310033,
    SPELL_MAGE_RESONANCE_AURA_R2                   = 1310034,
    SPELL_MAGE_RESONANT_SPARK                      = 1310030,
    SPELL_MAGE_RESONANT_SPARK_DEBUFF_AURA          = 1310031,
    SPELL_MAGE_RESONANT_SPARK_PROC_AURA            = 1310032,
    SPELL_MAGE_RHONINS_RETORT_AURA                 = 1310045,
    SPELL_MAGE_RHONINS_RETORT_TOUCH_OF_THE_MAGI    = 1310046,
    SPELL_MAGE_RING_OF_FROST_DUMMY                 = 1280062,
    SPELL_MAGE_RING_OF_FROST_FREEZE                = 1280052,
    SPELL_MAGE_RING_OF_FROST_SLOW                  = 1280053,
    SPELL_MAGE_RING_OF_FROST_SUMMON                = 1280050,
    SPELL_MAGE_RING_OF_FROST_TICK                  = 1280051,
    SPELL_MAGE_RISK_OF_RUNEWEAVER_AURA             = 1310063,
    SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC             = 1310064,
    SPELL_MAGE_RULE_OF_THREES_AURA                 = 1310026,
    SPELL_MAGE_RULE_OF_THREES_AURA_TRIGGER         = 1310027,
    SPELL_MAGE_SEAR                                = 1300019,
    SPELL_MAGE_SHARED_POWER_PROC                   = 1310043,
    SPELL_MAGE_SHIMMER                             = 1280000,
    SPELL_MAGE_SOUL_OF_SHAZZRATHI_AURA             = 1300040,
    SPELL_MAGE_SOUL_OF_SHAZZRATHI_BUFF             = 1300042,
    SPELL_MAGE_SOUL_OF_SHAZZRATHI_COMBUSTION       = 1300043,
    SPELL_MAGE_SOUL_OF_SHAZZRATHI_COUNTER          = 1300041,
    SPELL_MAGE_SPLITTING_ICE_AURA                  = 1290013,
    SPELL_MAGE_SUNDERING_FLAME_AURA                = 1300030,
    SPELL_MAGE_SUNDERING_FLAME_DEBUFF              = 1300031,
    SPELL_MAGE_SUPERCONDUCTOR_AURA                 = 1310069,
    SPELL_MAGE_SUPERCONDUCTOR_BUFF                 = 1310070,
    SPELL_MAGE_SUPERNOVA                           = 1310053,
    SPELL_MAGE_TEMPEST_BARRIER_AURA                = 1280013,
    SPELL_MAGE_TEMPEST_BARRIER_PROC                = 1280014,
    SPELL_MAGE_TEMPORAL_DISPLACEMENT               = 1280002,
    SPELL_MAGE_TEMPORAL_WARP_AURA                  = 1280039,
    SPELL_MAGE_THERMAL_VOID_AURA                   = 1290023,
    SPELL_MAGE_TIME_WARD_AURA                      = 1280040,
    SPELL_MAGE_TIME_WARD_PROC                      = 1280041,
    SPELL_MAGE_TIME_WARD_PROC_FINISH               = 1280042,
    SPELL_MAGE_TIME_WARP                           = 1280001,
    SPELL_MAGE_TOUCH_OF_THE_MAGI_AURA              = 1310015,
    SPELL_MAGE_TOUCH_OF_THE_MAGI_EXPLOSION         = 1310016
};

enum SpellBunnies
{
    NPC_MAGE_COMET_STORM_TARGET                    = 1291000,
    NPC_MAGE_MIRROR_IMAGE                          = 31216
};

enum SpellHelpers
{
    SPELL_SHAMAN_EXHAUSTION                        = 57723,
    SPELL_SHAMAN_SATED                             = 57724
};

// -31641 - Blazing Speed
class spell_mage_blazing_speed : public AuraScript
{
    PrepareAuraScript(spell_mage_blazing_speed);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BLAZING_SPEED });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        if (Unit* target = eventInfo.GetActionTarget())
        {
            target->CastSpell(target, SPELL_MAGE_BLAZING_SPEED, true, nullptr, aurEff);
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_blazing_speed::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

class spell_mage_arcane_blast : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_blast);

    bool Load() override { _triggerSpellId = 0; return true; }

    void HandleTriggerSpell(SpellEffIndex effIndex)
    {
        _triggerSpellId = GetSpellInfo()->Effects[effIndex].TriggerSpell;
        PreventHitDefaultEffect(effIndex);
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        caster->CastSpell(caster, _triggerSpellId, TRIGGERED_FULL_MASK);
        if (caster->HasAura(SPELL_MAGE_RULE_OF_THREES_AURA))
            caster->CastSpell(caster, SPELL_MAGE_RULE_OF_THREES_AURA_TRIGGER, TRIGGERED_FULL_MASK);
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (caster->HasAura(SPELL_MAGE_MISSILE_BARRAGE_AURA))
        {
            uint16 chance = sSpellMgr->GetSpellInfo(SPELL_MAGE_MISSILE_BARRAGE_AURA)->Effects[EFFECT_0].CalcValue();
            if (irand(1, 100) <= chance)
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_TRIGGER, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_arcane_blast::HandleOnHit);
        OnEffectLaunch += SpellEffectFn(spell_mage_arcane_blast::HandleTriggerSpell, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        OnEffectLaunchTarget += SpellEffectFn(spell_mage_arcane_blast::HandleTriggerSpell, EFFECT_1, SPELL_EFFECT_TRIGGER_SPELL);
        AfterCast += SpellCastFn(spell_mage_arcane_blast::HandleAfterCast);
    }

private:
    uint32 _triggerSpellId;
};

class spell_mage_burning_determination : public AuraScript
{
    PrepareAuraScript(spell_mage_burning_determination);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetSpellInfo() || !eventInfo.GetActionTarget())
            return false;

        // Need Interrupt or Silenced mechanic
        if (!(eventInfo.GetSpellInfo()->GetAllEffectsMechanicMask() & ((1 << MECHANIC_INTERRUPT) | (1 << MECHANIC_SILENCE))))
            return false;

        // Xinef: immuned effect should just eat charge
        if (eventInfo.GetHitMask() & PROC_HIT_IMMUNE)
        {
            eventInfo.GetActionTarget()->RemoveAurasDueToSpell(54748);
            return false;
        }
        if (Aura* aura = eventInfo.GetActionTarget()->GetAura(54748))
        {
            if (aura->GetDuration() < aura->GetMaxDuration())
                eventInfo.GetActionTarget()->RemoveAurasDueToSpell(54748);
            return false;
        }

        return true;
    }

    void HandleProc(AuraEffect const*  aurEff, ProcEventInfo&  /*eventInfo*/)
    {
        PreventDefaultAction();
        GetUnitOwner()->CastSpell(GetUnitOwner(), 54748, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_burning_determination::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_burning_determination::HandleProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }
};

class spell_mage_molten_armor : public AuraScript
{
    PrepareAuraScript(spell_mage_molten_armor);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo || (eventInfo.GetTypeMask() & PROC_FLAG_TAKEN_MELEE_AUTO_ATTACK))
            return true;

        if (!eventInfo.GetActionTarget())
        {
            return false;
        }

        // Xinef: Molten Shields talent
        if (AuraEffect* aurEff = eventInfo.GetActionTarget()->GetAuraEffect(SPELL_AURA_ADD_FLAT_MODIFIER, SPELLFAMILY_MAGE, 16, EFFECT_0))
            return roll_chance_i(aurEff->GetSpellInfo()->GetRank() * 50);

        return false;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_molten_armor::CheckProc);
    }
};

class spell_mage_mirror_image : public AuraScript
{
    PrepareAuraScript(spell_mage_mirror_image)

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->CastSpell((Unit*)nullptr, GetSpellInfo()->Effects[aurEff->GetEffIndex()].TriggerSpell, true);
    }

    void CalcPeriodic(AuraEffect const* /*effect*/, bool& isPeriodic, int32&  /*amplitude*/)
    {
        isPeriodic = false;
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_mirror_image::HandleEffectApply, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_mage_mirror_image::CalcPeriodic, EFFECT_2, SPELL_AURA_PERIODIC_DUMMY);
    }
};

class spell_mage_burnout : public AuraScript
{
    PrepareAuraScript(spell_mage_burnout);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_BURNOUT_TRIGGER });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        return eventInfo.GetSpellInfo() != nullptr;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        int32 mana = int32(eventInfo.GetSpellInfo()->CalcPowerCost(GetTarget(), eventInfo.GetSchoolMask()));
        mana = CalculatePct(mana, aurEff->GetAmount());

        GetTarget()->CastCustomSpell(SPELL_MAGE_BURNOUT_TRIGGER, SPELLVALUE_BASE_POINT0, mana, GetTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_burnout::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_burnout::HandleProc, EFFECT_1, SPELL_AURA_DUMMY);
    }
};

class spell_mage_burnout_trigger : public SpellScript
{
    PrepareSpellScript(spell_mage_burnout_trigger);

    void HandleDummy(SpellEffIndex effIndex)
    {
        PreventHitDefaultEffect(effIndex);
        if (Unit* target = GetHitUnit())
        {
            int32 newDamage = -(target->ModifyPower(POWER_MANA, -GetEffectValue()));
            GetSpell()->ExecuteLogEffectTakeTargetPower(effIndex, target, POWER_MANA, newDamage, 0.0f);
        }
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_burnout_trigger::HandleDummy, EFFECT_0, SPELL_EFFECT_POWER_BURN);
    }
};

class spell_mage_pet_scaling : public AuraScript
{
    PrepareAuraScript(spell_mage_pet_scaling);

    void CalculateResistanceAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 40% of resistance from owner and 35% of armor (guessed)
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            SpellSchoolMask schoolMask = SpellSchoolMask(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            int32 modifier = schoolMask == SPELL_SCHOOL_MASK_NORMAL ? 35 : 40;
            amount = CalculatePct(std::max<int32>(0, owner->GetResistance(schoolMask)), modifier);
        }
    }

    void CalculateStatAmount(AuraEffect const* aurEff, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 30% of intellect / stamina
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            Stats stat = Stats(aurEff->GetSpellInfo()->Effects[aurEff->GetEffIndex()].MiscValue);
            amount = CalculatePct(std::max<int32>(0, owner->GetStat(stat)), 30);
        }
    }

    void CalculateAPAmount(AuraEffect const*  /*aurEff*/, int32&   /*amount*/, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 0% AP
    }

    void CalculateSPAmount(AuraEffect const*  /*aurEff*/, int32& amount, bool& /*canBeRecalculated*/)
    {
        // xinef: mage pet inherits 33% of SP
        if (Unit* owner = GetUnitOwner()->GetOwner())
        {
            int32 frost = owner->SpellBaseDamageBonusDone(SPELL_SCHOOL_MASK_FROST);
            amount = CalculatePct(std::max<int32>(0, frost), 33);

            // xinef: Update appropriate player field
            if (owner->GetTypeId() == TYPEID_PLAYER)
                owner->SetUInt32Value(PLAYER_PET_SPELL_POWER, (uint32)amount);
        }
    }

    void HandleEffectApply(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        if (GetUnitOwner()->IsPet())
            return;

        GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, aurEff->GetAuraType(), true, SPELL_BLOCK_TYPE_POSITIVE);
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_ATTACK_POWER)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_ATTACK_POWER_PCT, true, SPELL_BLOCK_TYPE_POSITIVE);
        else if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT)
            GetUnitOwner()->ApplySpellImmune(0, IMMUNITY_STATE, SPELL_AURA_MOD_TOTAL_STAT_PERCENTAGE, true, SPELL_BLOCK_TYPE_POSITIVE);
    }

    void CalcPeriodic(AuraEffect const* /*aurEff*/, bool& isPeriodic, int32& amplitude)
    {
        if (!GetUnitOwner()->IsPet())
            return;

        isPeriodic = true;
        amplitude = 2 * IN_MILLISECONDS;
    }

    void HandlePeriodic(AuraEffect const* aurEff)
    {
        PreventDefaultAction();
        if (aurEff->GetAuraType() == SPELL_AURA_MOD_STAT && (aurEff->GetMiscValue() == STAT_STAMINA || aurEff->GetMiscValue() == STAT_INTELLECT))
        {
            int32 currentAmount = aurEff->GetAmount();
            int32 newAmount = GetEffect(aurEff->GetEffIndex())->CalculateAmount(GetCaster());
            if (newAmount != currentAmount)
            {
                if (aurEff->GetMiscValue() == STAT_STAMINA)
                {
                    uint32 actStat = GetUnitOwner()->GetHealth();
                    GetEffect(aurEff->GetEffIndex())->ChangeAmount(newAmount, false);
                    GetUnitOwner()->SetHealth(std::min<uint32>(GetUnitOwner()->GetMaxHealth(), actStat));
                }
                else
                {
                    uint32 actStat = GetUnitOwner()->GetPower(POWER_MANA);
                    GetEffect(aurEff->GetEffIndex())->ChangeAmount(newAmount, false);
                    GetUnitOwner()->SetPower(POWER_MANA, std::min<uint32>(GetUnitOwner()->GetMaxPower(POWER_MANA), actStat));
                }
            }
        }
        else
            GetEffect(aurEff->GetEffIndex())->RecalculateAmount();
    }

    void Register() override
    {
        if (m_scriptSpellId != 35657)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateResistanceAmount, EFFECT_ALL, SPELL_AURA_MOD_RESISTANCE);

        if (m_scriptSpellId == 35657 || m_scriptSpellId == 35658)
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateStatAmount, EFFECT_ALL, SPELL_AURA_MOD_STAT);

        if (m_scriptSpellId == 35657)
        {
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateAPAmount, EFFECT_ALL, SPELL_AURA_MOD_ATTACK_POWER);
            DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_pet_scaling::CalculateSPAmount, EFFECT_ALL, SPELL_AURA_MOD_DAMAGE_DONE);
        }

        OnEffectApply += AuraEffectApplyFn(spell_mage_pet_scaling::HandleEffectApply, EFFECT_ALL, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
        DoEffectCalcPeriodic += AuraEffectCalcPeriodicFn(spell_mage_pet_scaling::CalcPeriodic, EFFECT_ALL, SPELL_AURA_ANY);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_pet_scaling::HandlePeriodic, EFFECT_ALL, SPELL_AURA_ANY);
    }
};

class spell_mage_brain_freeze : public AuraScript
{
    PrepareAuraScript(spell_mage_brain_freeze);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        // xinef: Improved Blizzard, generic chilled check
        if (spellInfo->SpellFamilyFlags[0] & 0x100000)
            return spellInfo->Id == SPELL_MAGE_IMPROVED_BLIZZARD_CHILLED;

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_brain_freeze::CheckProc);
    }
};

class spell_mage_glyph_of_eternal_water : public AuraScript
{
    PrepareAuraScript(spell_mage_glyph_of_eternal_water);

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Player* player = target->ToPlayer())
                if (Pet* pet = player->GetPet())
                    if (pet->GetEntry() == NPC_WATER_ELEMENTAL_PERM)
                        pet->Remove(PET_SAVE_NOT_IN_SLOT);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_glyph_of_eternal_water::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

    class spell_mage_combustion_proc : public AuraScript
    {
        PrepareAuraScript(spell_mage_combustion_proc);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMBUSTION });
    }

        void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
        {
            GetTarget()->RemoveAurasDueToSpell(SPELL_MAGE_COMBUSTION);
        }

        void Register() override
        {
            AfterEffectRemove += AuraEffectRemoveFn(spell_mage_combustion_proc::OnRemove, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
        }
    };

// Incanter's Absorbtion
class spell_mage_incanters_absorbtion_base_AuraScript : public AuraScript
{
public:
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, SPELL_MAGE_INCANTERS_ABSORBTION });
    }

    void Trigger(AuraEffect* aurEff, DamageInfo& /*dmgInfo*/, uint32& absorbAmount)
    {
        Unit* target = GetTarget();

        if (AuraEffect* talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_MAGE_INCANTERS_ABSORBTION, EFFECT_0))
        {
            int32 bp = CalculatePct(absorbAmount, talentAurEff->GetAmount());
            if (AuraEffect* currentAura = target->GetAuraEffect(SPELL_AURA_MOD_DAMAGE_DONE, SPELLFAMILY_MAGE, 2941, EFFECT_0))
            {
                bp += int32(currentAura->GetAmount() * (currentAura->GetBase()->GetDuration() / (float)currentAura->GetBase()->GetMaxDuration()));
                currentAura->ChangeAmount(bp);
                currentAura->GetBase()->RefreshDuration();
            }
            else
                target->CastCustomSpell(target, SPELL_MAGE_INCANTERS_ABSORBTION_TRIGGERED, &bp, nullptr, nullptr, true, nullptr, aurEff);
        }
    }
};

// -11113 - Blast Wave
class spell_mage_blast_wave : public SpellScript
{
    PrepareSpellScript(spell_mage_blast_wave);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_GLYPH_OF_BLAST_WAVE });
    }

    void HandleKnockBack(SpellEffIndex effIndex)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_GLYPH_OF_BLAST_WAVE))
            PreventHitDefaultEffect(effIndex);
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (caster->HasAura(SPELL_MAGE_FIRESTARTER_AURA))
        {
            caster->ToPlayer()->RemoveSpellCooldown(SPELL_MAGE_FLAMESTRIKE, false);

            if (!caster->HasAura(SPELL_MAGE_FIRESTARTER_BUFF))
                caster->CastSpell(caster, SPELL_MAGE_FIRESTARTER_BUFF, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_blast_wave::HandleOnHit);
        OnEffectHitTarget += SpellEffectFn(spell_mage_blast_wave::HandleKnockBack, EFFECT_2, SPELL_EFFECT_KNOCK_BACK);
    }
};

// 11958 - Cold Snap
class spell_mage_cold_snap : public SpellScript
{
    PrepareSpellScript(spell_mage_cold_snap);

    bool Load() override
    {
        return GetCaster()->GetTypeId() == TYPEID_PLAYER;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Player* caster = GetCaster()->ToPlayer();
        // immediately finishes the cooldown on Frost spells

        PlayerSpellMap const& spellMap = caster->GetSpellMap();
        for (PlayerSpellMap::const_iterator itr = spellMap.begin(); itr != spellMap.end(); ++itr)
        {
            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(itr->first);
            if (spellInfo->SpellFamilyName == SPELLFAMILY_MAGE && (spellInfo->GetSchoolMask() & SPELL_SCHOOL_MASK_FROST) && ((spellInfo->SpellFamilyFlags[0] & 0x240) || (spellInfo->SpellFamilyFlags[2] & 0x400040)) && spellInfo->GetRecoveryTime() > 0)
            {
                auto citr = caster->GetSpellCooldownMap().find(spellInfo->Id);
                if (citr != caster->GetSpellCooldownMap().end() && citr->second.needSendToClient)
                    caster->RemoveSpellCooldown(spellInfo->Id, true);
                else
                    caster->RemoveSpellCooldown(spellInfo->Id, false);
            }
        }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_cold_snap::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -543  - Fire Ward
// -6143 - Frost Ward
class spell_mage_fire_frost_ward : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_fire_frost_ward);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FROST_WARDING_TRIGGERED, SPELL_MAGE_FROST_WARDING_R1 });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.68% from sp bonus
            float bonus = 0.8068f;

            bonus *= caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
            bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void Absorb(AuraEffect* aurEff, DamageInfo& dmgInfo, uint32& absorbAmount)
    {
        Unit* target = GetTarget();
        if (AuraEffect* talentAurEff = target->GetAuraEffectOfRankedSpell(SPELL_MAGE_FROST_WARDING_R1, EFFECT_0))
        {
            int32 chance = talentAurEff->GetSpellInfo()->Effects[EFFECT_1].CalcValue(); // SPELL_EFFECT_DUMMY with NO_TARGET

            if (roll_chance_i(chance))
            {
                int32 bp = dmgInfo.GetDamage();
                target->CastCustomSpell(target, SPELL_MAGE_FROST_WARDING_TRIGGERED, &bp, nullptr, nullptr, true, nullptr, aurEff);
                absorbAmount = 0;

                // Xinef: trigger Incanters Absorbtion
                uint32 damage = dmgInfo.GetDamage();
                Trigger(aurEff, dmgInfo, damage);

                // Xinef: hack for chaos bolt
                if (!dmgInfo.GetSpellInfo() || dmgInfo.GetSpellInfo()->SpellIconID != 3178)
                    dmgInfo.AbsorbDamage(bp);

                PreventDefaultAction();
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_fire_frost_ward::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        //OnEffectAbsorb += AuraEffectAbsorbFn(spell_mage_fire_frost_ward::Absorb, EFFECT_0);
        //AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_fire_frost_ward::Trigger, EFFECT_0);
    }
};

// 54646 - Focus Magic
class spell_mage_focus_magic : public AuraScript
{
    PrepareAuraScript(spell_mage_focus_magic);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FOCUS_MAGIC_PROC });
    }

    bool Load() override
    {
        _procTarget = nullptr;
        return true;
    }

    bool CheckProc(ProcEventInfo& /*eventInfo*/)
    {
        _procTarget = GetCaster();
        return _procTarget && _procTarget->IsAlive();
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();
        GetTarget()->CastSpell(_procTarget, SPELL_MAGE_FOCUS_MAGIC_PROC, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_focus_magic::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_focus_magic::HandleProc, EFFECT_0, SPELL_AURA_MOD_SPELL_CRIT_CHANCE);
    }

private:
    Unit* _procTarget;
};

// -11426 - Ice Barrier
class spell_mage_ice_barrier_aura : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_ice_barrier_aura);

    /// @todo: Rework
    static int32 CalculateSpellAmount(Unit* caster, int32 amount, SpellInfo const* spellInfo, const AuraEffect* aurEff)
    {
        // +80.68% from sp bonus
        float bonus = 0.8068f;

        bonus *= caster->SpellBaseDamageBonusDone(spellInfo->GetSchoolMask());

        // Glyph of Ice Barrier: its weird having a SPELLMOD_ALL_EFFECTS here but its blizzards doing :)
        // Glyph of Ice Barrier is only applied at the spell damage bonus because it was already applied to the base value in CalculateSpellDamage
        bonus = caster->ApplyEffectModifiers(spellInfo, aurEff->GetEffIndex(), bonus);

        bonus *= caster->CalculateLevelPenalty(spellInfo);

        amount += int32(bonus);
        return amount;
    }

    void CalculateAmount(AuraEffect const* aurEff, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
            amount = CalculateSpellAmount(caster, amount, GetSpellInfo(), aurEff);
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_ice_barrier_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_ice_barrier_aura::Trigger, EFFECT_0);
    }
};

class spell_mage_ice_barrier : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_barrier);

    /// @todo: Rework
    static int32 CalculateSpellAmount(Unit* caster, int32 amount, SpellInfo const* spellInfo, const AuraEffect* aurEff)
    {
        // +80.68% from sp bonus
        float bonus = 0.8068f;

        bonus *= caster->SpellBaseDamageBonusDone(spellInfo->GetSchoolMask());

        // Glyph of Ice Barrier: its weird having a SPELLMOD_ALL_EFFECTS here but its blizzards doing :)
        // Glyph of Ice Barrier is only applied at the spell damage bonus because it was already applied to the base value in CalculateSpellDamage
        bonus = caster->ApplyEffectModifiers(spellInfo, aurEff->GetEffIndex(), bonus);

        bonus *= caster->CalculateLevelPenalty(spellInfo);

        amount += int32(bonus);
        return amount;
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_SCHOOL_ABSORB, (SpellFamilyNames)GetSpellInfo()->SpellFamilyName, GetSpellInfo()->SpellIconID, EFFECT_0))
        {
            int32 newAmount = GetSpellInfo()->Effects[EFFECT_0].CalcValue(caster, nullptr, nullptr);
            newAmount = CalculateSpellAmount(caster, newAmount, GetSpellInfo(), aurEff);

            if (aurEff->GetAmount() > newAmount)
                return SPELL_FAILED_AURA_BOUNCED;
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_ice_barrier::CheckCast);
    }
};

// -11119 - Ignite
class spell_mage_ignite : public AuraScript
{
    PrepareAuraScript(spell_mage_ignite);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_IGNITE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (!eventInfo.GetActor() || !eventInfo.GetProcTarget())
            return false;

        DamageInfo* damageInfo = eventInfo.GetDamageInfo();

        if (!damageInfo || !damageInfo->GetSpellInfo())
        {
            return false;
        }

        // Molten Armor
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
        {
            if (spellInfo->SpellFamilyFlags[1] & 0x8)
            {
                return false;
            }
        }

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        SpellInfo const* igniteDot = sSpellMgr->AssertSpellInfo(SPELL_MAGE_IGNITE);

        int32 amount = int32(CalculatePct(eventInfo.GetDamageInfo()->GetDamage(), 40) / igniteDot->GetMaxTicks());

        if (eventInfo.GetProcTarget()->HasAura(SPELL_MAGE_IGNITE))
        {
            uint32 tickNumber = eventInfo.GetProcTarget()->GetAura(SPELL_MAGE_IGNITE)->GetEffect(EFFECT_0)->GetTickNumber();
            int32 currentAmount = eventInfo.GetProcTarget()->GetAura(SPELL_MAGE_IGNITE)->GetEffect(EFFECT_0)->GetAmount() / (igniteDot->GetMaxTicks() - tickNumber);
            amount += currentAmount;
        }

        // Xinef: implement ignite bug
        //eventInfo.GetProcTarget()->CastDelayedSpellWithPeriodicAmount(eventInfo.GetActor(), SPELL_MAGE_IGNITE, SPELL_AURA_PERIODIC_DAMAGE, amount);
        GetTarget()->CastCustomSpell(SPELL_MAGE_IGNITE, SPELLVALUE_BASE_POINT0, amount, eventInfo.GetProcTarget(), true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_ignite::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_ignite::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -44457 - Living Bomb
class spell_mage_living_bomb : public AuraScript
{
    PrepareAuraScript(spell_mage_living_bomb);

    bool Validate(SpellInfo const* spell) override
    {
        if (!sSpellMgr->GetSpellInfo(uint32(spell->Effects[EFFECT_1].CalcValue())))
            return false;
        return true;
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        AuraRemoveMode removeMode = GetTargetApplication()->GetRemoveMode();
        if (removeMode != AURA_REMOVE_BY_ENEMY_SPELL && removeMode != AURA_REMOVE_BY_EXPIRE)
            return;

        if (Unit* caster = GetCaster())
            caster->CastSpell(GetTarget(), uint32(aurEff->GetAmount()), true, nullptr, aurEff);

        if (GetCaster()->HasAura(SPELL_MAGE_CONFLAGRATION_AURA))
            GetCaster()->ToPlayer()->ModifySpellCooldown(SPELL_MAGE_FLAMECANNON, -4000);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_living_bomb::AfterRemove, EFFECT_1, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// -1463 - Mana Shield
class spell_mage_mana_shield : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_mana_shield);

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            // +80.53% from sp bonus
            float bonus = 0.8053f;

            bonus *= caster->SpellBaseDamageBonusDone(GetSpellInfo()->GetSchoolMask());
            bonus *= caster->CalculateLevelPenalty(GetSpellInfo());

            amount += int32(bonus);
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_mana_shield::CalculateAmount, EFFECT_0, SPELL_AURA_MANA_SHIELD);
        AfterEffectManaShield += AuraEffectManaShieldFn(spell_mage_mana_shield::Trigger, EFFECT_0);
    }
};

// -29074 - Master of Elements
class spell_mage_master_of_elements : public AuraScript
{
    PrepareAuraScript(spell_mage_master_of_elements);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        if (!damageInfo || !damageInfo->GetSpellInfo())
            return false;

        return true;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        if (!_spellInfo)
            return;

        if (Unit* target = GetTarget())
        {
            int32 mana = int32(_spellInfo->CalcPowerCost(target, eventInfo.GetSchoolMask()) / _ticksModifier);
            mana = CalculatePct(mana, aurEff->GetAmount());

            if (mana > 0)
            {
                target->CastCustomSpell(SPELL_MAGE_MASTER_OF_ELEMENTS_ENERGIZE, SPELLVALUE_BASE_POINT0, mana, target, true, nullptr, aurEff);
            }
        }
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_master_of_elements::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_master_of_elements::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }

private:
    SpellInfo const* _spellInfo = nullptr;
    uint8 _ticksModifier = 1;
};

enum SilvermoonPolymorph
{
    NPC_AUROSALIA   = 18744,
};

/// @todo move out of here and rename - not a mage spell
// 32826 - Polymorph (Visual)
class spell_mage_polymorph_cast_visual : public SpellScript
{
    PrepareSpellScript(spell_mage_polymorph_cast_visual);

    static const uint32 PolymorhForms[6];

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        // check if spell ids exist in dbc
        for (uint32 i = 0; i < 6; ++i)
            if (!sSpellMgr->GetSpellInfo(PolymorhForms[i]))
                return false;
        return true;
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        if (Unit* target = GetCaster()->FindNearestCreature(NPC_AUROSALIA, 30.0f))
            if (target->GetTypeId() == TYPEID_UNIT)
                target->CastSpell(target, PolymorhForms[urand(0, 5)], true);
    }

    void Register() override
    {
        OnEffectHitTarget += SpellEffectFn(spell_mage_polymorph_cast_visual::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

const uint32 spell_mage_polymorph_cast_visual::spell_mage_polymorph_cast_visual::PolymorhForms[6] =
{
    SPELL_MAGE_SQUIRREL_FORM,
    SPELL_MAGE_GIRAFFE_FORM,
    SPELL_MAGE_SERPENT_FORM,
    SPELL_MAGE_DRAGONHAWK_FORM,
    SPELL_MAGE_WORGEN_FORM,
    SPELL_MAGE_SHEEP_FORM
};

// 31687 - Summon Water Elemental
class spell_mage_summon_water_elemental : public SpellScript
{
    PrepareSpellScript(spell_mage_summon_water_elemental)
    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_GLYPH_OF_ETERNAL_WATER,
                SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY,
                SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT
            });
    }

    void HandleDummy(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();

        if (Creature* pet = ObjectAccessor::GetCreature(*caster, caster->GetPetGUID()))
            if (!pet->IsAlive())
                pet->ToTempSummon()->UnSummon();

        // Glyph of Eternal Water
        if (caster->HasAura(SPELL_MAGE_GLYPH_OF_ETERNAL_WATER))
            caster->CastSpell(caster, SPELL_MAGE_SUMMON_WATER_ELEMENTAL_PERMANENT, true);
        else
            caster->CastSpell(caster, SPELL_MAGE_SUMMON_WATER_ELEMENTAL_TEMPORARY, true);

        if (Creature* pet = ObjectAccessor::GetCreature(*caster, caster->GetPetGUID()))
            if (pet->GetCharmInfo() && caster->ToPlayer())
            {
                pet->m_CreatureSpellCooldowns.clear();
                SpellInfo const* spellInfo = sSpellMgr->GetSpellInfo(31707);
                pet->GetCharmInfo()->ToggleCreatureAutocast(spellInfo, true);
                pet->GetCharmInfo()->SetSpellAutocast(spellInfo, true);
                caster->ToPlayer()->CharmSpellInitialize();
            }
    }

    void Register() override
    {
        OnEffectHit += SpellEffectFn(spell_mage_summon_water_elemental::HandleDummy, EFFECT_0, SPELL_EFFECT_DUMMY);
    }
};

// -31571 - Arcane Potency
class spell_mage_arcane_potency : public AuraScript
{
    PrepareAuraScript(spell_mage_arcane_potency);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANE_POTENCY_RANK_1,
                SPELL_MAGE_ARCANE_POTENCY_RANK_2
            });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        static uint32 const triggerSpell[2] = { SPELL_MAGE_ARCANE_POTENCY_RANK_1, SPELL_MAGE_ARCANE_POTENCY_RANK_2 };

        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        uint32 spellId = triggerSpell[GetSpellInfo()->GetRank() - 1];
        caster->CastSpell(caster, spellId, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_arcane_potency::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 11129 - Combustion
class spell_mage_combustion : public AuraScript
{
    PrepareAuraScript(spell_mage_combustion);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_COMBUSTION_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        // Do not take charges, add a stack of crit buff
        if (!(eventInfo.GetHitMask() & PROC_HIT_CRITICAL))
        {
            eventInfo.GetActor()->CastSpell((Unit*)nullptr, SPELL_MAGE_COMBUSTION_PROC, true);
            return false;
        }

        return true;
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_combustion::CheckProc);
    }
};

// -11185 - Improved Blizzard
class spell_mage_imp_blizzard : public AuraScript
{
    PrepareAuraScript(spell_mage_imp_blizzard);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_CHILLED });
    }

    void HandleChill(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        uint32 triggerSpellId = sSpellMgr->GetSpellWithRank(SPELL_MAGE_CHILLED, GetSpellInfo()->GetRank());
        eventInfo.GetActor()->CastSpell(eventInfo.GetProcTarget(), triggerSpellId, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_imp_blizzard::HandleChill, EFFECT_0, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// 37447 - Improved Mana Gems
// 61062 - Improved Mana Gems
class spell_mage_imp_mana_gems : public AuraScript
{
    PrepareAuraScript(spell_mage_imp_mana_gems);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MANA_SURGE });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->CastSpell((Unit*)nullptr, SPELL_MAGE_MANA_SURGE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_imp_mana_gems::HandleProc, EFFECT_1, SPELL_AURA_OVERRIDE_CLASS_SCRIPTS);
    }
};

// -31656 - Empowered Fire
class spell_mage_empowered_fire : public AuraScript
{
    PrepareAuraScript(spell_mage_empowered_fire);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_EMPOWERED_FIRE_PROC });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (SpellInfo const* spellInfo = eventInfo.GetSpellInfo())
            if (spellInfo->Id == SPELL_MAGE_IGNITE)
                return true;

        return false;
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& /*eventInfo*/)
    {
        PreventDefaultAction();

        Unit* target = GetTarget();
        int32 bp0 = int32(CalculatePct(target->GetCreateMana(), aurEff->GetAmount()));
        target->CastCustomSpell(SPELL_MAGE_EMPOWERED_FIRE_PROC, SPELLVALUE_BASE_POINT0, bp0, target, true, nullptr, aurEff);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_empowered_fire::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_empowered_fire::HandleProc, EFFECT_0, SPELL_AURA_ADD_FLAT_MODIFIER);
    }
};

// 74396 - Fingers of Frost
class spell_mage_fingers_of_frost : public AuraScript
{
    PrepareAuraScript(spell_mage_fingers_of_frost);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_FINGERS_OF_FROST_AURASTATE_AURA });
    }

    void HandleDummy(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        eventInfo.GetActor()->RemoveAuraFromStack(GetId());
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        GetTarget()->RemoveAurasDueToSpell(SPELL_MAGE_FINGERS_OF_FROST_AURASTATE_AURA);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_fingers_of_frost::HandleDummy, EFFECT_0, SPELL_AURA_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_fingers_of_frost::OnRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 48108 - Hot Streak
// 57761 - Fireball!
class spell_mage_gen_extra_effects : public AuraScript
{
    PrepareAuraScript(spell_mage_gen_extra_effects);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                    SPELL_MAGE_T10_2P_BONUS,
                    SPELL_MAGE_T10_2P_BONUS_EFFECT,
                    SPELL_MAGE_T8_4P_BONUS
            });
    }

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetActor();
        // Prevent double proc for Arcane missiles
        if (caster == eventInfo.GetProcTarget())
            return false;

        // Proc chance is unknown, we'll just use dummy aura amount
        if (AuraEffect const* aurEff = caster->GetAuraEffect(SPELL_MAGE_T8_4P_BONUS, EFFECT_0))
            if (roll_chance_i(aurEff->GetAmount()))
                return false;

        return true;
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        Unit* caster = eventInfo.GetActor();

        if (caster->HasAura(SPELL_MAGE_T10_2P_BONUS))
            caster->CastSpell((Unit*)nullptr, SPELL_MAGE_T10_2P_BONUS_EFFECT, true);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_gen_extra_effects::CheckProc);
        OnProc += AuraProcFn(spell_mage_gen_extra_effects::HandleProc);
    }
};

// 56375 - Glyph of Polymorph
class spell_mage_glyph_of_polymorph : public AuraScript
{
    PrepareAuraScript(spell_mage_glyph_of_polymorph);

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* target = eventInfo.GetProcTarget();
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE, ObjectGuid::Empty, target->GetAura(32409)); // SW:D shall not be removed.
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_DAMAGE_PERCENT);
        target->RemoveAurasByType(SPELL_AURA_PERIODIC_LEECH);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_glyph_of_polymorph::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// 56374 - Glyph of Icy Veins
class spell_mage_glyph_of_icy_veins : public AuraScript
{
    PrepareAuraScript(spell_mage_glyph_of_icy_veins);

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActor();
        caster->RemoveAurasByType(SPELL_AURA_HASTE_SPELLS, ObjectGuid::Empty, 0, true, false);
        caster->RemoveAurasByType(SPELL_AURA_MOD_DECREASE_SPEED);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_glyph_of_icy_veins::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -44445 - Hot Streak
class spell_mage_hot_streak : public AuraScript
{
    PrepareAuraScript(spell_mage_hot_streak);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_HOT_STREAK_PROC });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        AuraEffect* counter = GetEffect(EFFECT_1);
        if (!counter)
            return;

        // Count spell criticals in a row in second aura
        if (eventInfo.GetHitMask() & PROC_HIT_CRITICAL)
        {
            counter->SetAmount(counter->GetAmount() * 2);
            if (counter->GetAmount() < 100) // not enough
                return;

            // roll chance
            if (!roll_chance_i(aurEff->GetAmount()))
                return;

            Unit* caster = eventInfo.GetActor();
            caster->CastSpell(caster, SPELL_MAGE_HOT_STREAK_PROC, true, nullptr, aurEff);
        }

        // reset counter
        counter->SetAmount(25);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_hot_streak::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -29441 - Magic Absorption
class spell_mage_magic_absorption : public AuraScript
{
    PrepareAuraScript(spell_mage_magic_absorption);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo({ SPELL_MAGE_MAGIC_ABSORPTION_MANA });
    }

    void HandleProc(AuraEffect const* aurEff, ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();
        Unit* caster = eventInfo.GetActionTarget();
        int32 bp = CalculatePct(static_cast<int32>(caster->GetMaxPower(POWER_MANA)), aurEff->GetAmount());
        caster->CastCustomSpell(SPELL_MAGE_MAGIC_ABSORPTION_MANA, SPELLVALUE_BASE_POINT0, bp, caster, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_magic_absorption::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
    }
};

// -44404 - Missile Barrage
class spell_mage_missile_barrage : public AuraScript
{
    PrepareAuraScript(spell_mage_missile_barrage);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        SpellInfo const* spellInfo = eventInfo.GetSpellInfo();
        if (!spellInfo)
            return false;

        // Arcane Blast - full chance
        if (spellInfo->SpellFamilyFlags[0] & 0x20000000)
            return true;

        // Rest of spells have half chance
        return roll_chance_i(50);
    }

    void Register() override
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_missile_barrage::CheckProc);
    }
};

#define FingersOfFrostScriptName "spell_mage_fingers_of_frost_proc_aura"
class spell_mage_fingers_of_frost_proc_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_fingers_of_frost_proc_aura);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellPhaseMask() != PROC_SPELL_PHASE_CAST)
        {
            eventInfo.SetProcChance(_chance);
        }

        return true;
    }

    void HandleOnEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_CAST)
        {
            _chance = 100.f;
            _spell = eventInfo.GetProcSpell();

            if (!_spell || _spell->GetDelayMoment() <= 0)
            {
                PreventDefaultAction();
            }
        }
        else
        {
            if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_FINISH || ((_spell && _spell->GetDelayMoment() > 0) || !eventInfo.GetDamageInfo()))
            {
                PreventDefaultAction();
            }

            _chance = 0.f;
            _spell = nullptr;
        }
    }

    void HandleAfterEffectProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_HIT)
        {
            _chance = 100.f;
        }
        else if (eventInfo.GetSpellPhaseMask() == PROC_SPELL_PHASE_FINISH)
        {
            _chance = 0.f;
            _spell = nullptr;
        }
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_fingers_of_frost_proc_aura::CheckProc);
        OnEffectProc += AuraEffectProcFn(spell_mage_fingers_of_frost_proc_aura::HandleOnEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
        AfterEffectProc += AuraEffectProcFn(spell_mage_fingers_of_frost_proc_aura::HandleAfterEffectProc, EFFECT_0, SPELL_AURA_PROC_TRIGGER_SPELL);
    }

public:
    Spell const* GetProcSpell() const { return _spell; }

private:
    float _chance = 0.f;
    Spell const* _spell = nullptr;
};

typedef spell_mage_fingers_of_frost_proc_aura spell_mage_fingers_of_frost_proc_aura_script;

class spell_mage_fingers_of_frost_proc : public AuraScript
{
    PrepareAuraScript(spell_mage_fingers_of_frost_proc);

    bool CheckProc(ProcEventInfo& eventInfo)
    {
        if (Aura* aura = GetCaster()->GetAuraOfRankedSpell(SPELL_MAGE_FINGERS_OF_FROST))
        {
            if (spell_mage_fingers_of_frost_proc_aura_script* script = dynamic_cast<spell_mage_fingers_of_frost_proc_aura_script*>(aura->GetScriptByName(FingersOfFrostScriptName)))
            {
                if (Spell const* fofProcSpell = script->GetProcSpell())
                {
                    if (fofProcSpell == eventInfo.GetProcSpell())
                    {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void Register()
    {
        DoCheckProc += AuraCheckProcFn(spell_mage_fingers_of_frost_proc::CheckProc);
    }
};

// Duskhaven
class SpellMageCastEvent : public BasicEvent
{
public:
    SpellMageCastEvent(Unit* caster, Unit* victim, uint32 spellId) : _caster(caster), _victim(victim), _spellId(spellId) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _caster->CastSpell(_victim, _spellId);
        return true;
    }

private:
    Unit* _caster;
    Unit* _victim;
    uint32 _spellId;
};

class SpellMageCometStormEvent : public BasicEvent
{
public:
    SpellMageCometStormEvent(Unit* caster, Unit* victim, uint32 spellId, Position const& dest) : _caster(caster), _victim(victim), _spellId(spellId), _dest(dest), _count(0) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        float x = _dest.GetPositionX() + frand(-3.0f, 3.0f);
        float y = _dest.GetPositionY() + frand(-3.0f, 3.0f);
        float z = _dest.GetPositionZ();

        Creature* pSpawn = _victim->SummonCreature(NPC_MAGE_COMET_STORM_TARGET, x, y, z, 0.0f, TEMPSUMMON_TIMED_DESPAWN, 1500);
        pSpawn->SetFaction(_victim->GetFaction());
        _caster->CastSpell(pSpawn, _spellId, true);
        _count++;

        if (_count >= 7)
            return true;

        _caster->m_Events.AddEvent(this, irand(100, 275));
        return false;
    }

private:
    Unit* _caster;
    Unit* _victim;
    uint32 _spellId;
    uint8 _count;
    Position _dest;
};

class SpellMageRemoveAuraChargeEvent : public BasicEvent
{
public:
    SpellMageRemoveAuraChargeEvent(Unit* target, uint32 spellId) : _target(target), _spellId(spellId) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _target->GetAura(_spellId)->DropCharge();
        return true;
    }

private:
    Unit* _target;
    uint32 _spellId;
};

class SpellMageRemoveEffectEvent : public BasicEvent
{
public:
    SpellMageRemoveEffectEvent(Unit* target, uint32 spellId) : _target(target), _spellId(spellId) {}

    bool Execute(uint64 /*time*/, uint32 /*diff*/) override
    {
        _target->RemoveAuraFromStack(_spellId);
        return true;
    }

private:
    Unit* _target;
    uint32 _spellId;
};

// 1310029, 1310077 - Arcane Barrage
class spell_mage_arcane_barrage : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_barrage);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANE_BARRAGE,
                SPELL_MAGE_ARCANE_BARRAGE2,
                SPELL_MAGE_ARCANE_BARRAGE_ACCELERATION,
                SPELL_MAGE_ARCANE_BARRAGE_SLOW,
                SPELL_MAGE_ARCANE_FEEDBACK,
                SPELL_MAGE_MISSILE_BARRAGE_AURA,
                SPELL_MAGE_MISSILE_BARRAGE_TRIGGER,
                SPELL_MAGE_RESONANCE_AURA_R1,
                SPELL_MAGE_RESONANCE_AURA_R2
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        if (GetCaster()->HasAura(SPELL_MAGE_RESONANCE_AURA_R1) || GetCaster()->HasAura(SPELL_MAGE_RESONANCE_AURA_R2))
        {
            if (targets.size() > 3)
                targets.resize(3);
            _chainTargetCount = targets.size();
        }
    }

    void HandleOnEffectHitTarget(SpellEffIndex /*effIndex*/)
    {
        Unit* caster = GetCaster();
        int32 damage = GetHitDamage();

        if (caster->HasAura(SPELL_MAGE_MISSILE_BARRAGE_AURA))
        {
            uint16 chance = sSpellMgr->GetSpellInfo(SPELL_MAGE_MISSILE_BARRAGE_AURA)->Effects[EFFECT_1].CalcValue();

            if (irand(1, 100) <= chance)
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_TRIGGER, true);
        }

        if (caster->HasAura(SPELL_MAGE_RESONANCE_AURA_R2))
        {
            caster->CastSpell(caster, SPELL_MAGE_ARCANE_BARRAGE_ACCELERATION, true);
            caster->CastSpell(GetHitUnit(), SPELL_MAGE_ARCANE_BARRAGE_SLOW, true);

            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_RESONANCE_AURA_R2)->Effects[EFFECT_1].CalcValue();
            damage += CalculatePct(damage, _chainTargetCount * bonusPct);
        }
        else if (caster->HasAura(SPELL_MAGE_RESONANCE_AURA_R1))
        {
            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_RESONANCE_AURA_R1)->Effects[EFFECT_1].CalcValue();
            damage += CalculatePct(damage, _chainTargetCount * bonusPct);
        }

        SetHitDamage(damage);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_arcane_barrage::FilterTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
        OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_barrage::HandleOnEffectHitTarget, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    uint32 _chainTargetCount = 0;
};

// 1310009 - Arcane Explosion
class spell_mage_arcane_explosion : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_explosion);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANE_EXPLOSION,
                SPELL_MAGE_RAPID_DECOMPOSITION_AURA,
                SPELL_MAGE_RULE_OF_THREES_AURA
            });
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        if (caster->HasAura(SPELL_MAGE_RULE_OF_THREES_AURA) && !caster->HasAura(SPELL_MAGE_RAPID_DECOMPOSITION_AURA))
            caster->CastSpell(caster, SPELL_MAGE_RULE_OF_THREES_AURA_TRIGGER, true);
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_arcane_explosion::HandleAfterCast);
    }
};

// 1310006 - Arcane Missiles
class spell_mage_arcane_missiles : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_missiles);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANE_MISSILES,
                SPELL_MAGE_MISSILE_BARRAGE_NOSTACK,
                SPELL_MAGE_MISSILE_BARRAGE_STACK,
                SPELL_MAGE_RISK_OF_RUNEWEAVER_AURA,
                SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC
            });
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();
        uint16 castTime = 2600;

        if (caster->HasAura(SPELL_MAGE_MISSILE_BARRAGE_NOSTACK))
        {
            if (caster->HasAura(SPELL_MAGE_RISK_OF_RUNEWEAVER_AURA))
                caster->m_Events.AddEvent(new SpellMageCastEvent(caster, caster, SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC), caster->m_Events.CalculateTime(castTime));

            caster->m_Events.AddEvent(new SpellMageRemoveEffectEvent(caster, SPELL_MAGE_MISSILE_BARRAGE_NOSTACK), caster->m_Events.CalculateTime(castTime));
        }

        if (caster->HasAura(SPELL_MAGE_MISSILE_BARRAGE_STACK))
        {
            if (caster->HasAura(SPELL_MAGE_RISK_OF_RUNEWEAVER_AURA))
                caster->m_Events.AddEvent(new SpellMageCastEvent(caster, caster, SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC), caster->m_Events.CalculateTime(castTime));

            caster->m_Events.AddEvent(new SpellMageRemoveEffectEvent(caster, SPELL_MAGE_MISSILE_BARRAGE_STACK), caster->m_Events.CalculateTime(castTime));
        }
    }

    void Register() override
    {
        AfterCast += SpellCastFn(spell_mage_arcane_missiles::HandleAfterCast);
    }
};

// 1310037 - Arcane Surge
class spell_mage_arcane_surge : public SpellScript
{
    PrepareSpellScript(spell_mage_arcane_surge);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANE_SURGE
            });
    }

    void HandleBeforeCast()
    {
        Unit* caster = GetCaster();

        if (caster)
            manaPct = caster->GetPowerPct(POWER_MANA) / 100;
    }

    void HandleSetDamage(SpellEffIndex /*effIndex*/)
    {
        int32 damage = int32(GetHitDamage() * manaPct);
        SetHitDamage(damage);
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_mage_arcane_surge::HandleBeforeCast);
        OnEffectHitTarget += SpellEffectFn(spell_mage_arcane_surge::HandleSetDamage, EFFECT_0, SPELL_EFFECT_SCHOOL_DAMAGE);
    }

private:
    float manaPct;
};

// 1290031 - Comet
class spell_mage_avalanche_comet : public SpellScript
{
    PrepareSpellScript(spell_mage_avalanche_comet);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_AVALANCHE_AURA,
                SPELL_MAGE_AVALANCHE_COMET_PROC,
                SPELL_MAGE_FINGERS_OF_FROST_PROC
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_AVALANCHE_AURA))
            caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_PROC, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_avalanche_comet::HandleOnHit);
    }
};

// 1280003, 1280004, 1280005, 1280014 - Barriers On Proc script
class spell_mage_barriers_onproc_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_barriers_onproc_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_BLAZING_BARRIER,
                SPELL_MAGE_FROST_BARRIER,
                SPELL_MAGE_PRISMATIC_BARRIER,
                SPELL_MAGE_TEMPEST_BARRIER_PROC,
                SPELL_MAGE_DIVERTED_ENERGY_AURA,
                SPELL_MAGE_DIVERTED_ENERGY_PROC
            });
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();

        if (!damageInfo || !damageInfo->GetDamage())
            return;

        if (damageInfo->GetDamageType() != DIRECT_DAMAGE || damageInfo->GetAttackType() != BASE_ATTACK || damageInfo->GetAttackType() != OFF_ATTACK)
            PreventDefaultAction();

        int32 absorb = damageInfo->GetAbsorb();
        int32 pct = sSpellMgr->GetSpellInfo(SPELL_MAGE_DIVERTED_ENERGY_AURA)->Effects[EFFECT_0].CalcValue();
        int32 heal = int32(CalculatePct(absorb, pct));

        Unit* caster = GetCaster();

        if (caster && heal)
            caster->CastCustomSpell(SPELL_MAGE_DIVERTED_ENERGY_PROC, SPELLVALUE_BASE_POINT0, heal, caster, true);
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_mage_barriers_onproc_aura::HandleProc);
    }
};

// 1280004 - Blazing Barrier
class spell_mage_blazing_barrier_aura : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_blazing_barrier_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_BLAZING_BARRIER
            });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            int32 casterHp = caster->GetMaxHealth();
            int32 spellPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_BLAZING_BARRIER)->Effects[EFFECT_2].CalcValue();
            amount = int32(CalculatePct(casterHp, spellPct));
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_blazing_barrier_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_blazing_barrier_aura::Trigger, EFFECT_0);
    }
};

class spell_mage_blazing_barrier : public SpellScript
{
    PrepareSpellScript(spell_mage_blazing_barrier);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_BLAZING_BARRIER
            });
    }

    static int32 CalculateAmount(Unit* caster)
    {
        if (caster)
        {
            int32 casterHp = caster->GetMaxHealth();
            int32 spellPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_BLAZING_BARRIER)->Effects[EFFECT_2].CalcValue();
            int32 amount = int32(CalculatePct(casterHp, spellPct));

            return amount;
        }

        return 0;
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (caster)
        {
            if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_SCHOOL_ABSORB, (SpellFamilyNames)GetSpellInfo()->SpellFamilyName, GetSpellInfo()->SpellIconID, EFFECT_0))
            {
                int32 newAmount = CalculateAmount(caster);

                if (aurEff->GetAmount() > newAmount)
                    return SPELL_FAILED_AURA_BOUNCED;
            }
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_blazing_barrier::CheckCast);
    }
};

class spell_mage_blazing_barrier_onremove_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_blazing_barrier_onremove_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_BLAZING_BARRIER,
                SPELL_MAGE_BLAZING_BARRIER_DAMAGE_AOE
            });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();

        if (caster && aurEff->GetAmount() <= 0)
            caster->CastSpell(caster, SPELL_MAGE_BLAZING_BARRIER_DAMAGE_AOE, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_blazing_barrier_onremove_aura::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1953 - Blink
class spell_mage_blink : public SpellScript
{
    PrepareSpellScript(spell_mage_blink);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_BLINK,
                SPELL_MAGE_DISPLACEMENT_SUMMON,
                SPELL_MAGE_DISPLACEMENT_TALENT_AURA,
                SPELL_MAGE_TEMPEST_BARRIER_AURA,
                SPELL_MAGE_TEMPEST_BARRIER_PROC
            });
    }

    void HandleOnCast()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_DISPLACEMENT_TALENT_AURA))
            caster->CastSpell(caster, SPELL_MAGE_DISPLACEMENT_SUMMON, true);
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_TEMPEST_BARRIER_AURA))
            caster->CastSpell(caster, SPELL_MAGE_TEMPEST_BARRIER_PROC, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_blink::HandleOnCast);
        AfterCast += SpellCastFn(spell_mage_blink::HandleAfterCast);
    }
};

// 1310073 - Cascading Power
class spell_mage_cascading_power_aura : public SpellScript
{
    PrepareSpellScript(spell_mage_cascading_power_aura);

    void HandleBeforeHit(SpellMissInfo missInfo)
    {
        caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_CASCADING_POWER_BUFF))
            duration = caster->GetAura(SPELL_MAGE_CASCADING_POWER_BUFF)->GetDuration();
    }

    void HandleAfterHit()
    {
        if (caster && caster->HasAura(SPELL_MAGE_CASCADING_POWER_BUFF))
        {
            Aura* aura = caster->GetAura(SPELL_MAGE_CASCADING_POWER_BUFF);

            if (aura->GetStackAmount() > 1)
                aura->SetDuration(duration);
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_mage_cascading_power_aura::HandleBeforeHit);
        AfterHit += SpellHitFn(spell_mage_cascading_power_aura::HandleAfterHit);
    }

private:
    Unit* caster;
    float duration = 0;
};

// 1290026 - Comet Storm
class spell_mage_comet_storm : public SpellScript
{
    PrepareSpellScript(spell_mage_comet_storm);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_COMET_STORM_SPELL,
                SPELL_MAGE_COMET_STORM_COMET
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* victim = GetHitUnit();

        if (caster && victim)
            caster->m_Events.AddEventAtOffset(new SpellMageCometStormEvent(caster, victim, SPELL_MAGE_COMET_STORM_COMET, victim->GetPosition()), randtime(100ms, 275ms));

        if (caster && caster->HasAura(SPELL_MAGE_CHILLED_TO_THE_BONE))
            caster->m_Events.AddEvent(new SpellMageRemoveAuraChargeEvent(caster, SPELL_MAGE_CHILLED_TO_THE_BONE), caster->m_Events.CalculateTime(1000));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_comet_storm::HandleOnHit);
    }
};

// 1290063 - Cone of Cold
class spell_mage_cone_of_cold : public SpellScript
{
    PrepareSpellScript(spell_mage_cone_of_cold);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_CONE_OF_COLD,
                SPELL_MAGE_DIAMOND_DUST_AURA
            });
    }

    void HandleCast()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_DIAMOND_DUST_AURA))
            if (Player* player = caster->ToPlayer())
                player->RemoveCategoryCooldown(29); // Blizzard
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_cone_of_cold::HandleCast);
    }
};

// 1290048 - Crystallize
class spell_mage_crystallize_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_crystallize_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_CRYSTALLIZE_AURA,
                SPELL_MAGE_CRYSTALLIZE_SPELL
            });
    }

    void HandleOnApply()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (!player->HasActiveSpell(SPELL_MAGE_CRYSTALLIZE_SPELL))
                    player->learnSpell(SPELL_MAGE_CRYSTALLIZE_SPELL);
    }

    void HandleOnRemove()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (player->HasActiveSpell(SPELL_MAGE_CRYSTALLIZE_SPELL))
                    player->removeSpell(SPELL_MAGE_CRYSTALLIZE_SPELL, SPEC_MASK_ALL, false);
    }

    void Register() override
    {
        OnAuraApply += AuraApplyFn(spell_mage_crystallize_aura::HandleOnApply);
        OnAuraRemove += AuraRemoveFn(spell_mage_crystallize_aura::HandleOnRemove);
    }
};

// 1290040, 1290041 - Deep Freeze
class spell_mage_deep_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_deep_freeze);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_DEEP_FREEZE_PROC1,
                SPELL_MAGE_DEEP_FREEZE_PROC2
            });
    }

    void HandleOnHit()
    {
        Unit* target = GetHitUnit();

        if (target && target->GetTypeId() == TYPEID_PLAYER)
            SetHitDamage(GetHitDamage() * 0.25f);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_deep_freeze::HandleOnHit);
    }
};

// 1280056 - Displacement talent
class spell_mage_displacement_talent_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_displacement_talent_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_DISPLACEMENT_TALENT_AURA,
                SPELL_MAGE_DISPLACEMENT_TELEPORT
            });
    }

    void HandleOnApply()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (!player->HasActiveSpell(SPELL_MAGE_DISPLACEMENT_TELEPORT))
                    player->learnSpell(SPELL_MAGE_DISPLACEMENT_TELEPORT);
    }

    void HandleOnRemove()
    {
        if (Unit* caster = GetCaster())
            if (Player* player = caster->ToPlayer())
                if (player && player->HasActiveSpell(SPELL_MAGE_DISPLACEMENT_TELEPORT))
                    player->removeSpell(SPELL_MAGE_DISPLACEMENT_TELEPORT, SPEC_MASK_ALL, false);
    }

    void Register() override
    {
        OnAuraApply += AuraApplyFn(spell_mage_displacement_talent_aura::HandleOnApply);
        OnAuraRemove += AuraRemoveFn(spell_mage_displacement_talent_aura::HandleOnRemove);
    }
};

// 1280054 - Displacement: Teleport
class spell_mage_displacement_teleport : public AuraScript
{
    PrepareAuraScript(spell_mage_displacement_teleport);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_DISPLACEMENT_SUMMON,
                SPELL_MAGE_DISPLACEMENT_TELEPORT
            });
    }

    void HandleTeleport(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* target = GetTarget())
            if (Player* player = target->ToPlayer())
                if (GameObject* circle = player->GetGameObject(SPELL_MAGE_DISPLACEMENT_SUMMON))
                {
                    player->NearTeleportTo(circle->GetPositionX(), circle->GetPositionY(), circle->GetPositionZ(), circle->GetOrientation(), false, false, false, true);
                    player->RemoveAurasWithMechanic(1 << MECHANIC_SNARE);
                }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_displacement_teleport::HandleTeleport, EFFECT_0, SPELL_AURA_OBS_MOD_HEALTH, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1280055 - Displacement: Summon
class spell_mage_displacement_summon : public AuraScript
{
    PrepareAuraScript(spell_mage_displacement_summon);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_DISPLACEMENT_ALLOW_CAST_AURA,
                SPELL_MAGE_DISPLACEMENT_SUMMON,
                SPELL_MAGE_DISPLACEMENT_TELEPORT
            });
    }

    void HandleRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes mode)
    {
        Unit* target = GetTarget();

        if (target && !(mode & AURA_EFFECT_HANDLE_REAPPLY))
            target->RemoveGameObject(GetId(), true);

        if (target)
            target->RemoveAura(SPELL_MAGE_DISPLACEMENT_ALLOW_CAST_AURA);
    }

    void HandleDummyTick(AuraEffect const* /*aurEff*/)
    {
        if (GameObject* circle = GetTarget()->GetGameObject(GetId()))
        {
            Unit* target = GetTarget();

            SpellInfo const* spellInfo = sSpellMgr->AssertSpellInfo(SPELL_MAGE_DISPLACEMENT_TELEPORT);

            if (target && target->IsWithinDist(circle, spellInfo->GetMaxRange(true)))
                if (!target->HasAura(SPELL_MAGE_DISPLACEMENT_ALLOW_CAST_AURA))
                    target->CastSpell(target, SPELL_MAGE_DISPLACEMENT_ALLOW_CAST_AURA, true);
            else
                if (target)
                    target->RemoveAura(SPELL_MAGE_DISPLACEMENT_ALLOW_CAST_AURA);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_displacement_summon::HandleRemove, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_displacement_summon::HandleDummyTick, EFFECT_0, SPELL_AURA_PERIODIC_DUMMY);
    }
};

// 1300020 - Dragon's Breath
class spell_mage_dragons_breath : public SpellScript
{
    PrepareSpellScript(spell_mage_dragons_breath);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_DRAGONS_BREATH,
                SPELL_MAGE_FIRESTARTER_AURA,
                SPELL_MAGE_FIRESTARTER_BUFF,
                SPELL_MAGE_FLAMESTRIKE
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_FIRESTARTER_AURA))
        {
            caster->ToPlayer()->RemoveSpellCooldown(SPELL_MAGE_FLAMESTRIKE, false);

            if (!caster->HasAura(SPELL_MAGE_FIRESTARTER_BUFF))
                caster->CastSpell(caster, SPELL_MAGE_FIRESTARTER_BUFF, true);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_dragons_breath::HandleOnHit);
    }
};

// 1290021 - Ebonbolt
class spell_mage_ebonbolt : public SpellScript
{
    PrepareSpellScript(spell_mage_ebonbolt);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_CHILLED_TO_THE_BONE,
                SPELL_MAGE_EBONBOLT
            });
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_CHILLED_TO_THE_BONE))
            caster->GetAura(SPELL_MAGE_CHILLED_TO_THE_BONE)->DropCharge();
    }

    void Register() override
    {
        AfterHit += SpellHitFn(spell_mage_ebonbolt::HandleAfterHit);
    }
};

// 1310011 - Evocation
class spell_mage_evocation_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_evocation_aura);

    void HandleBeforeCast(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();

        if (caster)
            startMana = caster->GetPower(POWER_MANA);
    }

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANE_ASCENDANCE_AURA,
                SPELL_MAGE_ARCANE_ASCENDANCE_PROC,
                SPELL_MAGE_EVOCATION,
                SPELL_MAGE_FINGERS_OF_FROST_PROC,
                SPELL_MAGE_HOT_STREAK_PROC,
                SPELL_MAGE_MASTERY_ARCANE_MASTERY,
                SPELL_MAGE_MASTERY_FLASHBURN,
                SPELL_MAGE_MASTERY_ICICLES,
                SPELL_MAGE_MISSILE_BARRAGE_TRIGGER
            });
    }

    void HandleOnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /**/)
    {
        Unit* caster = GetCaster();

        if (caster && GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE && caster->HasAura(SPELL_MAGE_ARCANE_ASCENDANCE_AURA))
        {
            if (caster->HasAura(SPELL_MAGE_MASTERY_ARCANE_MASTERY))
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_TRIGGER, true);

            if (caster->HasAura(SPELL_MAGE_MASTERY_FLASHBURN))
                caster->CastSpell(caster, SPELL_MAGE_HOT_STREAK_PROC, true);

            if (caster->HasAura(SPELL_MAGE_MASTERY_ICICLES))
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_PROC, true);

            if (startMana)
            {
                int32 spellPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_ARCANE_ASCENDANCE_AURA)->Effects[EFFECT_0].CalcValue();
                int32 damage = int32(CalculatePct((caster->GetPower(POWER_MANA) - startMana), spellPct));

                caster->CastCustomSpell(SPELL_MAGE_ARCANE_ASCENDANCE_PROC, SPELLVALUE_BASE_POINT0, damage, caster, true);
            }
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_evocation_aura::HandleBeforeCast, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT, AURA_EFFECT_HANDLE_REAL);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_evocation_aura::HandleOnRemove, EFFECT_0, SPELL_AURA_MOD_POWER_REGEN_PERCENT, AURA_EFFECT_HANDLE_REAL);
    }

private:
    int32 startMana = 0;
};

// 1300011 - Fireball
class spell_mage_fireball : public SpellScript
{
    PrepareSpellScript(spell_mage_fireball);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FIREBALL,
                SPELL_MAGE_IMPROVED_SCORCH_DEBUFF,
                SPELL_MAGE_MISSILE_BARRAGE_AURA,
                SPELL_MAGE_MISSILE_BARRAGE_TRIGGER
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (caster && caster->HasAura(SPELL_MAGE_MISSILE_BARRAGE_AURA))
        {
            uint16 chance = sSpellMgr->GetSpellInfo(SPELL_MAGE_MISSILE_BARRAGE_AURA)->Effects[EFFECT_1].CalcValue();
            if (irand(1, 100) <= chance)
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_TRIGGER, true);
        }

        if (target && target->HasAura(SPELL_MAGE_IMPROVED_SCORCH_DEBUFF) && target->GetAura(SPELL_MAGE_IMPROVED_SCORCH_DEBUFF)->GetCaster()->GetGUID() == GetCaster()->GetGUID())
        {
            target->GetAura(SPELL_MAGE_IMPROVED_SCORCH_DEBUFF)->RefreshDuration();
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_fireball::HandleOnHit);
    }
};

// 1280005 - Frost Barrier
class spell_mage_frost_barrier_aura : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_frost_barrier_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FROST_BARRIER
            });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            int32 casterHp = caster->GetMaxHealth();
            int32 spellPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_FROST_BARRIER)->Effects[EFFECT_1].CalcValue();
            amount = int32(CalculatePct(casterHp, spellPct));
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_frost_barrier_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_frost_barrier_aura::Trigger, EFFECT_0);
    }
};

class spell_mage_frost_barrier : public SpellScript
{
    PrepareSpellScript(spell_mage_frost_barrier);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FROST_BARRIER
            });
    }

    static int32 CalculateAmount(Unit* caster)
    {
        if (caster)
        {
            int32 casterHp = caster->GetMaxHealth();
            int32 spellPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_FROST_BARRIER)->Effects[EFFECT_2].CalcValue();
            int32 amount = int32(CalculatePct(casterHp, spellPct));

            return amount;
        }

        return 0;
    }

    SpellCastResult CheckCast()
    {
        Unit* caster = GetCaster();

        if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_SCHOOL_ABSORB, (SpellFamilyNames)GetSpellInfo()->SpellFamilyName, GetSpellInfo()->SpellIconID, EFFECT_0))
        {
            int32 newAmount = CalculateAmount(caster);

            if (aurEff->GetAmount() > newAmount)
                return SPELL_FAILED_AURA_BOUNCED;
        }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_frost_barrier::CheckCast);
    }
};

// 1290057 - Frostbolt
class spell_mage_frostbolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostbolt);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_CHILLED_TO_THE_BONE,
                SPELL_MAGE_FROSTBOLT,
                SPELL_MAGE_GLACIAL_ASSAULT_AURA,
                SPELL_MAGE_GLACIAL_ASSAULT_PROC,
                SPELL_MAGE_ICE_FORM_AURA,
                SPELL_MAGE_ICICLE_AURA,
                SPELL_MAGE_ICICLE_SPELL,
                SPELL_MAGE_ICY_VEINS_ICE_FORM,
                SPELL_MAGE_MISSILE_BARRAGE_AURA,
                SPELL_MAGE_MISSILE_BARRAGE_TRIGGER
            });
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_GLACIAL_ASSAULT_AURA) && (caster->HasAura(SPELL_MAGE_ICE_FORM_AURA) || caster->HasAura(SPELL_MAGE_ICY_VEINS_ICE_FORM)))
            caster->CastSpell(caster, SPELL_MAGE_GLACIAL_ASSAULT_PROC, true);
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_MISSILE_BARRAGE_AURA))
        {
            uint16 chance = sSpellMgr->GetSpellInfo(SPELL_MAGE_MISSILE_BARRAGE_AURA)->Effects[EFFECT_1].CalcValue();
            if (irand(1, 100) <= chance)
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_TRIGGER, true);
        }
        
        if (caster && caster->HasAura(SPELL_MAGE_ICICLE_AURA))
            if (caster->GetAura(SPELL_MAGE_ICICLE_AURA)->GetStackAmount() >= 5)
                caster->CastSpell(GetHitUnit(), SPELL_MAGE_ICICLE_SPELL, true);
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_CHILLED_TO_THE_BONE))
            caster->GetAura(SPELL_MAGE_CHILLED_TO_THE_BONE)->DropCharge();
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_frostbolt::HandleOnHit);
        AfterCast += SpellCastFn(spell_mage_frostbolt::HandleAfterCast);
        AfterHit += SpellHitFn(spell_mage_frostbolt::HandleAfterHit);
    }
};

// 1290025 - Frost Bomb
class spell_mage_frost_bomb : public SpellScript
{
    PrepareSpellScript(spell_mage_frost_bomb);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_AVALANCHE_COMET_PROC,
                SPELL_MAGE_FROST_BOMB
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();
        uint32 damage = GetHitDamage();

        if (target && target == GetExplTargetUnit())
        {
            if (caster && caster->HasSpell(SPELL_MAGE_AVALANCHE_AURA))
                for (int i = 1; i < 3; ++i)
                    caster->m_Events.AddEvent(new SpellMageCastEvent(caster, target, SPELL_MAGE_AVALANCHE_COMET_PROC), caster->m_Events.CalculateTime(i * 400));

            damage *= 2;
            SetHitDamage(damage);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_frost_bomb::HandleOnHit);
    }
};

// 1280057 - Frostfire Bolt
class spell_mage_frostfire_bolt : public SpellScript
{
    PrepareSpellScript(spell_mage_frostfire_bolt);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_CHILLED_TO_THE_BONE,
                SPELL_MAGE_ARCANE_FEEDBACK,
                SPELL_MAGE_DOUBLE_TIME_AURA,
                SPELL_MAGE_FROSTFIRE_BOLT,
                SPELL_MAGE_MISSILE_BARRAGE_AURA,
                SPELL_MAGE_MISSILE_BARRAGE_TRIGGER,
                SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC
            });
    }

    void HandleOnCast()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_DOUBLE_TIME_AURA) && caster->HasAura(SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC))
            for (int i = 0; i < 2; ++i)
                caster->CastSpell(caster, SPELL_MAGE_ARCANE_FEEDBACK, true);
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_MISSILE_BARRAGE_AURA))
        {
            uint16 chance = sSpellMgr->GetSpellInfo(SPELL_MAGE_MISSILE_BARRAGE_AURA)->Effects[EFFECT_1].CalcValue();
            if (irand(1, 100) <= chance)
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_TRIGGER, true);
        }

        if (caster && caster->HasAura(SPELL_MAGE_DOUBLE_TIME_AURA) && caster->HasAura(SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC))
            caster->CastSpell(GetHitUnit(), SPELL_MAGE_FROSTFIRE_BOLT, true);
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_CHILLED_TO_THE_BONE))
            caster->GetAura(SPELL_MAGE_CHILLED_TO_THE_BONE)->DropCharge();
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_frostfire_bolt::HandleOnCast);
        OnHit += SpellHitFn(spell_mage_frostfire_bolt::HandleOnHit);
        AfterHit += SpellHitFn(spell_mage_frostfire_bolt::HandleAfterHit);
    }
};

// 1290054 - Glacial Assault
class spell_mage_glacial_assault_proc : public SpellScript
{
    PrepareSpellScript(spell_mage_glacial_assault_proc);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_GLACIAL_ASSAULT_PROC,
                SPELL_MAGE_GLACIAL_ASSAULT_GLACIAL_SPIKE
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (caster && target)
            for (int i = 1; i < 3; ++i)
                caster->m_Events.AddEvent(new SpellMageCastEvent(caster, target, SPELL_MAGE_GLACIAL_ASSAULT_GLACIAL_SPIKE), caster->m_Events.CalculateTime(i * 400));
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_glacial_assault_proc::HandleOnHit);
    }
};

// 1290020 - Glacial Spike
class spell_mage_glacial_spike : public SpellScript
{
    PrepareSpellScript(spell_mage_glacial_spike);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_CHILLED_TO_THE_BONE,
                SPELL_MAGE_FLASH_FREEZE_AURA,
                SPELL_MAGE_GLACIAL_SPIKE,
                SPELL_MAGE_ICICLE_AURA,
                SPELL_MAGE_ICICLE_VISUAL1,
                SPELL_MAGE_ICICLE_VISUAL2,
                SPELL_MAGE_ICICLE_VISUAL3,
                SPELL_MAGE_ICICLE_VISUAL4,
                SPELL_MAGE_ICICLE_VISUAL5,
                SPELL_MAGE_PERMAFROST_PROC,
                SPELL_MAGE_SPLITTING_ICE_AURA
            });
    }

    void HandleCast()
    {
        Unit* caster = GetCaster();

        if (caster)
        {
            caster->RemoveAura(SPELL_MAGE_ICICLE_AURA);
            for (int i = 0; i < 5; ++i)
                caster->RemoveAura(SPELL_MAGE_ICICLE_VISUAL1 + i);
        }
    }

    void RecalculateDamage()
    {
        /*Unit* caster = GetCaster();
        int32 damage = GetHitDamage();
        float dmgBonus = 0;

        if (caster && caster->HasAura(SPELL_MAGE_SPLITTING_ICE_AURA))
        {
            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_SPLITTING_ICE_AURA)->Effects[EFFECT_0].CalcValue();
            dmgBonus += CalculatePct(dmgBonus, bonusPct);
        }

        if (caster && caster->HasAura(SPELL_MAGE_PERMAFROST_PROC))
        {
            uint8 stackSize = caster->GetAura(SPELL_MAGE_PERMAFROST_PROC)->GetStackAmount();
            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_PERMAFROST_PROC)->Effects[EFFECT_0].CalcValue() * stackSize;
            dmgBonus += CalculatePct(dmgBonus, bonusPct);
        }

        if (caster && caster->HasAura(SPELL_MAGE_FLASH_FREEZE_AURA))
        {
            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_FLASH_FREEZE_AURA)->Effects[EFFECT_0].CalcValue();
            dmgBonus += CalculatePct(dmgBonus, bonusPct);
        }

        damage += int32(dmgBonus);
        SetHitDamage(damage);*/
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_CHILLED_TO_THE_BONE))
            caster->GetAura(SPELL_MAGE_CHILLED_TO_THE_BONE)->DropCharge();
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_glacial_spike::HandleCast);
        OnHit += SpellHitFn(spell_mage_glacial_spike::RecalculateDamage);
        AfterHit += SpellHitFn(spell_mage_glacial_spike::HandleAfterHit);
    }
};

// 1290055 - Glacial Spike
class spell_mage_glacial_spike_glacial_assault : public SpellScript
{
    PrepareSpellScript(spell_mage_glacial_spike_glacial_assault);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FLASH_FREEZE_AURA,
                SPELL_MAGE_GLACIAL_ASSAULT_GLACIAL_SPIKE,
                SPELL_MAGE_PERMAFROST_PROC,
                SPELL_MAGE_SPLITTING_ICE_AURA
            });
    }

    void RecalculateDamage()
    {
        /*Unit* caster = GetCaster();
        int32 damage = GetHitDamage();
        float dmgBonus = 0;

        if (caster && caster->HasAura(SPELL_MAGE_SPLITTING_ICE_AURA))
        {
            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_SPLITTING_ICE_AURA)->Effects[EFFECT_0].CalcValue();
            dmgBonus += CalculatePct(dmgBonus, bonusPct);
        }
        if (caster && caster->HasAura(SPELL_MAGE_PERMAFROST_PROC))
        {
            uint8 stackSize = caster->GetAura(SPELL_MAGE_PERMAFROST_PROC)->GetStackAmount();
            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_PERMAFROST_PROC)->Effects[EFFECT_0].CalcValue() * stackSize;
            dmgBonus += CalculatePct(dmgBonus, bonusPct);
        }
        if (caster && caster->HasAura(SPELL_MAGE_FLASH_FREEZE_AURA))
        {
            int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_FLASH_FREEZE_AURA)->Effects[EFFECT_0].CalcValue();
            dmgBonus += CalculatePct(dmgBonus, bonusPct);
        }

        damage = int32((damage + dmgBonus) / 2);
        SetHitDamage(damage);*/
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_glacial_spike_glacial_assault::RecalculateDamage);
    }
};

// 48108 - Hot Streak
class spell_mage_hot_streak_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_hot_streak_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_GREATER_PYROBLAST_AURA,
                SPELL_MAGE_GREATER_PYROBLAST_COUNTER,
                SPELL_MAGE_SOUL_OF_SHAZZRATHI_AURA,
                SPELL_MAGE_SOUL_OF_SHAZZRATHI_COUNTER,
                SPELL_MAGE_HOT_STREAK_PROC
            });
    }

    void HandleOnRemove()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_GREATER_PYROBLAST_AURA))
            caster->CastSpell(caster, SPELL_MAGE_GREATER_PYROBLAST_COUNTER, true);

        if (caster && caster->HasAura(SPELL_MAGE_SOUL_OF_SHAZZRATHI_AURA))
            caster->CastSpell(caster, SPELL_MAGE_SOUL_OF_SHAZZRATHI_COUNTER, true);
    }

    void Register() override
    {
        OnAuraRemove += AuraRemoveFn(spell_mage_hot_streak_aura::HandleOnRemove);
    }
};

// 1290008 - Ice Lance
class spell_mage_ice_lance : public SpellScript
{
    PrepareSpellScript(spell_mage_ice_lance);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_CHAIN_REACTION_AURA,
                SPELL_MAGE_CHAIN_REACTION_PROC,
                SPELL_MAGE_CHILLED_TO_THE_BONE,
                SPELL_MAGE_GLACIAL_SPIKE,
                SPELL_MAGE_ICE_BLADES_AURA,
                SPELL_MAGE_ICE_BLADES_ICE_LANCE,
                SPELL_MAGE_ICE_LANCE,
                SPELL_MAGE_ICICLE_AURA,
                SPELL_MAGE_ICICLE_SPELL,
                SPELL_MAGE_ICICLE_VISUAL1,
                SPELL_MAGE_ICICLE_VISUAL2,
                SPELL_MAGE_ICICLE_VISUAL3,
                SPELL_MAGE_ICICLE_VISUAL4,
                SPELL_MAGE_ICICLE_VISUAL5,
                SPELL_MAGE_ICY_VEINS_AURA,
                SPELL_MAGE_THERMAL_VOID_AURA
            });
    }

    void HandleHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (caster && caster->HasAura(SPELL_MAGE_ICICLE_AURA) && !caster->HasSpell(SPELL_MAGE_GLACIAL_SPIKE))
        {
            Aura* icicleStack = caster->GetAura(SPELL_MAGE_ICICLE_AURA);
            uint8 stacks = icicleStack->GetStackAmount();
            if (target)
                for (uint8 i = 0; i < stacks; ++i)
                {
                    caster->RemoveAuraFromStack(SPELL_MAGE_ICICLE_AURA);
                    caster->m_Events.AddEvent(new SpellMageCastEvent(caster, target, SPELL_MAGE_ICICLE_SPELL), caster->m_Events.CalculateTime(i * 200));
                    caster->RemoveAura(SPELL_MAGE_ICICLE_VISUAL1 + i);
                }
        }

        if (caster && target && caster->HasAura(SPELL_MAGE_CHAIN_REACTION_AURA) && target->HasAuraState(AURA_STATE_FROZEN))
            caster->CastSpell(caster, SPELL_MAGE_CHAIN_REACTION_PROC, true);

        if (caster && caster->HasAura(SPELL_MAGE_ICE_BLADES_AURA) && caster->HasAura(SPELL_MAGE_FINGERS_OF_FROST_PROC));
        {
            for (int i = 1; i < 3; ++i)
                caster->m_Events.AddEvent(new SpellMageCastEvent(caster, caster, SPELL_MAGE_ICE_BLADES_ICE_LANCE), caster->m_Events.CalculateTime(i * 200));
        }

        if (caster && caster->HasAura(SPELL_MAGE_THERMAL_VOID_AURA) && caster->HasAura(SPELL_MAGE_ICY_VEINS_AURA))
        {
            Aura* aura = caster->GetAura(SPELL_MAGE_ICY_VEINS_AURA);
            int32 auraAmount = aura->GetDuration();

            if (auraAmount < 20)
            {
                caster->GetAura(SPELL_MAGE_ICY_VEINS_AURA)->AddDuration(1000);
                aura->GetEffect(EFFECT_2)->SetAmount(auraAmount + 1);
            }
        }
    }

    void HandleAfterHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_CHILLED_TO_THE_BONE))
            caster->GetAura(SPELL_MAGE_CHILLED_TO_THE_BONE)->DropCharge();
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_ice_lance::HandleHit);
        AfterHit += SpellHitFn(spell_mage_ice_lance::HandleAfterHit);
    }
};

// 1290001 - Icicle
class spell_mage_icicle_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_icicle_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ICICLE_AURA,
                SPELL_MAGE_ICICLE_SPELL,
                SPELL_MAGE_ICICLE_VISUAL1,
                SPELL_MAGE_ICICLE_VISUAL2,
                SPELL_MAGE_ICICLE_VISUAL3,
                SPELL_MAGE_ICICLE_VISUAL4,
                SPELL_MAGE_ICICLE_VISUAL5
            });
    }

    void HandleOnApply()
    {
        Unit* caster = GetCaster();
        uint8 stackSize = caster->GetAura(SPELL_MAGE_ICICLE_AURA)->GetStackAmount();

        if (caster && stackSize < 5)
            caster->CastSpell(caster, SPELL_MAGE_ICICLE_VISUAL1 + stackSize, true);
    }

    void Register() override
    {
        OnAuraApply += AuraApplyFn(spell_mage_icicle_aura::HandleOnApply);
    }
};

// 12472 - Icy Veins
class spell_mage_icy_veins : public SpellScript
{
    PrepareSpellScript(spell_mage_icy_veins);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ICE_FORM_TALENT,
                SPELL_MAGE_ICY_VEINS_AURA,
                SPELL_MAGE_ICY_VEINS_ICE_FORM
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_ICE_FORM_TALENT))
            caster->CastSpell(caster, SPELL_MAGE_ICY_VEINS_ICE_FORM, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_icy_veins::HandleOnHit);
    }
};

// 1280047 - Master of Magic - Aura Counter
class spell_mage_master_of_magic_trigger : public SpellScript
{
    PrepareSpellScript(spell_mage_master_of_magic_trigger);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_MASTER_OF_MAGIC_ARCANE_AURA,
                SPELL_MAGE_MASTER_OF_MAGIC_COUNTER,
                SPELL_MAGE_MASTER_OF_MAGIC_FIRE_AURA,
                SPELL_MAGE_MASTER_OF_MAGIC_FROST_AURA,
                SPELL_MAGE_MASTER_OF_MAGIC_ICD_AURA,
                SPELL_MAGE_MASTER_OF_MAGIC_PROC_AURA
            });
    }

    void OnHitTrigger()
    {
        Unit* caster = GetCaster();

        if (caster && !caster->HasAura(SPELL_MAGE_MASTER_OF_MAGIC_ICD_AURA) && caster->HasAura(SPELL_MAGE_MASTER_OF_MAGIC_ARCANE_AURA) && caster->HasAura(SPELL_MAGE_MASTER_OF_MAGIC_FIRE_AURA) && caster->HasAura(SPELL_MAGE_MASTER_OF_MAGIC_FROST_AURA))
            caster->CastSpell(caster, SPELL_MAGE_MASTER_OF_MAGIC_PROC_AURA, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_master_of_magic_trigger::OnHitTrigger);
    }
};

// 1300024 - Meteor
class spell_mage_meteor : public SpellScript
{
    PrepareSpellScript(spell_mage_meteor);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_IGNITE,
                SPELL_MAGE_METEOR_PROC
            });
    }

    void HandleOnHit()
    {
        SpellInfo const* igniteDot = sSpellMgr->AssertSpellInfo(SPELL_MAGE_IGNITE);
        Unit* target = GetHitUnit();
        int32 amount = int32(CalculatePct(GetHitDamage(), 40) / igniteDot->GetMaxTicks());

        if (target && target->HasAura(SPELL_MAGE_IGNITE))
        {
            uint32 tickNumber = target->GetAura(SPELL_MAGE_IGNITE)->GetEffect(EFFECT_0)->GetTickNumber();
            int32 currentAmount = target->GetAura(SPELL_MAGE_IGNITE)->GetEffect(EFFECT_0)->GetAmount() / (igniteDot->GetMaxTicks() - tickNumber);
            amount += currentAmount;
        }

        GetCaster()->CastCustomSpell(SPELL_MAGE_IGNITE, SPELLVALUE_BASE_POINT0, amount, target, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_meteor::HandleOnHit);
    }
};

// 55342 - Mirror Image
class spell_mage_mirror_image_spell : public SpellScript
{
    PrepareSpellScript(spell_mage_mirror_image_spell);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_MIRROR_IMAGE_AURA,
                SPELL_MAGE_MIRROR_IMAGE_BUFF_AURA
            });
    }

    void HandleOnHit()
    {
        if (Unit* caster = GetCaster())
            caster->CastSpell(caster, SPELL_MAGE_MIRROR_IMAGE_BUFF_AURA, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_mirror_image_spell::HandleOnHit);
    }
};

// 1280030 - Mirror Image
class spell_mage_mirror_image_aura : public AuraScript  // Aleist3r: I still have no idea if this fuck works - remember, not really a coder :P
{
    PrepareAuraScript(spell_mage_mirror_image_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_MIRROR_IMAGE_BUFF_AURA
            });
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        if (Unit* caster = GetCaster())
        {
            std::list<Creature*> mirrorImages;
            caster->GetAllMinionsByEntry(mirrorImages, NPC_MAGE_MIRROR_IMAGE);

            if (!mirrorImages.empty())
                (*mirrorImages.begin())->DespawnOrUnsummon();
            else
                caster->RemoveAura(SPELL_MAGE_MIRROR_IMAGE_BUFF_AURA);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_mage_mirror_image_aura::HandleProc);
    }
};

// 1310017 - Missile Barrage
class spell_mage_missile_barrage_trigger : public SpellScript
{
    PrepareSpellScript(spell_mage_missile_barrage_trigger);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANIST_MIND_AURA,
                SPELL_MAGE_MISSILE_BARRAGE_BUFF,
                SPELL_MAGE_MISSILE_BARRAGE_NOSTACK,
                SPELL_MAGE_MISSILE_BARRAGE_STACK,
                SPELL_MAGE_MISSILE_BARRAGE_TRIGGER
            });
    }

    void HandleEffectHit()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_ARCANIST_MIND_AURA))
        {
            caster->CastSpell(caster, SPELL_MAGE_ARCANIST_MIND_BUFF, true);
            caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_STACK, true);
            caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_BUFF, true);
        }
        else
        {
            if (caster)
            {
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_NOSTACK, true);
                caster->CastSpell(caster, SPELL_MAGE_MISSILE_BARRAGE_BUFF, true);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_missile_barrage_trigger::HandleEffectHit);
    }
};

// 1310018, 1310019 - Missile Barrage
class spell_mage_missile_barrage_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_missile_barrage_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ARCANIST_MIND_BUFF,
                SPELL_MAGE_MISSILE_BARRAGE_BUFF,
                SPELL_MAGE_MISSILE_BARRAGE_NOSTACK,
                SPELL_MAGE_MISSILE_BARRAGE_STACK
            });
    }

    void AfterRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();

        if (caster)
        {
            if (caster->HasAura(SPELL_MAGE_ARCANIST_MIND_BUFF))
                caster->RemoveAura(SPELL_MAGE_ARCANIST_MIND_BUFF);

            caster->RemoveAura(SPELL_MAGE_MISSILE_BARRAGE_BUFF);
        }
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_missile_barrage_aura::AfterRemove, EFFECT_0, SPELL_AURA_ANY, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1310075 - Particle Disintegration
class spell_mage_particle_disintegration_aura : public SpellScript
{
    PrepareSpellScript(spell_mage_particle_disintegration_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_PARTICLE_DISINTEGRATION_DEBUFF
            });
    }

    void HandleOnHit()
    {
        target = GetHitUnit();

        if (target && target->HasAura(SPELL_MAGE_PARTICLE_DISINTEGRATION_DEBUFF))
            duration = target->GetAura(SPELL_MAGE_PARTICLE_DISINTEGRATION_DEBUFF)->GetDuration();
    }

    void HandleAfterHit()
    {
        if (target && target->HasAura(SPELL_MAGE_PARTICLE_DISINTEGRATION_DEBUFF))
        {
            Aura* aura = target->GetAura(SPELL_MAGE_PARTICLE_DISINTEGRATION_DEBUFF);

            if (aura->GetStackAmount() > 1)
                aura->SetDuration(duration);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_particle_disintegration_aura::HandleOnHit);
        AfterHit += SpellHitFn(spell_mage_particle_disintegration_aura::HandleAfterHit);
    }

private:
    Unit* target;
    float duration = 0;
};

// 1280033 - Permafrost
class spell_mage_permafrost_aura : public SpellScript
{
    PrepareSpellScript(spell_mage_permafrost_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_PERMAFROST_PROC
            });
    }

    void HandleBeforeHit(SpellMissInfo missInfo)
    {
        caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_PERMAFROST_PROC))
            duration = caster->GetAura(SPELL_MAGE_PERMAFROST_PROC)->GetDuration();
    }

    void HandleAfterHit()
    {
        if (caster && caster->HasAura(SPELL_MAGE_PERMAFROST_PROC))
        {
            Aura* aura = caster->GetAura(SPELL_MAGE_PERMAFROST_PROC);

            if (aura->GetStackAmount() > 1)
                aura->SetDuration(duration);
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_mage_permafrost_aura::HandleBeforeHit);
        AfterHit += SpellHitFn(spell_mage_permafrost_aura::HandleAfterHit);
    }

private:
    Unit* caster;
    float duration = 0;
};

// 1310065 - Power of the Mad Prince
class spell_mage_power_of_the_mad_prince_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_power_of_the_mad_prince_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_POWER_OF_THE_MAD_PRINCE_AURA,
                SPELL_MAGE_POWER_OF_THE_MAD_PRINCE_BUFF
            });
    }

    void OnPeriodic(AuraEffect const* /*aurEff*/)
    {
        Unit* caster = GetCaster();

        if (caster)
            if (caster->GetPowerPct(POWER_MANA) > 95.0f)
            {
                PreventDefaultAction();

                if (caster->HasAura(SPELL_MAGE_POWER_OF_THE_MAD_PRINCE_BUFF))
                    caster->RemoveAura(SPELL_MAGE_POWER_OF_THE_MAD_PRINCE_BUFF);
            }
            else
            {
                int32 missingMana = (100 - int32(caster->GetPowerPct(POWER_MANA))) * 0.2;
                caster->CastCustomSpell(SPELL_MAGE_POWER_OF_THE_MAD_PRINCE_BUFF, SPELLVALUE_BASE_POINT0, missingMana, caster, true);
            }
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_power_of_the_mad_prince_aura::OnPeriodic, EFFECT_1, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

// 1280003 - Prismatic Barrier
class spell_mage_prismatic_barrier_aura : public spell_mage_incanters_absorbtion_base_AuraScript
{
    PrepareAuraScript(spell_mage_prismatic_barrier_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_PRISMATIC_BARRIER,
                SPELL_MAGE_FORCE_BARRIER_AURA
            });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        canBeRecalculated = false;
        if (Unit* caster = GetCaster())
        {
            int32 casterHp = caster->GetMaxHealth();
            int32 spellPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_PRISMATIC_BARRIER)->Effects[EFFECT_2].CalcValue();
            amount = int32(CalculatePct(casterHp, spellPct));

            if (caster->HasAura(SPELL_MAGE_FORCE_BARRIER_AURA))
            {
                int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_FORCE_BARRIER_AURA)->Effects[EFFECT_0].CalcValue();
                amount = int32(AddPct(amount, bonusPct));
            }
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_prismatic_barrier_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
        AfterEffectAbsorb += AuraEffectAbsorbFn(spell_mage_prismatic_barrier_aura::Trigger, EFFECT_0);
    }
};

class spell_mage_prismatic_barrier : public SpellScript
{
    PrepareSpellScript(spell_mage_prismatic_barrier);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_PRISMATIC_BARRIER,
                SPELL_MAGE_FORCE_BARRIER_AURA
            });
    }

    static int32 CalculateAmount(Unit* caster)
    {
        if (caster)
        {
            int32 casterHp = caster->GetMaxHealth();
            int32 spellPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_PRISMATIC_BARRIER)->Effects[EFFECT_2].CalcValue();
            int32 amount = round(CalculatePct(casterHp, spellPct));

            if (caster->HasAura(SPELL_MAGE_FORCE_BARRIER_AURA))
            {
                int32 bonusPct = sSpellMgr->GetSpellInfo(SPELL_MAGE_FORCE_BARRIER_AURA)->Effects[EFFECT_0].CalcValue();
                amount = round(AddPct(amount, bonusPct));
            }

            return amount;
        }

        return 0;
    }

    SpellCastResult CheckCast()
    {
        if (Unit* caster = GetCaster())
            if (AuraEffect* aurEff = caster->GetAuraEffect(SPELL_AURA_SCHOOL_ABSORB, (SpellFamilyNames)GetSpellInfo()->SpellFamilyName, GetSpellInfo()->SpellIconID, EFFECT_0))
            {
                int32 newAmount = CalculateAmount(caster);

                if (aurEff->GetAmount() > newAmount)
                    return SPELL_FAILED_AURA_BOUNCED;
            }

        return SPELL_CAST_OK;
    }

    void Register() override
    {
        OnCheckCast += SpellCheckCastFn(spell_mage_prismatic_barrier::CheckCast);
    }
};

class spell_mage_prismatic_barrier_onremove_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_prismatic_barrier_onremove_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_PRISMATIC_BARRIER,
                SPELL_MAGE_PRISMATIC_BARRIER_MANA_REGEN
            });
    }

    void OnRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();

        if (caster && aurEff->GetAmount() <= 0)
            caster->CastSpell(caster, SPELL_MAGE_PRISMATIC_BARRIER_MANA_REGEN, true, nullptr, aurEff);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_prismatic_barrier_onremove_aura::OnRemove, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1300009 - Pyroblast
class spell_mage_pyroblast : public SpellScript
{
    PrepareSpellScript(spell_mage_pyroblast);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_HOT_STREAK_PROC,
                SPELL_MAGE_IMPROVED_SCORCH_DEBUFF,
                SPELL_MAGE_PYROBLAST,
                SPELL_MAGE_SUNDERING_FLAME_AURA,
                SPELL_MAGE_SUNDERING_FLAME_DEBUFF,
                SPELL_MAGE_SOUL_OF_SHAZZRATHI_BUFF,
                SPELL_MAGE_SOUL_OF_SHAZZRATHI_COMBUSTION
            });
    }

    void HandleOnHit()
    {
        Unit* caster = GetCaster();
        Unit* target = GetHitUnit();

        if (target && target->HasAura(SPELL_MAGE_IMPROVED_SCORCH_DEBUFF) && target->GetAura(SPELL_MAGE_IMPROVED_SCORCH_DEBUFF)->GetCaster()->GetGUID() == GetCaster()->GetGUID())
            target->GetAura(SPELL_MAGE_IMPROVED_SCORCH_DEBUFF)->RefreshDuration();

        if (caster && cast)
            caster->CastSpell(target, SPELL_MAGE_SUNDERING_FLAME_DEBUFF, true);
    }

    void HandleOnCast()
    {
        Unit* caster = GetCaster();

        if (caster)
        {
            if (caster->HasAura(SPELL_MAGE_HOT_STREAK_PROC) && caster->HasAura(SPELL_MAGE_SUNDERING_FLAME_AURA))
                cast = true;

            if (caster->HasAura(SPELL_MAGE_SOUL_OF_SHAZZRATHI_BUFF) && !caster->HasAura(SPELL_MAGE_HOT_STREAK_PROC))
            {
                caster->CastSpell(caster, SPELL_MAGE_SOUL_OF_SHAZZRATHI_COMBUSTION, true);
                caster->RemoveAura(SPELL_MAGE_SOUL_OF_SHAZZRATHI_BUFF);
            }
        }
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_pyroblast::HandleOnCast);
        OnHit += SpellHitFn(spell_mage_pyroblast::HandleOnHit);
    }

private:
    bool cast = false;
};

// 1290042 - Ray of Frost
class spell_mage_ray_of_frost_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_ray_of_frost_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_RAY_OF_FROST_BUFF,
                SPELL_MAGE_RAY_OF_FROST_MAIN
            });
    }

    void HandleEffectPeriodic(AuraEffect const* aurEff)
    {
        Unit* caster = GetCaster();
        
        if (caster && aurEff->GetTickNumber() > 1)
             caster->CastSpell(caster, SPELL_MAGE_RAY_OF_FROST_BUFF, true);
    }

    void Register() override
    {
        OnEffectPeriodic += AuraEffectPeriodicFn(spell_mage_ray_of_frost_aura::HandleEffectPeriodic, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL);
    }
};

class spell_mage_ray_of_frost : public SpellScript
{
    PrepareSpellScript(spell_mage_ray_of_frost);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_RAY_OF_FROST_MAIN
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([&](WorldObject* target) -> bool
            {
                Unit* unit = target->ToUnit();
                Unit* caster = GetCaster();

                if (caster)
                {
                    if (unit && unit->HasAura(SPELL_MAGE_RAY_OF_FROST_MAIN, caster->GetGUID()))
                        return true;

                    if (caster->GetDistance(target) > caster->GetDistance(mainTarget))
                        return true;
                }
            });
    }

    void HandleCast()
    {
        mainTarget = GetExplTargetUnit();
    }

    void Register() override
    {
        BeforeCast += SpellCastFn(spell_mage_ray_of_frost::HandleCast);
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_ray_of_frost::FilterTargets, EFFECT_2, TARGET_UNIT_CONE_ENEMY_24);
    }

private:
    Unit* mainTarget;
};

// 1310030 - Resonant Spark
class spell_mage_resonant_spark : public SpellScript
{
    PrepareSpellScript(spell_mage_resonant_spark);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_RESONANT_SPARK,
                SPELL_MAGE_RESONANT_SPARK_PROC_AURA,
            });
    }

    void SendMiss(SpellMissInfo missInfo)
    {
        if (missInfo != SPELL_MISS_NONE)
        {
            if (Unit* caster = GetCaster())
                if (caster->HasAura(SPELL_MAGE_RESONANT_SPARK_PROC_AURA))
                    caster->RemoveAura(SPELL_MAGE_RESONANT_SPARK_PROC_AURA);

            return;
        }
    }

    void Register() override
    {
        BeforeHit += BeforeSpellHitFn(spell_mage_resonant_spark::SendMiss);
    }
};

class spell_mage_resonant_spark_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_resonant_spark_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_FLAME_CONVERGENCE_AURA,
                SPELL_MAGE_FLAME_CONVERGENCE_PROC,
                SPELL_MAGE_RESONANT_SPARK,
                SPELL_MAGE_RESONANT_SPARK_DEBUFF_AURA,
                SPELL_MAGE_RESONANT_SPARK_PROC_AURA
            });
    }

    void HandleEffectRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        if (caster && target)
        {
            if (caster->HasAura(SPELL_MAGE_RESONANT_SPARK_PROC_AURA))
                caster->RemoveAura(SPELL_MAGE_RESONANT_SPARK_PROC_AURA);

            if (caster->HasAura(SPELL_MAGE_FLAME_CONVERGENCE_AURA))
                caster->CastSpell(target, SPELL_MAGE_FLAME_CONVERGENCE_PROC, true);
        }
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_resonant_spark_aura::HandleEffectRemove, EFFECT_2, SPELL_AURA_PERIODIC_DAMAGE, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1280051 - Ring of Frost
class spell_mage_ring_of_frost : public AuraScript      // TODO: check
{
    PrepareAuraScript(spell_mage_ring_of_frost);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_RING_OF_FROST_TICK,
                SPELL_MAGE_RING_OF_FROST_SUMMON
            });
    }

    void Apply(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        std::list<TempSummon*> minions;
        GetCaster()->GetAllSummonsByEntry(minions, sSpellMgr->GetSpellInfo(SPELL_MAGE_RING_OF_FROST_SUMMON)->GetEffect(EFFECT_0).MiscValue);

        for (TempSummon* summon : minions)
        {
            if (TempSummon* ringOfFrost = GetRingOfFrostMinion())
            {
                if (summon->GetTimer() > ringOfFrost->GetTimer())
                {
                    ringOfFrost->DespawnOrUnsummon();
                    _ringOfFrostGUID = summon->GetGUID();
                }
            }
            else
                _ringOfFrostGUID = summon->GetGUID();
        }
    }

    void Register() override
    {
        OnEffectApply += AuraEffectApplyFn(spell_mage_ring_of_frost::Apply, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL_OR_REAPPLY_MASK);
    }

private:
    TempSummon* GetRingOfFrostMinion() const
    {
        if (Creature* creature = ObjectAccessor::GetCreature(*GetOwner(), _ringOfFrostGUID))
            return creature->ToTempSummon();
        return nullptr;
    }

    ObjectGuid _ringOfFrostGUID;
};

// 1280052 - Ring of Frost
class spell_mage_ring_of_frost_freeze : public SpellScript
{
    PrepareSpellScript(spell_mage_ring_of_frost_freeze);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_RING_OF_FROST_DUMMY,
                SPELL_MAGE_RING_OF_FROST_FREEZE,
                SPELL_MAGE_RING_OF_FROST_SLOW
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        WorldLocation const* dest = GetExplTargetDest();
        float outRadius = 20.0f;
        float inRadius = 6.5f;

        targets.remove_if([dest, outRadius, inRadius](WorldObject* target)
        {
            Unit* unit = target->ToUnit();
            if (!unit)
                return true;
            return unit->HasAura(SPELL_MAGE_RING_OF_FROST_DUMMY) || unit->HasAura(SPELL_MAGE_RING_OF_FROST_FREEZE) || unit->HasAura(SPELL_MAGE_RING_OF_FROST_SLOW) || unit->GetExactDist(dest) > outRadius || unit->GetExactDist(dest) < inRadius;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_ring_of_frost_freeze::FilterTargets, EFFECT_0, TARGET_UNIT_DEST_AREA_ENEMY);
    }
};

class spell_mage_ring_of_frost_freeze_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_ring_of_frost_freeze_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_RING_OF_FROST_DUMMY,
                SPELL_MAGE_RING_OF_FROST_FREEZE,
                SPELL_MAGE_RING_OF_FROST_SLOW
            });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            if (GetCaster())
                GetCaster()->CastSpell(GetTarget(), SPELL_MAGE_RING_OF_FROST_DUMMY, true);

        if (GetTargetApplication()->GetRemoveMode() == AURA_REMOVE_BY_EXPIRE)
            if (GetCaster())
                GetCaster()->CastSpell(GetTarget(), SPELL_MAGE_RING_OF_FROST_SLOW, true);
    }

    void Register() override
    {
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_ring_of_frost_freeze_aura::OnRemove, EFFECT_0, SPELL_AURA_MOD_STUN, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1280053 - Ring of Frost
class spell_mage_ring_of_frost_slow : public SpellScript
{
    PrepareSpellScript(spell_mage_ring_of_frost_slow);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_AVALANCHE_COMET_PROC,
                SPELL_MAGE_COMET_STORM_SPELL,
                SPELL_MAGE_RING_OF_FROST_SLOW
            });
    }

    void HandleOnHit()
    {
        if (GetCaster()->HasSpell(SPELL_MAGE_COMET_STORM_SPELL))
            GetCaster()->CastSpell(GetHitUnit(), SPELL_MAGE_AVALANCHE_COMET_PROC, true);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_ring_of_frost_slow::HandleOnHit);
    }
};

// 1310064 - Risk of Runeweaver
class spell_mage_risk_of_runeweaver_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_risk_of_runeweaver_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_RISK_OF_RUNEWEAVER_PROC,
                SPELL_MAGE_SUPERCONDUCTOR_AURA,
                SPELL_MAGE_SUPERCONDUCTOR_BUFF
            });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_MAGE_SUPERCONDUCTOR_AURA) && caster->IsInCombat())
                caster->CastSpell(caster, SPELL_MAGE_SUPERCONDUCTOR_BUFF, true);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_risk_of_runeweaver_aura::OnRemove, EFFECT_0, SPELL_AURA_ADD_PCT_MODIFIER, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1310043 - Shared Power
class spell_mage_shared_power_aura : public SpellScript
{
    PrepareSpellScript(spell_mage_shared_power_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SHARED_POWER_PROC,
                SPELL_MAGE_FOCUS_MAGIC_ALLY,
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([&](WorldObject* target) -> bool
        {
            Unit* unit = target->ToUnit();

            if (unit && unit->GetGUID() != GetCaster()->GetGUID() && unit->HasAura(SPELL_MAGE_FOCUS_MAGIC_ALLY) && unit->GetAura(SPELL_MAGE_FOCUS_MAGIC_ALLY)->GetCaster() == GetCaster())
                return false;
            return true;
        });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_shared_power_aura::FilterTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
    }
};

// 1300019 - Sear
class spell_mage_sear : public SpellScript
{
    PrepareSpellScript(spell_mage_sear);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_METEOR_AURA,
                SPELL_MAGE_HOT_STREAK_AURA,
                SPELL_MAGE_HOT_STREAK_PROC,
                SPELL_MAGE_IMPACT_ZONE_AURA,
                SPELL_MAGE_SEAR
            });
    }

    void HandleOnHit()
    {
        Unit* target = GetHitUnit();
        Unit* caster = GetCaster();

        if (caster && target)
        {
            if (target == GetExplTargetUnit() && caster->HasAura(SPELL_MAGE_METEOR_AURA))
            {
                caster->m_Events.AddEvent(new SpellMageCastEvent(caster, target, SPELL_MAGE_METEOR_AURA), caster->m_Events.CalculateTime(800));

                if (caster->HasAura(SPELL_MAGE_IMPACT_ZONE_AURA))
                    caster->m_Events.AddEvent(new SpellMageCastEvent(caster, target, SPELL_MAGE_METEOR_AURA), caster->m_Events.CalculateTime(1600));
            }

            if (target == GetExplTargetUnit() && caster->HasAura(SPELL_MAGE_HOT_STREAK_AURA))
            {
                AuraEffect* counter = caster->GetAura(SPELL_MAGE_HOT_STREAK_AURA)->GetEffect(EFFECT_1);

                if (SpellIsCrit(GetSpell()))
                {
                    counter->SetAmount(counter->GetAmount() * 2);

                    if (counter->GetAmount() < 100)
                        return;
                    else
                    {
                        caster->CastSpell(caster, SPELL_MAGE_HOT_STREAK_PROC, true);
                        counter->SetAmount(25);
                    }
                }
                else
                    counter->SetAmount(25);
            }
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_sear::HandleOnHit);
    }
};

// 1280000 - Shimmer
class spell_mage_shimmer : public SpellScript
{
    PrepareSpellScript(spell_mage_shimmer);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_DISPLACEMENT_SUMMON,
                SPELL_MAGE_DISPLACEMENT_TALENT_AURA,
                SPELL_MAGE_SHIMMER,
                SPELL_MAGE_TEMPEST_BARRIER_AURA,
                SPELL_MAGE_TEMPEST_BARRIER_PROC
            });
    }

    void HandleOnCast()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_DISPLACEMENT_TALENT_AURA))
            caster->CastSpell(caster, SPELL_MAGE_DISPLACEMENT_SUMMON, true);
    }

    void HandleAfterCast()
    {
        Unit* caster = GetCaster();

        if (caster && caster->HasAura(SPELL_MAGE_TEMPEST_BARRIER_AURA))
            caster->CastSpell(caster, SPELL_MAGE_TEMPEST_BARRIER_PROC, true);
    }

    void Register() override
    {
        OnCast += SpellCastFn(spell_mage_shimmer::HandleOnCast);
        AfterCast += SpellCastFn(spell_mage_shimmer::HandleAfterCast);
    }
};

// 1300031 - Sundering Flame
class spell_mage_sundering_flame_debuff_aura : public SpellScript
{
    PrepareSpellScript(spell_mage_sundering_flame_debuff_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SUNDERING_FLAME_DEBUFF
            });
    }

    void HandleOnHit()
    {
        target = GetHitUnit();

        if (target && target->HasAura(SPELL_MAGE_SUNDERING_FLAME_DEBUFF))
            duration = target->GetAura(SPELL_MAGE_SUNDERING_FLAME_DEBUFF)->GetDuration();
    }

    void HandleAfterHit()
    {
        if (target && target->HasAura(SPELL_MAGE_SUNDERING_FLAME_DEBUFF))
        {
            Aura* aura = target->GetAura(SPELL_MAGE_SUNDERING_FLAME_DEBUFF);

            if (duration && aura->GetStackAmount() > 1)
                aura->SetDuration(duration);
        }
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_sundering_flame_debuff_aura::HandleOnHit);
        AfterHit += SpellHitFn(spell_mage_sundering_flame_debuff_aura::HandleAfterHit);
    }

private:
    Unit* target;
    float duration = 0;
};

// 1310069 - Superconductor
class spell_mage_superconductor_periodic_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_superconductor_periodic_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SUPERCONDUCTOR_AURA,
                SPELL_MAGE_SUPERCONDUCTOR_BUFF
            });
    }

    void OnRemove(AuraEffect const* /*aurEff*/, AuraEffectHandleModes /*mode*/)
    {
        if (Unit* caster = GetCaster())
            if (caster->HasAura(SPELL_MAGE_SUPERCONDUCTOR_BUFF))
                caster->RemoveAura(SPELL_MAGE_SUPERCONDUCTOR_BUFF);
    }

    void Register() override
    {
        OnEffectRemove += AuraEffectRemoveFn(spell_mage_superconductor_periodic_aura::OnRemove, EFFECT_0, SPELL_AURA_PERIODIC_TRIGGER_SPELL, AURA_EFFECT_HANDLE_REAL);
    }
};

// 1310053 - Supernova
class spell_mage_supernova : public SpellScript
{
    PrepareSpellScript(spell_mage_supernova);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_SUPERNOVA
            });
    }

    void HandleOnHit()
    {
        if (Unit* target = GetHitUnit())
            if (target == GetExplTargetUnit())
                SetHitDamage(GetHitDamage() * 2);
    }

    void Register() override
    {
        OnHit += SpellHitFn(spell_mage_supernova::HandleOnHit);
    }
};

// 1280014 - Tempest Barrier
class spell_mage_tempest_barrier_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_tempest_barrier_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_TEMPEST_BARRIER_PROC
            });
    }

    void CalculateAmount(AuraEffect const* /*aurEff*/, int32& amount, bool& canBeRecalculated)
    {
        if (Unit* caster = GetCaster())
        {
            canBeRecalculated = false;
            int32 pct = sSpellMgr->GetSpellInfo(SPELL_MAGE_TEMPEST_BARRIER_PROC)->Effects[EFFECT_1].CalcValue();
            amount = int32(CalculatePct(caster->GetMaxHealth(), pct));
        }
    }

    void Register() override
    {
        DoEffectCalcAmount += AuraEffectCalcAmountFn(spell_mage_tempest_barrier_aura::CalculateAmount, EFFECT_0, SPELL_AURA_SCHOOL_ABSORB);
    }
};

// 1280040 - Time Ward
class spell_mage_time_ward_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_time_ward_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_TIME_WARD_AURA,
                SPELL_MAGE_TIME_WARD_PROC,
                SPELL_MAGE_TIME_WARD_PROC_FINISH
            });
    }

    void HandleProc(ProcEventInfo& eventInfo)
    {
        PreventDefaultAction();

        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        if (caster && target)
        {
            if (counter > 1)
            {
                caster->CastSpell(target, SPELL_MAGE_TIME_WARD_PROC, true);
                --counter;
            }
            else
            {
                caster->CastSpell(target, SPELL_MAGE_TIME_WARD_PROC_FINISH, true);
                --counter;
            }

            if (caster->HasAura(SPELL_MAGE_MASTERY_ARCANE_MASTERY))
                caster->CastSpell(caster, SPELL_MAGE_CLEARCASTING);

            if (caster->HasAura(SPELL_MAGE_MASTERY_FLASHBURN))
                caster->ToPlayer()->ModifySpellCooldown(SPELL_MAGE_FIRE_BLAST, -6000);

            if (caster->HasAura(SPELL_MAGE_MASTERY_ICICLES))
                caster->CastSpell(caster, SPELL_MAGE_FINGERS_OF_FROST_PROC);
        }
    }

    void Register() override
    {
        OnProc += AuraProcFn(spell_mage_time_ward_aura::HandleProc);
    }

private:
    uint8 counter = sSpellMgr->GetSpellInfo(SPELL_MAGE_TIME_WARD_AURA)->ProcCharges;
};

// 1280001 - Time Warp
class spell_mage_timewarp : public SpellScript
{
    PrepareSpellScript(spell_mage_timewarp);

    bool Validate(SpellInfo const* /*spellInfo*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_TEMPORAL_DISPLACEMENT,
                SPELL_MAGE_TEMPORAL_WARP_AURA,
                SPELL_MAGE_TIME_WARP,
                SPELL_SHAMAN_EXHAUSTION,
                SPELL_SHAMAN_SATED
            });
    }

    void RemoveInvalidTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([&](WorldObject const* target)
        {
            if (Unit const* unitTarget = target->ToUnit())
            {
                if (unitTarget == GetCaster() && GetCaster()->HasAura(SPELL_MAGE_TEMPORAL_WARP_AURA))
                    return false;
                else
                {
                    if (unitTarget->HasAura(SPELL_MAGE_TEMPORAL_DISPLACEMENT) || unitTarget->HasAura(SPELL_SHAMAN_EXHAUSTION) || unitTarget->HasAura(SPELL_SHAMAN_SATED))
                        return true;

                    return false;
                }
            }
        });
    }

    void ApplyDebuff()
    {
        if (Unit* target = GetHitUnit())
            target->CastSpell(target, SPELL_MAGE_TEMPORAL_DISPLACEMENT, true);
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_timewarp::RemoveInvalidTargets, EFFECT_ALL, TARGET_UNIT_CASTER_AREA_RAID);
        AfterHit += SpellHitFn(spell_mage_timewarp::ApplyDebuff);
    }
};

// 1310015 - Touch of the Magi
class spell_mage_touch_of_the_magi_aura : public AuraScript
{
    PrepareAuraScript(spell_mage_touch_of_the_magi_aura);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ECHO_OF_ANTONIDAS_AURA,
                SPELL_MAGE_ECHO_OF_ANTONIDAS_TOUCH_OF_THE_MAGI,
                SPELL_MAGE_RHONINS_RETORT_AURA,
                SPELL_MAGE_RHONINS_RETORT_TOUCH_OF_THE_MAGI,
                SPELL_MAGE_TOUCH_OF_THE_MAGI_AURA,
                SPELL_MAGE_TOUCH_OF_THE_MAGI_EXPLOSION
            });
    }

    void HandleProc(AuraEffect const* /*aurEff*/, ProcEventInfo& eventInfo)
    {
        DamageInfo* damageInfo = eventInfo.GetDamageInfo();
        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        if (caster && target)
            if (damageInfo)
            {
                if (damageInfo->GetAttacker() == caster && damageInfo->GetVictim() == target)
                    damageTaken += damageInfo->GetDamage();

                if (caster->HasAura(SPELL_MAGE_ECHO_OF_ANTONIDAS_AURA))
                    if (damageInfo->GetDamageType() == DIRECT_DAMAGE || damageInfo->GetDamageType() == SPELL_DIRECT_DAMAGE)
                        caster->CastSpell(target, SPELL_MAGE_ECHO_OF_ANTONIDAS_TOUCH_OF_THE_MAGI, true);
            }
    }

    void AfterRemove(AuraEffect const* aurEff, AuraEffectHandleModes /*mode*/)
    {
        int32 damage = CalculatePct(damageTaken, aurEff->GetAmount());
        Unit* caster = GetCaster();
        Unit* target = GetTarget();

        if (!damage || GetTargetApplication()->GetRemoveMode() != AURA_REMOVE_BY_EXPIRE)
            return;

        if (caster && target)
        {
            caster->CastCustomSpell(target, SPELL_MAGE_TOUCH_OF_THE_MAGI_EXPLOSION, &damage, nullptr, nullptr, true);

            if (caster->HasAura(SPELL_MAGE_RHONINS_RETORT_AURA))
            {
                damage /= 2;
                caster->CastCustomSpell(target, SPELL_MAGE_RHONINS_RETORT_TOUCH_OF_THE_MAGI, &damage, nullptr, nullptr, true);
            }
        }
    }

    void Register() override
    {
        OnEffectProc += AuraEffectProcFn(spell_mage_touch_of_the_magi_aura::HandleProc, EFFECT_0, SPELL_AURA_DUMMY);
        AfterEffectRemove += AuraEffectRemoveFn(spell_mage_touch_of_the_magi_aura::AfterRemove, EFFECT_0, SPELL_AURA_DUMMY, AURA_EFFECT_HANDLE_REAL);
    }

private:
    int32 damageTaken = 0;
};

// 1310046, 1310062 - Touch of the Magi
class spell_mage_touch_of_the_magi_talent_procs : public SpellScript
{
    PrepareSpellScript(spell_mage_touch_of_the_magi_talent_procs);

    bool Validate(SpellInfo const* /*spellEntry*/) override
    {
        return ValidateSpellInfo(
            {
                SPELL_MAGE_ECHO_OF_ANTONIDAS_TOUCH_OF_THE_MAGI,
                SPELL_MAGE_RHONINS_RETORT_TOUCH_OF_THE_MAGI
            });
    }

    void FilterTargets(std::list<WorldObject*>& targets)
    {
        targets.remove_if([&](WorldObject* target) -> bool
            {
                Unit* unit = target->ToUnit();

                if (unit == GetExplTargetUnit())
                    return true;
                return false;
            });
    }

    void Register() override
    {
        OnObjectAreaTargetSelect += SpellObjectAreaTargetSelectFn(spell_mage_touch_of_the_magi_talent_procs::FilterTargets, EFFECT_0, TARGET_UNIT_TARGET_ENEMY);
    }
};

void AddSC_mage_spell_scripts()
{
    RegisterSpellScript(spell_mage_blazing_speed);
    RegisterSpellScript(spell_mage_arcane_blast);
    RegisterSpellScript(spell_mage_burning_determination);
    RegisterSpellScript(spell_mage_molten_armor);
    RegisterSpellScript(spell_mage_mirror_image);
    RegisterSpellScript(spell_mage_burnout);
    RegisterSpellScript(spell_mage_burnout_trigger);
    RegisterSpellScript(spell_mage_pet_scaling);
    RegisterSpellScript(spell_mage_brain_freeze);
    RegisterSpellScript(spell_mage_glyph_of_eternal_water);
    RegisterSpellScript(spell_mage_combustion_proc);
    RegisterSpellScript(spell_mage_blast_wave);
    RegisterSpellScript(spell_mage_cold_snap);
    RegisterSpellScript(spell_mage_fire_frost_ward);
    RegisterSpellScript(spell_mage_focus_magic);
    RegisterSpellScript(spell_mage_ice_barrier);
    RegisterSpellScript(spell_mage_ignite);
    RegisterSpellScript(spell_mage_living_bomb);
    RegisterSpellScript(spell_mage_mana_shield);
    RegisterSpellScript(spell_mage_master_of_elements);
    RegisterSpellScript(spell_mage_polymorph_cast_visual);
    RegisterSpellScript(spell_mage_summon_water_elemental);
    // RegisterSpellScript(spell_mage_fingers_of_frost_proc_aura);    // Probably not needed anymore
    // RegisterSpellScript(spell_mage_fingers_of_frost_proc);         // And this one as well
    RegisterSpellScript(spell_mage_arcane_potency);
    RegisterSpellScript(spell_mage_combustion);
    RegisterSpellScript(spell_mage_imp_blizzard);
    RegisterSpellScript(spell_mage_imp_mana_gems);
    RegisterSpellScript(spell_mage_empowered_fire);
    RegisterSpellScript(spell_mage_fingers_of_frost);
    RegisterSpellScript(spell_mage_gen_extra_effects);
    RegisterSpellScript(spell_mage_glyph_of_polymorph);
    RegisterSpellScript(spell_mage_glyph_of_icy_veins);
    RegisterSpellScript(spell_mage_hot_streak);
    RegisterSpellScript(spell_mage_magic_absorption);
    RegisterSpellScript(spell_mage_missile_barrage);

    // Duskhaven
    RegisterSpellScript(spell_mage_arcane_barrage);
    RegisterSpellScript(spell_mage_arcane_explosion);
    RegisterSpellScript(spell_mage_arcane_missiles);
    RegisterSpellScript(spell_mage_arcane_surge);
    RegisterSpellScript(spell_mage_avalanche_comet);
    RegisterSpellScript(spell_mage_barriers_onproc_aura);
    RegisterSpellScript(spell_mage_blazing_barrier_aura);
    RegisterSpellScript(spell_mage_blazing_barrier);
    RegisterSpellScript(spell_mage_blazing_barrier_onremove_aura);
    RegisterSpellScript(spell_mage_blink);
    RegisterSpellScript(spell_mage_cascading_power_aura);
    RegisterSpellScript(spell_mage_comet_storm);
    RegisterSpellScript(spell_mage_cone_of_cold);
    RegisterSpellScript(spell_mage_crystallize_aura);
    RegisterSpellScript(spell_mage_deep_freeze);
    RegisterSpellScript(spell_mage_displacement_talent_aura);
    RegisterSpellScript(spell_mage_displacement_teleport);
    RegisterSpellScript(spell_mage_displacement_summon);
    RegisterSpellScript(spell_mage_dragons_breath);
    RegisterSpellScript(spell_mage_ebonbolt);
    RegisterSpellScript(spell_mage_evocation_aura);
    RegisterSpellScript(spell_mage_fireball);
    RegisterSpellScript(spell_mage_frost_barrier_aura);
    RegisterSpellScript(spell_mage_frost_barrier);
    RegisterSpellScript(spell_mage_frostbolt);
    RegisterSpellScript(spell_mage_frost_bomb);
    RegisterSpellScript(spell_mage_frostfire_bolt);
    RegisterSpellScript(spell_mage_glacial_assault_proc);
    RegisterSpellScript(spell_mage_glacial_spike);
    RegisterSpellScript(spell_mage_glacial_spike_glacial_assault);
    RegisterSpellScript(spell_mage_hot_streak_aura);
    RegisterSpellScript(spell_mage_ice_lance);
    RegisterSpellScript(spell_mage_icicle_aura);
    RegisterSpellScript(spell_mage_icy_veins);
    RegisterSpellScript(spell_mage_master_of_magic_trigger);
    RegisterSpellScript(spell_mage_meteor);
    RegisterSpellScript(spell_mage_mirror_image_aura);
    RegisterSpellScript(spell_mage_mirror_image_spell);
    RegisterSpellScript(spell_mage_missile_barrage_trigger);
    RegisterSpellScript(spell_mage_missile_barrage_aura);
    RegisterSpellScript(spell_mage_particle_disintegration_aura);
    RegisterSpellScript(spell_mage_permafrost_aura);
    RegisterSpellScript(spell_mage_power_of_the_mad_prince_aura);
    RegisterSpellScript(spell_mage_prismatic_barrier_aura);
    RegisterSpellScript(spell_mage_prismatic_barrier);
    RegisterSpellScript(spell_mage_prismatic_barrier_onremove_aura);
    RegisterSpellScript(spell_mage_pyroblast);
    RegisterSpellScript(spell_mage_ray_of_frost_aura);
    RegisterSpellScript(spell_mage_ray_of_frost);
    RegisterSpellScript(spell_mage_resonant_spark);
    RegisterSpellScript(spell_mage_resonant_spark_aura);
    RegisterSpellScript(spell_mage_ring_of_frost);
    RegisterSpellScript(spell_mage_ring_of_frost_freeze);
    RegisterSpellScript(spell_mage_ring_of_frost_freeze_aura);
    RegisterSpellScript(spell_mage_ring_of_frost_slow);
    RegisterSpellScript(spell_mage_risk_of_runeweaver_aura);
    RegisterSpellScript(spell_mage_shared_power_aura);
    RegisterSpellScript(spell_mage_sear);
    RegisterSpellScript(spell_mage_shimmer);
    RegisterSpellScript(spell_mage_sundering_flame_debuff_aura);
    RegisterSpellScript(spell_mage_superconductor_periodic_aura);
    RegisterSpellScript(spell_mage_supernova);
    RegisterSpellScript(spell_mage_tempest_barrier_aura);
    RegisterSpellScript(spell_mage_time_ward_aura);
    RegisterSpellScript(spell_mage_timewarp);
    RegisterSpellScript(spell_mage_touch_of_the_magi_aura);
    RegisterSpellScript(spell_mage_touch_of_the_magi_talent_procs);
}
